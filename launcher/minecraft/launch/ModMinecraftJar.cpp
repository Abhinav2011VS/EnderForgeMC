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

#include "ModMinecraftJar.h"
#include "launch/LaunchTask.h"
#include "MMCZip.h"
#include "FileSystem.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"

void ModMinecraftJar::executeTask()
{
    auto m_inst = std::dynamic_pointer_cast<MinecraftInstance>(m_parent->instance());

    if(!m_inst->getJarMods().size())
    {
        emitSucceeded();
        return;
    }
    // nuke obsolete stripped jar(s) if needed
    if(!FS::ensureFolderPathExists(m_inst->binRoot()))
    {
        emitFailed(tr("Couldn't create the bin folder for Minecraft.jar"));
    }

    auto finalJarPath = QDir(m_inst->binRoot()).absoluteFilePath("minecraft.jar");
    if(!removeJar())
    {
        emitFailed(tr("Couldn't remove stale jar file: %1").arg(finalJarPath));
    }

    // create temporary modded jar, if needed
    auto components = m_inst->getPackProfile();
    auto profile = components->getProfile();
    auto jarMods = m_inst->getJarMods();
    if(jarMods.size())
    {
        auto mainJar = profile->getMainJar();
        QStringList jars, temp1, temp2, temp3, temp4;
        mainJar->getApplicableFiles(m_inst->runtimeContext(), jars, temp1, temp2, temp3, m_inst->getLocalLibraryPath());
        auto sourceJarPath = jars[0];
        if(!MMCZip::createModdedJar(sourceJarPath, finalJarPath, jarMods))
        {
            emitFailed(tr("Failed to create the custom Minecraft jar file."));
            return;
        }
    }
    emitSucceeded();
}

void ModMinecraftJar::finalize()
{
    removeJar();
}

bool ModMinecraftJar::removeJar()
{
    auto m_inst = std::dynamic_pointer_cast<MinecraftInstance>(m_parent->instance());
    auto finalJarPath = QDir(m_inst->binRoot()).absoluteFilePath("minecraft.jar");
    QFile finalJar(finalJarPath);
    if(finalJar.exists())
    {
        if(!finalJar.remove())
        {
            return false;
        }
    }
    return true;
}
