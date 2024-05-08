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

#include "NetAction.h"
#include "Sink.h"

namespace Net {

    class Upload : public NetAction {
        Q_OBJECT

    public:
        static Upload::Ptr makeByteArray(QUrl url, QByteArray *output, QByteArray m_post_data);
        auto abort() -> bool override;
        auto canAbort() const -> bool override { return true; };

    protected slots:
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal) override;
        void downloadError(QNetworkReply::NetworkError error) override;
        void sslErrors(const QList<QSslError> & errors);
        void downloadFinished() override;
        void downloadReadyRead() override;

    public slots:
        void executeTask() override;
    private:
        std::unique_ptr<Sink> m_sink;
        QByteArray m_post_data;

        bool handleRedirect();
    };

} // Net

