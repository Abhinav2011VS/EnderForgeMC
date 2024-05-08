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

#pragma once

#include "tasks/Task.h"
#include <QNetworkReply>
#include <QString>
#include <QBuffer>
#include <memory>
#include <array>

class PasteUpload : public Task
{
    Q_OBJECT
public:
    enum PasteType : int {
        // 0x0.st
        NullPointer,
        // hastebin.com
        Hastebin,
        // paste.gg
        PasteGG,
        // mclo.gs
        Mclogs,
        // Helpful to get the range of valid values on the enum for input sanitisation:
        First = NullPointer,
        Last = Mclogs
    };

    struct PasteTypeInfo {
        const QString name;
        const QString defaultBase;
        const QString endpointPath;
    };

    static std::array<PasteTypeInfo, 4> PasteTypes;

    PasteUpload(QWidget *window, QString text, QString url, PasteType pasteType);
    virtual ~PasteUpload();

    QString pasteLink()
    {
        return m_pasteLink;
    }
protected:
    virtual void executeTask();

private:
    QWidget *m_window;
    QString m_pasteLink;
    QString m_baseUrl;
    QString m_uploadUrl;
    PasteType m_pasteType;
    QByteArray m_text;
    std::shared_ptr<QNetworkReply> m_reply;
public
slots:
    void downloadError(QNetworkReply::NetworkError);
    void downloadFinished();
};
