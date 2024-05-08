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

#include <QtNetwork>

#include <QObject>
#include "NetAction.h"
#include "tasks/ConcurrentTask.h"

// Those are included so that they are also included by anyone using NetJob
#include "net/Download.h"
#include "net/HttpMetaCache.h"

class NetJob : public ConcurrentTask {
    Q_OBJECT

   public:
    using Ptr = shared_qobject_ptr<NetJob>;

    explicit NetJob(QString job_name, shared_qobject_ptr<QNetworkAccessManager> network) : ConcurrentTask(nullptr, job_name), m_network(network) {}
    ~NetJob() override = default;

    void startNext() override;

    auto size() const -> int;

    auto canAbort() const -> bool override;
    auto addNetAction(NetAction::Ptr action) -> bool;

    auto getFailedActions() -> QList<NetAction*>;
    auto getFailedFiles() -> QList<QString>;

   public slots:
    // Qt can't handle auto at the start for some reason?
    bool abort() override;

   protected:
    void updateState() override;

   private:
    shared_qobject_ptr<QNetworkAccessManager> m_network;

    int m_try = 1;
};
