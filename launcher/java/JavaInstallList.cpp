// SPDX-License-Identifier: GPL-3.0-only
/*
 *  EnderForgeMC - Minecraft Launcher
 *  Copyright (C) 2024 Abhinav VS
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QtNetwork>
#include <QtXml>

#include <QDebug>

#include "java/JavaInstallList.h"
#include "java/JavaCheckerJob.h"
#include "java/JavaUtils.h"
#include "MMCStrings.h"
#include "minecraft/VersionFilterData.h"

JavaInstallList::JavaInstallList(QObject *parent) : BaseVersionList(parent)
{
}

Task::Ptr JavaInstallList::getLoadTask()
{
    load();
    return getCurrentTask();
}

Task::Ptr JavaInstallList::getCurrentTask()
{
    if(m_status == Status::InProgress)
    {
        return m_loadTask;
    }
    return nullptr;
}

void JavaInstallList::load()
{
    if(m_status != Status::InProgress)
    {
        m_status = Status::InProgress;
        m_loadTask = new JavaListLoadTask(this);
        m_loadTask->start();
    }
}

const BaseVersionPtr JavaInstallList::at(int i) const
{
    return m_vlist.at(i);
}

bool JavaInstallList::isLoaded()
{
    return m_status == JavaInstallList::Status::Done;
}

int JavaInstallList::count() const
{
    return m_vlist.count();
}

QVariant JavaInstallList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > count())
        return QVariant();

    auto version = std::dynamic_pointer_cast<JavaInstall>(m_vlist[index.row()]);
    switch (role)
    {
        case VersionPointerRole:
            return QVariant::fromValue(m_vlist[index.row()]);
        case VersionIdRole:
            return version->descriptor();
        case VersionRole:
            return version->id.toString();
        case RecommendedRole:
            return version->recommended;
        case PathRole:
            return version->path;
        case ArchitectureRole:
            return version->arch;
        default:
            return QVariant();
    }
}

BaseVersionList::RoleList JavaInstallList::providesRoles() const
{
    return {VersionPointerRole, VersionIdRole, VersionRole, RecommendedRole, PathRole, ArchitectureRole};
}


void JavaInstallList::updateListData(QList<BaseVersionPtr> versions)
{
    beginResetModel();
    m_vlist = versions;
    sortVersions();
    if(m_vlist.size())
    {
        auto best = std::dynamic_pointer_cast<JavaInstall>(m_vlist[0]);
        best->recommended = true;
    }
    endResetModel();
    m_status = Status::Done;
    m_loadTask.reset();
}

bool sortJavas(BaseVersionPtr left, BaseVersionPtr right)
{
    auto rleft = std::dynamic_pointer_cast<JavaInstall>(right);
    auto rright = std::dynamic_pointer_cast<JavaInstall>(left);
    return (*rleft) > (*rright);
}

void JavaInstallList::sortVersions()
{
    beginResetModel();
    std::sort(m_vlist.begin(), m_vlist.end(), sortJavas);
    endResetModel();
}

JavaListLoadTask::JavaListLoadTask(JavaInstallList *vlist) : Task()
{
    m_list = vlist;
    m_currentRecommended = NULL;
}

JavaListLoadTask::~JavaListLoadTask()
{
}

void JavaListLoadTask::executeTask()
{
    setStatus(tr("Detecting Java installations..."));

    JavaUtils ju;
    QList<QString> candidate_paths = ju.FindJavaPaths();

    m_job = new JavaCheckerJob("Java detection");
    connect(m_job.get(), &Task::finished, this, &JavaListLoadTask::javaCheckerFinished);
    connect(m_job.get(), &Task::progress, this, &Task::setProgress);

    qDebug() << "Probing the following Java paths: ";
    int id = 0;
    for(QString candidate : candidate_paths)
    {
        qDebug() << " " << candidate;

        auto candidate_checker = new JavaChecker();
        candidate_checker->m_path = candidate;
        candidate_checker->m_id = id;
        m_job->addJavaCheckerAction(JavaCheckerPtr(candidate_checker));

        id++;
    }

    m_job->start();
}

void JavaListLoadTask::javaCheckerFinished()
{
    QList<JavaInstallPtr> candidates;
    auto results = m_job->getResults();

    qDebug() << "Found the following valid Java installations:";
    for(JavaCheckResult result : results)
    {
        if(result.validity == JavaCheckResult::Validity::Valid)
        {
            JavaInstallPtr javaVersion(new JavaInstall());

            javaVersion->id = result.javaVersion;
            javaVersion->arch = result.realPlatform;
            javaVersion->path = result.path;
            candidates.append(javaVersion);

            qDebug() << " " << javaVersion->id.toString() << javaVersion->arch << javaVersion->path;
        }
    }

    QList<BaseVersionPtr> javas_bvp;
    for (auto java : candidates)
    {
        //qDebug() << java->id << java->arch << " at " << java->path;
        BaseVersionPtr bp_java = std::dynamic_pointer_cast<BaseVersion>(java);

        if (bp_java)
        {
            javas_bvp.append(java);
        }
    }

    m_list->updateListData(javas_bvp);
    emitSucceeded();
}
