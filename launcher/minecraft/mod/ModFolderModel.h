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

#include <QList>
#include <QMap>
#include <QSet>
#include <QString>
#include <QDir>
#include <QAbstractListModel>

#include "Mod.h"
#include "ResourceFolderModel.h"

#include "minecraft/mod/tasks/ModFolderLoadTask.h"
#include "minecraft/mod/tasks/LocalModParseTask.h"

class LegacyInstance;
class BaseInstance;
class QFileSystemWatcher;

/**
 * A legacy mod list.
 * Backed by a folder.
 */
class ModFolderModel : public ResourceFolderModel
{
    Q_OBJECT
public:
    enum Columns
    {
        ActiveColumn = 0,
        NameColumn,
        VersionColumn,
        DateColumn,
        NUM_COLUMNS
    };
    enum ModStatusAction {
        Disable,
        Enable,
        Toggle
    };
    ModFolderModel(const QString &dir, bool is_indexed = false);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent) const override;

    [[nodiscard]] Task* createUpdateTask() override;
    [[nodiscard]] Task* createParseTask(Resource&) override;

    bool installMod(QString file_path) { return ResourceFolderModel::installResource(file_path); }
    bool uninstallMod(const QString& filename, bool preserve_metadata = false);

    /// Deletes all the selected mods
    bool deleteMods(const QModelIndexList &indexes);

    bool isValid();

    bool startWatching() override;
    bool stopWatching() override;

    QDir indexDir() { return { QString("%1/.index").arg(dir().absolutePath()) }; }

    auto selectedMods(QModelIndexList& indexes) -> QList<Mod*>;
    auto allMods() -> QList<Mod*>;

    RESOURCE_HELPERS(Mod)

private
slots:
    void onUpdateSucceeded() override;
    void onParseSucceeded(int ticket, QString resource_id) override;

protected:
    bool m_is_indexed;
    bool m_first_folder_load = true;
};
