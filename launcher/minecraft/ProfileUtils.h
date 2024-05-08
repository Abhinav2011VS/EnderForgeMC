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
#include "Library.h"
#include "VersionFile.h"

namespace ProfileUtils
{
typedef QStringList PatchOrder;

/// Read and parse a OneSix format order file
bool readOverrideOrders(QString path, PatchOrder &order);

/// Write a OneSix format order file
bool writeOverrideOrders(QString path, const PatchOrder &order);


/// Parse a version file in JSON format
VersionFilePtr parseJsonFile(const QFileInfo &fileInfo, const bool requireOrder);

/// Save a JSON file (in any format)
bool saveJsonFile(const QJsonDocument doc, const QString & filename);

/// Remove LWJGL from a patch file. This is applied to all Mojang-like profile files.
void removeLwjglFromPatch(VersionFilePtr patch);

}
