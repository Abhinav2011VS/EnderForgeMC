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

#include "ModFolderLoadTask.h"

#include "minecraft/mod/MetadataHandler.h"

#include <QThread>

ModFolderLoadTask::ModFolderLoadTask(QDir mods_dir, QDir index_dir, bool is_indexed, bool clean_orphan)
    : Task(nullptr, false)
    , m_mods_dir(mods_dir)
    , m_index_dir(index_dir)
    , m_is_indexed(is_indexed)
    , m_clean_orphan(clean_orphan)
    , m_result(new Result())
    , m_thread_to_spawn_into(thread())
{}

void ModFolderLoadTask::executeTask()
{
    if (thread() != m_thread_to_spawn_into)
        connect(this, &Task::finished, this->thread(), &QThread::quit);

    if (m_is_indexed) {
        // Read metadata first
        getFromMetadata();
    }

    // Read JAR files that don't have metadata
    m_mods_dir.refresh();
    for (auto entry : m_mods_dir.entryInfoList()) {
        Mod* mod(new Mod(entry));

        if (mod->enabled()) {
            if (m_result->mods.contains(mod->internal_id())) {
                m_result->mods[mod->internal_id()]->setStatus(ModStatus::Installed);
                // Delete the object we just created, since a valid one is already in the mods list.
                delete mod;
            }
            else {
                m_result->mods[mod->internal_id()] = mod;
                m_result->mods[mod->internal_id()]->setStatus(ModStatus::NoMetadata);
            }
        }
        else { 
            QString chopped_id = mod->internal_id().chopped(9);
            if (m_result->mods.contains(chopped_id)) {
                m_result->mods[mod->internal_id()] = mod;

                auto metadata = m_result->mods[chopped_id]->metadata();
                if (metadata) {
                    mod->setMetadata(*metadata);

                    m_result->mods[mod->internal_id()]->setStatus(ModStatus::Installed);
                    m_result->mods.remove(chopped_id);
                }
            }
            else {
                m_result->mods[mod->internal_id()] = mod;
                m_result->mods[mod->internal_id()]->setStatus(ModStatus::NoMetadata);
            }
        }
    }

    // Remove orphan metadata to prevent issues
    // See https://github.com/EnderForgeMC/EnderForgeMC/issues/996
    if (m_clean_orphan) {
        QMutableMapIterator iter(m_result->mods);
        while (iter.hasNext()) {
            auto mod = iter.next().value();
            if (mod->status() == ModStatus::NotInstalled) {
                mod->destroy(m_index_dir, false);
                iter.remove();
            }
        }
    }

    for (auto mod : m_result->mods)
        mod->moveToThread(m_thread_to_spawn_into);

    if (m_aborted)
        emit finished();
    else
        emitSucceeded();
}

void ModFolderLoadTask::getFromMetadata()
{
    m_index_dir.refresh();
    for (auto entry : m_index_dir.entryList(QDir::Files)) {
        auto metadata = Metadata::get(m_index_dir, entry);

        if(!metadata.isValid()){
            return;
        }

        auto* mod = new Mod(m_mods_dir, metadata);
        mod->setStatus(ModStatus::NotInstalled);
        m_result->mods[mod->internal_id()] = mod;
    }
}
