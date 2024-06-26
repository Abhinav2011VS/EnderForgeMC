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

#include <QMap>
#include <QString>
#include <QTimer>
#include <memory>

class HttpMetaCache;

class MetaEntry {
    friend class HttpMetaCache;

   protected:
    MetaEntry() = default;

   public:
    auto isStale() -> bool { return stale; }
    void setStale(bool stale) { this->stale = stale; }

    auto getFullPath() -> QString;

    auto getRemoteChangedTimestamp() -> QString { return remote_changed_timestamp; }
    void setRemoteChangedTimestamp(QString remote_changed_timestamp) { this->remote_changed_timestamp = remote_changed_timestamp; }
    void setLocalChangedTimestamp(qint64 timestamp) { local_changed_timestamp = timestamp; }

    auto getETag() -> QString { return etag; }
    void setETag(QString etag) { this->etag = etag; }

    auto getMD5Sum() -> QString { return md5sum; }
    void setMD5Sum(QString md5sum) { this->md5sum = md5sum; }

    /* Whether the entry expires after some time (false) or not (true). */
    void makeEternal(bool eternal) { is_eternal = eternal; }
    [[nodiscard]] bool isEternal() const { return is_eternal; }

    auto getCurrentAge() -> qint64 { return current_age; }
    void setCurrentAge(qint64 age) { current_age = age; }

    auto getMaximumAge() -> qint64 { return max_age; }
    void setMaximumAge(qint64 age) { max_age = age; }

    bool isExpired(qint64 offset) { return !is_eternal && (current_age >= max_age - offset); };

   protected:
    QString baseId;
    QString basePath;
    QString relativePath;
    QString md5sum;
    QString etag;

    qint64 local_changed_timestamp = 0;
    QString remote_changed_timestamp;  // QString for now, RFC 2822 encoded time
    qint64 current_age = 0;
    qint64 max_age = 0;
    bool is_eternal = false;

    bool stale = true;
};

using MetaEntryPtr = std::shared_ptr<MetaEntry>;

class HttpMetaCache : public QObject {
    Q_OBJECT
   public:
    // supply path to the cache index file
    HttpMetaCache(QString path = QString());
    ~HttpMetaCache() override;

    // get the entry solely from the cache
    // you probably don't want this, unless you have some specific caching needs.
    auto getEntry(QString base, QString resource_path) -> MetaEntryPtr;

    // get the entry from cache and verify that it isn't stale (within reason)
    auto resolveEntry(QString base, QString resource_path, QString expected_etag = QString()) -> MetaEntryPtr;

    // add a previously resolved stale entry
    auto updateEntry(MetaEntryPtr stale_entry) -> bool;

    // evict selected entry from cache
    auto evictEntry(MetaEntryPtr entry) -> bool;

    void addBase(QString base, QString base_root);

    // (re)start a timer that calls SaveNow later.
    void SaveEventually();
    void Load();

    auto getBasePath(QString base) -> QString;

   public slots:
    void SaveNow();

   private:
    // create a new stale entry, given the parameters
    auto staleEntry(QString base, QString resource_path) -> MetaEntryPtr;

    struct EntryMap {
        QString base_path;
        QMap<QString, MetaEntryPtr> entry_list;
    };

    QMap<QString, EntryMap> m_entries;
    QString m_index_file;
    QTimer saveBatchingTimer;
};
