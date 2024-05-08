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

#include <QDateTime>
#include <QFileInfo>
#include <QList>

#include "Resource.h"
#include "ModDetails.h"

class Mod : public Resource
{
    Q_OBJECT
public:
    using Ptr = shared_qobject_ptr<Mod>;
    using WeakPtr = QPointer<Mod>;

    Mod() = default;
    Mod(const QFileInfo &file);
    Mod(const QDir& mods_dir, const Metadata::ModStruct& metadata);
    Mod(QString file_path) : Mod(QFileInfo(file_path)) {}

    auto details()     const -> const ModDetails&;
    auto name()        const -> QString override;
    auto version()     const -> QString;
    auto homeurl()     const -> QString;
    auto description() const -> QString;
    auto authors()     const -> QStringList;
    auto status()      const -> ModStatus;

    auto metadata() -> std::shared_ptr<Metadata::ModStruct>;
    auto metadata() const -> const std::shared_ptr<Metadata::ModStruct>;

    void setStatus(ModStatus status);
    void setMetadata(std::shared_ptr<Metadata::ModStruct>&& metadata);
    void setMetadata(const Metadata::ModStruct& metadata) { setMetadata(std::make_shared<Metadata::ModStruct>(metadata)); }

    [[nodiscard]] auto compare(Resource const& other, SortType type) const -> std::pair<int, bool> override;
    [[nodiscard]] bool applyFilter(QRegularExpression filter) const override;

    // Delete all the files of this mod
    auto destroy(QDir& index_dir, bool preserve_metadata = false) -> bool;

    void finishResolvingWithDetails(ModDetails&& details);

protected:
    ModDetails m_local_details;
};
