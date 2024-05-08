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

#include <QJsonObject>
#include <QJsonArray>

#include "Rule.h"

RuleAction RuleAction_fromString(QString name)
{
    if (name == "allow")
        return Allow;
    if (name == "disallow")
        return Disallow;
    return Defer;
}

QList<std::shared_ptr<Rule>> rulesFromJsonV4(const QJsonObject &objectWithRules)
{
    QList<std::shared_ptr<Rule>> rules;
    auto rulesVal = objectWithRules.value("rules");
    if (!rulesVal.isArray())
        return rules;

    QJsonArray ruleList = rulesVal.toArray();
    for (auto ruleVal : ruleList)
    {
        std::shared_ptr<Rule> rule;
        if (!ruleVal.isObject())
            continue;
        auto ruleObj = ruleVal.toObject();
        auto actionVal = ruleObj.value("action");
        if (!actionVal.isString())
            continue;
        auto action = RuleAction_fromString(actionVal.toString());
        if (action == Defer)
            continue;

        auto osVal = ruleObj.value("os");
        if (!osVal.isObject())
        {
            // add a new implicit action rule
            rules.append(ImplicitRule::create(action));
            continue;
        }

        auto osObj = osVal.toObject();
        auto osNameVal = osObj.value("name");
        if (!osNameVal.isString())
            continue;
        QString osName = osNameVal.toString();
        QString versionRegex = osObj.value("version").toString();
        // add a new OS rule
        rules.append(OsRule::create(action, osName, versionRegex));
    }
    return rules;
}

QJsonObject ImplicitRule::toJson()
{
    QJsonObject ruleObj;
    ruleObj.insert("action", m_result == Allow ? QString("allow") : QString("disallow"));
    return ruleObj;
}

QJsonObject OsRule::toJson()
{
    QJsonObject ruleObj;
    ruleObj.insert("action", m_result == Allow ? QString("allow") : QString("disallow"));
    QJsonObject osObj;
    {
        osObj.insert("name", m_system);
        if(!m_version_regexp.isEmpty())
        {
            osObj.insert("version", m_version_regexp);
        }
    }
    ruleObj.insert("os", osObj);
    return ruleObj;
}

