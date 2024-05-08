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

#include "Validator.h"

#include <QCryptographicHash>
#include <QFile>

namespace Net {
class ChecksumValidator : public Validator {
   public:
    ChecksumValidator(QCryptographicHash::Algorithm algorithm, QByteArray expected = QByteArray())
        : m_checksum(algorithm), m_expected(expected){};
    virtual ~ChecksumValidator() = default;

   public:
    auto init(QNetworkRequest&) -> bool override
    {
        m_checksum.reset();
        return true;
    }

    auto write(QByteArray& data) -> bool override
    {
        m_checksum.addData(data);
        return true;
    }

    auto abort() -> bool override { return true; }

    auto validate(QNetworkReply&) -> bool override
    {
        if (m_expected.size() && m_expected != hash()) {
            qWarning() << "Checksum mismatch, download is bad.";
            return false;
        }
        return true;
    }

    auto hash() -> QByteArray { return m_checksum.result(); }

    void setExpected(QByteArray expected) { m_expected = expected; }

   private:
    QCryptographicHash m_checksum;
    QByteArray m_expected;
};
}  // namespace Net
