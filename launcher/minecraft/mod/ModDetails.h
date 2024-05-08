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

#include <memory>

#include <QString>
#include <QStringList>

#include "minecraft/mod/MetadataHandler.h"

enum class ModStatus {
    Installed,      // Both JAR and Metadata are present
    NotInstalled,   // Only the Metadata is present
    NoMetadata,     // Only the JAR is present
    Unknown,        // Default status
};

struct ModDetails
{
    /* Mod ID as defined in the ModLoader-specific metadata */
    QString mod_id = {};
    
    /* Human-readable name */
    QString name = {};
    
    /* Human-readable mod version */
    QString version = {};
    
    /* Human-readable minecraft version */
    QString mcversion = {};
    
    /* URL for mod's home page */
    QString homeurl = {};
    
    /* Human-readable description */
    QString description = {};

    /* List of the author's names */
    QStringList authors = {};

    /* Installation status of the mod */
    ModStatus status = ModStatus::Unknown;

    /* Metadata information, if any */
    std::shared_ptr<Metadata::ModStruct> metadata = nullptr;

    ModDetails() = default;

    /** Metadata should be handled manually to properly set the mod status. */
    ModDetails(ModDetails& other)
        : mod_id(other.mod_id)
        , name(other.name)
        , version(other.version)
        , mcversion(other.mcversion)
        , homeurl(other.homeurl)
        , description(other.description)
        , authors(other.authors)
        , status(other.status)
    {}

    ModDetails& operator=(ModDetails& other)
    {
        this->mod_id = other.mod_id;
        this->name = other.name;
        this->version = other.version;
        this->mcversion = other.mcversion;
        this->homeurl = other.homeurl;
        this->description = other.description;
        this->authors = other.authors;
        this->status = other.status;

        return *this;
    }

    ModDetails& operator=(ModDetails&& other)
    {
        this->mod_id = other.mod_id;
        this->name = other.name;
        this->version = other.version;
        this->mcversion = other.mcversion;
        this->homeurl = other.homeurl;
        this->description = other.description;
        this->authors = other.authors;
        this->status = other.status;

        return *this;
    }
};
