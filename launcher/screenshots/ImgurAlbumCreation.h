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

#include "net/NetAction.h"
#include "Screenshot.h"

typedef shared_qobject_ptr<class ImgurAlbumCreation> ImgurAlbumCreationPtr;
class ImgurAlbumCreation : public NetAction
{
public:
    explicit ImgurAlbumCreation(QList<ScreenShot::Ptr> screenshots);
    static ImgurAlbumCreationPtr make(QList<ScreenShot::Ptr> screenshots)
    {
        return ImgurAlbumCreationPtr(new ImgurAlbumCreation(screenshots));
    }

    QString deleteHash() const
    {
        return m_deleteHash;
    }
    QString id() const
    {
        return m_id;
    }

protected
slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal) override;
    void downloadError(QNetworkReply::NetworkError error) override;
    void downloadFinished() override;
    void downloadReadyRead() override {}

public
slots:
    void executeTask() override;

private:
    QList<ScreenShot::Ptr> m_screenshots;

    QString m_deleteHash;
    QString m_id;
};
