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

#include "AccountTask.h"
#include "MinecraftAccount.h"

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QByteArray>

#include <QDebug>

AccountTask::AccountTask(AccountData *data, QObject *parent)
    : Task(parent), m_data(data)
{
    changeState(AccountTaskState::STATE_CREATED);
}

QString AccountTask::getStateMessage() const
{
    switch (m_taskState)
    {
    case AccountTaskState::STATE_CREATED:
        return "Waiting...";
    case AccountTaskState::STATE_WORKING:
        return tr("Sending request to auth servers...");
    case AccountTaskState::STATE_SUCCEEDED:
        return tr("Authentication task succeeded.");
    case AccountTaskState::STATE_OFFLINE:
        return tr("Failed to contact the authentication server.");
    case AccountTaskState::STATE_DISABLED:
        return tr("Client ID has changed. New session needs to be created.");
    case AccountTaskState::STATE_FAILED_SOFT:
        return tr("Encountered an error during authentication.");
    case AccountTaskState::STATE_FAILED_HARD:
        return tr("Failed to authenticate. The session has expired.");
    case AccountTaskState::STATE_FAILED_GONE:
        return tr("Failed to authenticate. The account no longer exists.");
    default:
        return tr("...");
    }
}

bool AccountTask::changeState(AccountTaskState newState, QString reason)
{
    m_taskState = newState;
    // FIXME: virtual method invoked in constructor.
    // We want that behavior, but maybe make it less weird?
    setStatus(getStateMessage());
    switch(newState) {
        case AccountTaskState::STATE_CREATED: {
            m_data->errorString.clear();
            return true;
        }
        case AccountTaskState::STATE_WORKING: {
            m_data->accountState = AccountState::Working;
            return true;
        }
        case AccountTaskState::STATE_SUCCEEDED: {
            m_data->accountState = AccountState::Online;
            emitSucceeded();
            return false;
        }
        case AccountTaskState::STATE_OFFLINE: {
            m_data->errorString = reason;
            m_data->accountState = AccountState::Offline;
            emitFailed(reason);
            return false;
        }
        case AccountTaskState::STATE_DISABLED: {
            m_data->errorString = reason;
            m_data->accountState = AccountState::Disabled;
            emitFailed(reason);
            return false;
        }
        case AccountTaskState::STATE_FAILED_SOFT: {
            m_data->errorString = reason;
            m_data->accountState = AccountState::Errored;
            emitFailed(reason);
            return false;
        }
        case AccountTaskState::STATE_FAILED_HARD: {
            m_data->errorString = reason;
            m_data->accountState = AccountState::Expired;
            emitFailed(reason);
            return false;
        }
        case AccountTaskState::STATE_FAILED_GONE: {
            m_data->errorString = reason;
            m_data->accountState = AccountState::Gone;
            emitFailed(reason);
            return false;
        }
        default: {
            QString error = tr("Unknown account task state: %1").arg(int(newState));
            m_data->accountState = AccountState::Errored;
            emitFailed(error);
            return false;
        }
    }
}
