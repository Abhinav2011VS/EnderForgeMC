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

#include "HttpMetaCache.h"
#include "NetAction.h"
#include "Sink.h"
#include "Validator.h"

#include "QObjectPtr.h"

namespace Net {
class Download : public NetAction {
    Q_OBJECT

   public:
    using Ptr = shared_qobject_ptr<class Download>;
    enum class Option { NoOptions = 0, AcceptLocalFiles = 1, MakeEternal = 2 };
    Q_DECLARE_FLAGS(Options, Option)

   protected:
    explicit Download();

   public:
    ~Download() override = default;

    static auto makeCached(QUrl url, MetaEntryPtr entry, Options options = Option::NoOptions) -> Download::Ptr;
    static auto makeByteArray(QUrl url, QByteArray* output, Options options = Option::NoOptions) -> Download::Ptr;
    static auto makeFile(QUrl url, QString path, Options options = Option::NoOptions) -> Download::Ptr;

   public:
    void addValidator(Validator* v);
    auto abort() -> bool override;
    auto canAbort() const -> bool override { return true; };

   private:
    auto handleRedirect() -> bool;

   protected slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal) override;
    void downloadError(QNetworkReply::NetworkError error) override;
    void sslErrors(const QList<QSslError>& errors);
    void downloadFinished() override;
    void downloadReadyRead() override;

   public slots:
    void executeTask() override;

   private:
    std::unique_ptr<Sink> m_sink;
    Options m_options;
};
}  // namespace Net

Q_DECLARE_OPERATORS_FOR_FLAGS(Net::Download::Options)
