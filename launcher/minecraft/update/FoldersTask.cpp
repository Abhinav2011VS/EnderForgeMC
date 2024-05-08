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

#include "FoldersTask.h"
#include "minecraft/MinecraftInstance.h"
#include <QDir>

FoldersTask::FoldersTask(MinecraftInstance * inst)
    :Task()
{
    m_inst = inst;
}

void FoldersTask::executeTask()
{
    // Make directories
    QDir mcDir(m_inst->gameRoot());
    if (!mcDir.exists() && !mcDir.mkpath("."))
    {
        emitFailed(tr("Failed to create folder for Minecraft binaries."));
        return;
    }
    emitSucceeded();
}
