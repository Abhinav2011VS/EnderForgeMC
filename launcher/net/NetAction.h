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

#pragma once

#include <QNetworkReply>
#include <QUrl>

#include "QObjectPtr.h"
#include "tasks/Task.h"

class NetAction : public Task {
    Q_OBJECT
   protected:
    explicit NetAction() : Task() {};

   public:
    using Ptr = shared_qobject_ptr<NetAction>;

    virtual ~NetAction() = default;

    QUrl url() { return m_url; }
    auto index() -> int { return m_index_within_job; }

    void setNetwork(shared_qobject_ptr<QNetworkAccessManager> network) { m_network = network; }

   protected slots:
    virtual void downloadProgress(qint64 bytesReceived, qint64 bytesTotal) = 0;
    virtual void downloadError(QNetworkReply::NetworkError error) = 0;
    virtual void downloadFinished() = 0;
    virtual void downloadReadyRead() = 0;

   public slots:
    void startAction(shared_qobject_ptr<QNetworkAccessManager> network)
    {
        m_network = network;
        executeTask();
    }

   protected:
    void executeTask() override {};

   public:
    shared_qobject_ptr<QNetworkAccessManager> m_network;

    /// index within the parent job, FIXME: nuke
    int m_index_within_job = 0;

    /// the network reply
    unique_qobject_ptr<QNetworkReply> m_reply;

    /// source URL
    QUrl m_url;
};
