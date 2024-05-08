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

#pragma once

#include <QString>
#include <QList>
#include <QJsonObject>
#include <memory>
#include "RuntimeContext.h"

class Library;
class Rule;

enum RuleAction
{
    Allow,
    Disallow,
    Defer
};

QList<std::shared_ptr<Rule>> rulesFromJsonV4(const QJsonObject &objectWithRules);

class Rule
{
protected:
    RuleAction m_result;
    virtual bool applies(const Library *parent, const RuntimeContext & runtimeContext) = 0;

public:
    Rule(RuleAction result) : m_result(result)
    {
    }
    virtual ~Rule() {};
    virtual QJsonObject toJson() = 0;
    RuleAction apply(const Library *parent, const RuntimeContext & runtimeContext)
    {
        if (applies(parent, runtimeContext))
            return m_result;
        else
            return Defer;
    }
};

class OsRule : public Rule
{
private:
    // the OS
    QString m_system;
    // the OS version regexp
    QString m_version_regexp;

protected:
    virtual bool applies(const Library *, const RuntimeContext & runtimeContext)
    {
        return runtimeContext.classifierMatches(m_system);
    }
    OsRule(RuleAction result, QString system, QString version_regexp)
        : Rule(result), m_system(system), m_version_regexp(version_regexp)
    {
    }

public:
    virtual QJsonObject toJson();
    static std::shared_ptr<OsRule> create(RuleAction result, QString system,
                                          QString version_regexp)
    {
        return std::shared_ptr<OsRule>(new OsRule(result, system, version_regexp));
    }
};

class ImplicitRule : public Rule
{
protected:
    virtual bool applies(const Library *, const RuntimeContext & runtimeContext)
    {
        return true;
    }
    ImplicitRule(RuleAction result) : Rule(result)
    {
    }

public:
    virtual QJsonObject toJson();
    static std::shared_ptr<ImplicitRule> create(RuleAction result)
    {
        return std::shared_ptr<ImplicitRule>(new ImplicitRule(result));
    }
};
