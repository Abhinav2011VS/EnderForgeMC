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

#include "TexturePackFolderModel.h"

#include "minecraft/mod/tasks/BasicFolderLoadTask.h"
#include "minecraft/mod/tasks/LocalTexturePackParseTask.h"

TexturePackFolderModel::TexturePackFolderModel(const QString &dir) : ResourceFolderModel(QDir(dir)) {}

Task* TexturePackFolderModel::createUpdateTask()
{
    return new BasicFolderLoadTask(m_dir, [](QFileInfo const& entry) { return new TexturePack(entry); });
}

Task* TexturePackFolderModel::createParseTask(Resource& resource)
{
    return new LocalTexturePackParseTask(m_next_resolution_ticket, static_cast<TexturePack&>(resource));
}
