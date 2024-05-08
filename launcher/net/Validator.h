// SPDX-License-Identifier: GPL-3.0-only
/*
 *  EnderForgeMC - Minecraft Launcher
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

#include "net/NetAction.h"

namespace Net {
class Validator
{
public: /* con/des */
    Validator() {};
    virtual ~Validator() {};

public: /* methods */
    virtual bool init(QNetworkRequest & request) = 0;
    virtual bool write(QByteArray & data) = 0;
    virtual bool abort() = 0;
    virtual bool validate(QNetworkReply & reply) = 0;
};
}
