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

class ImgurUpload : public NetAction {
public:
    using Ptr = shared_qobject_ptr<ImgurUpload>;

    explicit ImgurUpload(ScreenShot::Ptr shot);
    static Ptr make(ScreenShot::Ptr shot) {
        return Ptr(new ImgurUpload(shot));
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
    ScreenShot::Ptr m_shot;
    bool finished = true;
};
