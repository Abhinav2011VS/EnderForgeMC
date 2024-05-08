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

#include <QJsonArray>
#include <QJsonDocument>

#include <QDebug>

#include "minecraft/VersionFile.h"
#include "minecraft/Library.h"
#include "minecraft/PackProfile.h"
#include "ParseUtils.h"

#include <Version.h>

static bool isMinecraftVersion(const QString &uid)
{
    return uid == "net.minecraft";
}

void VersionFile::applyTo(LaunchProfile *profile, const RuntimeContext & runtimeContext)
{
    // Only real Minecraft can set those. Don't let anything override them.
    if (isMinecraftVersion(uid))
    {
        profile->applyMinecraftVersion(version);
        profile->applyMinecraftVersionType(type);
        // HACK: ignore assets from other version files than Minecraft
        // workaround for stupid assets issue caused by amazon:
        // https://www.theregister.co.uk/2017/02/28/aws_is_awol_as_s3_goes_haywire/
        profile->applyMinecraftAssets(mojangAssetIndex);
    }

    profile->applyMainJar(mainJar);
    profile->applyMainClass(mainClass);
    profile->applyAppletClass(appletClass);
    profile->applyMinecraftArguments(minecraftArguments);
    profile->applyAddnJvmArguments(addnJvmArguments);
    profile->applyTweakers(addTweakers);
    profile->applyJarMods(jarMods);
    profile->applyMods(mods);
    profile->applyTraits(traits);
    profile->applyCompatibleJavaMajors(compatibleJavaMajors);

    for (auto library : libraries)
    {
        profile->applyLibrary(library, runtimeContext);
    }
    for (auto mavenFile : mavenFiles)
    {
        profile->applyMavenFile(mavenFile, runtimeContext);
    }
    for (auto agent : agents)
    {
        profile->applyAgent(agent, runtimeContext);
    }
    profile->applyProblemSeverity(getProblemSeverity());
}
