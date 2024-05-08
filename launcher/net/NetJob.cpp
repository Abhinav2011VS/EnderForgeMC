// SPDX-License-Identifier: GPL-3.0-only
/*
 *  EnderForgeMC - Minecraft Launcher
 *  Copyright (c) 2024 Abhinav VS
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

#include "NetJob.h"

auto NetJob::addNetAction(NetAction::Ptr action) -> bool
{
    action->m_index_within_job = m_queue.size();
    m_queue.append(action);

    action->setNetwork(m_network);

    return true;
}

void NetJob::startNext()
{
    if (m_queue.isEmpty() && m_doing.isEmpty()) {
        // We're finished, check for failures and retry if we can (up to 3 times)
        if (!m_failed.isEmpty() && m_try < 3) {
            m_try += 1;
            while (!m_failed.isEmpty())
                m_queue.enqueue(m_failed.take(*m_failed.keyBegin()));
        }
    }

    ConcurrentTask::startNext();
}

auto NetJob::size() const -> int
{
    return m_queue.size() + m_doing.size() + m_done.size();
}

auto NetJob::canAbort() const -> bool
{
    bool canFullyAbort = true;

    // can abort the downloads on the queue?
    for (auto part : m_queue)
        canFullyAbort &= part->canAbort();

    // can abort the active downloads?
    for (auto part : m_doing)
        canFullyAbort &= part->canAbort();

    return canFullyAbort;
}

auto NetJob::abort() -> bool
{
    bool fullyAborted = true;

    // fail all downloads on the queue
    for (auto task : m_queue)
        m_failed.insert(task.get(), task);
    m_queue.clear();

    // abort active downloads
    auto toKill = m_doing.values();
    for (auto part : toKill) {
        fullyAborted &= part->abort();
    }

    if (fullyAborted)
        emitAborted();
    else
        emitFailed(tr("Failed to abort all tasks in the NetJob!"));

    return fullyAborted;
}

auto NetJob::getFailedActions() -> QList<NetAction*>
{
    QList<NetAction*> failed;
    for (auto index : m_failed) {
        failed.push_back(dynamic_cast<NetAction*>(index.get()));
    }
    return failed;
}

auto NetJob::getFailedFiles() -> QList<QString>
{
    QList<QString> failed;
    for (auto index : m_failed) {
        failed.append(static_cast<NetAction*>(index.get())->url().toString());
    }
    return failed;
}

void NetJob::updateState()
{
    emit progress(m_done.count(), m_total_size);
    setStatus(tr("Executing %1 task(s) (%2 out of %3 are done)")
                  .arg(QString::number(m_doing.count()), QString::number(m_done.count()), QString::number(m_total_size)));
}
