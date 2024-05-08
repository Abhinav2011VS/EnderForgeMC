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

#include <QSaveFile>

#include "Sink.h"

namespace Net {
class FileSink : public Sink {
   public:
    FileSink(QString filename) : m_filename(filename){};
    virtual ~FileSink() = default;

   public:
    auto init(QNetworkRequest& request) -> Task::State override;
    auto write(QByteArray& data) -> Task::State override;
    auto abort() -> Task::State override;
    auto finalize(QNetworkReply& reply) -> Task::State override;

    auto hasLocalData() -> bool override;

   protected:
    virtual auto initCache(QNetworkRequest&) -> Task::State;
    virtual auto finalizeCache(QNetworkReply& reply) -> Task::State;

   protected:
    QString m_filename;
    bool wroteAnyData = false;
    std::unique_ptr<QSaveFile> m_output_file;
};
}  // namespace Net
