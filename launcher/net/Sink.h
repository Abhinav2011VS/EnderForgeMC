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

#include "net/NetAction.h"

#include "Validator.h"

namespace Net {
class Sink {
   public:
    Sink() = default;
    virtual ~Sink() = default;

   public:
    virtual auto init(QNetworkRequest& request) -> Task::State = 0;
    virtual auto write(QByteArray& data) -> Task::State = 0;
    virtual auto abort() -> Task::State = 0;
    virtual auto finalize(QNetworkReply& reply) -> Task::State = 0;

    virtual auto hasLocalData() -> bool = 0;

    void addValidator(Validator* validator)
    {
        if (validator) {
            validators.push_back(std::shared_ptr<Validator>(validator));
        }
    }

   protected:
    bool initAllValidators(QNetworkRequest& request)
    {
        for (auto& validator : validators) {
            if (!validator->init(request))
                return false;
        }
        return true;
    }
    bool finalizeAllValidators(QNetworkReply& reply)
    {
        for (auto& validator : validators) {
            if (!validator->validate(reply))
                return false;
        }
        return true;
    }
    bool failAllValidators()
    {
        bool success = true;
        for (auto& validator : validators) {
            success &= validator->abort();
        }
        return success;
    }
    bool writeAllValidators(QByteArray& data)
    {
        for (auto& validator : validators) {
            if (!validator->write(data))
                return false;
        }
        return true;
    }

   protected:
    std::vector<std::shared_ptr<Validator>> validators;
};
}  // namespace Net
