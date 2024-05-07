// SPDX-License-Identifier: GPL-3.0-only
/*
 *  PolyMC - Minecraft Launcher
 *  Copyright (c) 2022 Jamie Mansfield <jmansfield@cadixdev.org>
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
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
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include "InstanceSettingsPage.h"
#include "ui_InstanceSettingsPage.h"

#include <QFileDialog>
#include <QDialog>
#include <QMessageBox>

#include <sys.h>

#include "ui/dialogs/VersionSelectDialog.h"
#include "ui/widgets/CustomCommands.h"

#include "JavaCommon.h"
#include "Application.h"

#include "java/JavaInstallList.h"
#include "java/JavaUtils.h"
#include "FileSystem.h"

#include "minecraft/auth/AccountList.h"


InstanceSettingsPage::InstanceSettingsPage(BaseInstance *inst, QWidget *parent)
    : QWidget(parent), ui(new Ui::InstanceSettingsPage), m_instance(inst)
{
    m_settings = inst->settings();
    m_accounts = APPLICATION->accounts();
    ui->setupUi(this);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    // This would just be set in the .ui file, if only Qt's uic would just ignore propreties that don't exist. SAD!
    ui->accountComboBox->setPlaceholderText(tr("No account selected"));
#endif

    auto sysMB = Sys::getSystemRam() / Sys::mebibyte;
    ui->maxMemSpinBox->setMaximum(sysMB);
    connect(ui->openGlobalJavaSettingsButton, &QCommandLinkButton::clicked, this, &InstanceSettingsPage::globalSettingsButtonClicked);
    connect(APPLICATION, &Application::globalSettingsAboutToOpen, this, &InstanceSettingsPage::applySettings);
    connect(APPLICATION, &Application::globalSettingsClosed, this, &InstanceSettingsPage::loadSettings);
    loadSettings();
}

bool InstanceSettingsPage::shouldDisplay() const
{
    return !m_instance->isRunning();
}

InstanceSettingsPage::~InstanceSettingsPage()
{
    delete ui;
}

void InstanceSettingsPage::globalSettingsButtonClicked(bool)
{
    switch(ui->settingsTabs->currentIndex()) {
        case 0:
            APPLICATION->ShowGlobalSettings(this, "java-settings");
            return;
        case 1:
            APPLICATION->ShowGlobalSettings(this, "minecraft-settings");
            return;
        case 2:
            APPLICATION->ShowGlobalSettings(this, "custom-commands");
            return;
    }
}

bool InstanceSettingsPage::apply()
{
    applySettings();
    return true;
}

void InstanceSettingsPage::applySettings()
{
    SettingsObject::Lock lock(m_settings);

    // Miscellaneous
    bool miscellaneous = ui->miscellaneousSettingsBox->isChecked();
    m_settings->set("OverrideMiscellaneous", miscellaneous);
    if (miscellaneous)
    {
        m_settings->set("CloseAfterLaunch", ui->closeAfterLaunchCheck->isChecked());
        m_settings->set("QuitAfterGameStop", ui->quitAfterGameStopCheck->isChecked());
    }
    else
    {
        m_settings->reset("CloseAfterLaunch");
        m_settings->reset("QuitAfterGameStop");
    }

    // Console
    bool console = ui->consoleSettingsBox->isChecked();
    m_settings->set("OverrideConsole", console);
    if (console)
    {
        m_settings->set("ShowConsole", ui->showConsoleCheck->isChecked());
        m_settings->set("AutoCloseConsole", ui->autoCloseConsoleCheck->isChecked());
        m_settings->set("ShowConsoleOnError", ui->showConsoleErrorCheck->isChecked());
    }
    else
    {
        m_settings->reset("ShowConsole");
        m_settings->reset("AutoCloseConsole");
        m_settings->reset("ShowConsoleOnError");
    }

    // Window Size
    bool window = ui->windowSizeGroupBox->isChecked();
    m_settings->set("OverrideWindow", window);
    if (window)
    {
        m_settings->set("LaunchMaximized", ui->maximizedCheckBox->isChecked());
        m_settings->set("MinecraftWinWidth", ui->windowWidthSpinBox->value());
        m_settings->set("MinecraftWinHeight", ui->windowHeightSpinBox->value());
    }
    else
    {
        m_settings->reset("LaunchMaximized");
        m_settings->reset("MinecraftWinWidth");
        m_settings->reset("MinecraftWinHeight");
    }

    // Memory
    bool memory = ui->memoryGroupBox->isChecked();
    m_settings->set("OverrideMemory", memory);
    if (memory)
    {
        int min = ui->minMemSpinBox->value();
        int max = ui->maxMemSpinBox->value();
        if(min < max)
        {
            m_settings->set("MinMemAlloc", min);
            m_settings->set("MaxMemAlloc", max);
        }
        else
        {
            m_settings->set("MinMemAlloc", max);
            m_settings->set("MaxMemAlloc", min);
        }
        m_settings->set("PermGen", ui->permGenSpinBox->value());
    }
    else
    {
        m_settings->reset("MinMemAlloc");
        m_settings->reset("MaxMemAlloc");
        m_settings->reset("PermGen");
    }

    // Java Install Settings
    bool javaInstall = ui->javaSettingsGroupBox->isChecked();
    m_settings->set("OverrideJavaLocation", javaInstall);
    if (javaInstall)
    {
        m_settings->set("JavaPath", ui->javaPathTextBox->text());
        m_settings->set("IgnoreJavaCompatibility", ui->skipCompatibilityCheckbox->isChecked());
    }
    else
    {
        m_settings->reset("JavaPath");
        m_settings->reset("IgnoreJavaCompatibility");
    }

    // Java arguments
    bool javaArgs = ui->javaArgumentsGroupBox->isChecked();
    m_settings->set("OverrideJavaArgs", javaArgs);
    if(javaArgs)
    {
        m_settings->set("JvmArgs", ui->jvmArgsTextBox->toPlainText().replace("\n", " "));
    }
    else
    {
        m_settings->reset("JvmArgs");
    }

    // old generic 'override both' is removed.
    m_settings->reset("OverrideJava");

    // Custom Commands
    bool custcmd = ui->customCommands->checked();
    m_settings->set("OverrideCommands", custcmd);
    if (custcmd)
    {
        m_settings->set("PreLaunchCommand", ui->customCommands->prelaunchCommand());
        m_settings->set("WrapperCommand", ui->customCommands->wrapperCommand());
        m_settings->set("PostExitCommand", ui->customCommands->postexitCommand());
    }
    else
    {
        m_settings->reset("PreLaunchCommand");
        m_settings->reset("WrapperCommand");
        m_settings->reset("PostExitCommand");
    }

    // Workarounds
    bool workarounds = ui->nativeWorkaroundsGroupBox->isChecked();
    m_settings->set("OverrideNativeWorkarounds", workarounds);
    if(workarounds)
    {
        m_settings->set("UseNativeOpenAL", ui->useNativeOpenALCheck->isChecked());
        m_settings->set("UseNativeGLFW", ui->useNativeGLFWCheck->isChecked());
    }
    else
    {
        m_settings->reset("UseNativeOpenAL");
        m_settings->reset("UseNativeGLFW");
    }

    // Performance
    bool performance = ui->perfomanceGroupBox->isChecked();
    m_settings->set("OverridePerformance", performance);
    if(performance)
    {
        m_settings->set("EnableFeralGamemode", ui->enableFeralGamemodeCheck->isChecked());
        m_settings->set("EnableMangoHud", ui->enableMangoHud->isChecked());
        m_settings->set("UseDiscreteGpu", ui->useDiscreteGpuCheck->isChecked());
    }
    else
    {
        m_settings->reset("EnableFeralGamemode");
        m_settings->reset("EnableMangoHud");
        m_settings->reset("UseDiscreteGpu");
    }

    // Game time
    bool gameTime = ui->gameTimeGroupBox->isChecked();
    m_settings->set("OverrideGameTime", gameTime);
    if (gameTime)
    {
        m_settings->set("ShowGameTime", ui->showGameTime->isChecked());
        m_settings->set("RecordGameTime", ui->recordGameTime->isChecked());
    }
    else
    {
        m_settings->reset("ShowGameTime");
        m_settings->reset("RecordGameTime");
    }

    // Join server on launch
    bool joinServerOnLaunch = ui->serverJoinGroupBox->isChecked();
    m_settings->set("JoinServerOnLaunch", joinServerOnLaunch);
    if (joinServerOnLaunch)
    {
        m_settings->set("JoinServerOnLaunchAddress", ui->serverJoinAddress->text());
    }
    else
    {
        m_settings->reset("JoinServerOnLaunchAddress");
    }

    // Account override settings
    bool accountOverride = ui->accountGroupBox->isChecked();
    m_settings->set("OverrideAccount", accountOverride);
    if (accountOverride && ui->accountComboBox->currentData().isValid())
    {
        m_settings->set("OverrideAccountProfileId", ui->accountComboBox->currentData().toString());
    }
    else
    {
        m_settings->reset("OverrideAccountProfileId");
    }

    // FIXME: This should probably be called by a signal instead
    m_instance->updateRuntimeContext();
}

void InstanceSettingsPage::loadSettings()
{
    // Miscellaneous
    ui->miscellaneousSettingsBox->setChecked(m_settings->get("OverrideMiscellaneous").toBool());
    ui->closeAfterLaunchCheck->setChecked(m_settings->get("CloseAfterLaunch").toBool());
    ui->quitAfterGameStopCheck->setChecked(m_settings->get("QuitAfterGameStop").toBool());

    // Console
    ui->consoleSettingsBox->setChecked(m_settings->get("OverrideConsole").toBool());
    ui->showConsoleCheck->setChecked(m_settings->get("ShowConsole").toBool());
    ui->autoCloseConsoleCheck->setChecked(m_settings->get("AutoCloseConsole").toBool());
    ui->showConsoleErrorCheck->setChecked(m_settings->get("ShowConsoleOnError").toBool());

    // Window Size
    ui->windowSizeGroupBox->setChecked(m_settings->get("OverrideWindow").toBool());
    ui->maximizedCheckBox->setChecked(m_settings->get("LaunchMaximized").toBool());
    ui->windowWidthSpinBox->setValue(m_settings->get("MinecraftWinWidth").toInt());
    ui->windowHeightSpinBox->setValue(m_settings->get("MinecraftWinHeight").toInt());

    // Memory
    ui->memoryGroupBox->setChecked(m_settings->get("OverrideMemory").toBool());
    int min = m_settings->get("MinMemAlloc").toInt();
    int max = m_settings->get("MaxMemAlloc").toInt();
    if(min < max)
    {
        ui->minMemSpinBox->setValue(min);
        ui->maxMemSpinBox->setValue(max);
    }
    else
    {
        ui->minMemSpinBox->setValue(max);
        ui->maxMemSpinBox->setValue(min);
    }
    ui->permGenSpinBox->setValue(m_settings->get("PermGen").toInt());
    bool permGenVisible = m_settings->get("PermGenVisible").toBool();
    ui->permGenSpinBox->setVisible(permGenVisible);
    ui->labelPermGen->setVisible(permGenVisible);
    ui->labelPermgenNote->setVisible(permGenVisible);


    // Java Settings
    bool overrideJava = m_settings->get("OverrideJava").toBool();
    bool overrideLocation = m_settings->get("OverrideJavaLocation").toBool() || overrideJava;
    bool overrideArgs = m_settings->get("OverrideJavaArgs").toBool() || overrideJava;

    ui->javaSettingsGroupBox->setChecked(overrideLocation);
    ui->javaPathTextBox->setText(m_settings->get("JavaPath").toString());
    ui->skipCompatibilityCheckbox->setChecked(m_settings->get("IgnoreJavaCompatibility").toBool());

    ui->javaArgumentsGroupBox->setChecked(overrideArgs);
    ui->jvmArgsTextBox->setPlainText(m_settings->get("JvmArgs").toString());

    // Custom commands
    ui->customCommands->initialize(
        true,
        m_settings->get("OverrideCommands").toBool(),
        m_settings->get("PreLaunchCommand").toString(),
        m_settings->get("WrapperCommand").toString(),
        m_settings->get("PostExitCommand").toString()
    );

    // Workarounds
    ui->nativeWorkaroundsGroupBox->setChecked(m_settings->get("OverrideNativeWorkarounds").toBool());
    ui->useNativeGLFWCheck->setChecked(m_settings->get("UseNativeGLFW").toBool());
    ui->useNativeOpenALCheck->setChecked(m_settings->get("UseNativeOpenAL").toBool());

    // Performance
    ui->perfomanceGroupBox->setChecked(m_settings->get("OverridePerformance").toBool());
    ui->enableFeralGamemodeCheck->setChecked(m_settings->get("EnableFeralGamemode").toBool());
    ui->enableMangoHud->setChecked(m_settings->get("EnableMangoHud").toBool());
    ui->useDiscreteGpuCheck->setChecked(m_settings->get("UseDiscreteGpu").toBool());

#if !defined(Q_OS_LINUX)
    ui->settingsTabs->setTabVisible(ui->settingsTabs->indexOf(ui->performancePage), false);
#endif

    if (!(APPLICATION->capabilities() & Application::SupportsGameMode)) {
        ui->enableFeralGamemodeCheck->setDisabled(true);
        ui->enableFeralGamemodeCheck->setToolTip(tr("Feral Interactive's GameMode could not be found on your system."));
    }

    if (!(APPLICATION->capabilities() & Application::SupportsMangoHud)) {
        ui->enableMangoHud->setDisabled(true);
        ui->enableMangoHud->setToolTip(tr("MangoHud could not be found on your system."));
    }

    // Miscellanous
    ui->gameTimeGroupBox->setChecked(m_settings->get("OverrideGameTime").toBool());
    ui->showGameTime->setChecked(m_settings->get("ShowGameTime").toBool());
    ui->recordGameTime->setChecked(m_settings->get("RecordGameTime").toBool());

    ui->serverJoinGroupBox->setChecked(m_settings->get("JoinServerOnLaunch").toBool());
    ui->serverJoinAddress->setText(m_settings->get("JoinServerOnLaunchAddress").toString());

    ui->accountGroupBox->setChecked(m_settings->get("OverrideAccount").toBool());
    if (m_accounts->count() > 0) {
        for (int i = 0; i < m_accounts->count(); i++) {
            MinecraftAccountPtr account = m_accounts->at(i);

            QString profileLabel = account->profileName();
            QString profileId = account->profileId();
            QString profileType = account->typeString();
            profileType[0] = profileType[0].toUpper();
            QPixmap profileFace = account->getFace();
            QIcon profileIcon = !profileFace.isNull() ? QIcon(profileFace) : APPLICATION->getThemedIcon("noaccount");

            ui->accountComboBox->addItem(profileIcon, QString("%1 (%2)").arg(profileLabel, profileType), profileId);
        }

        QString accountProfileId = m_settings->get("OverrideAccountProfileId").toString();
        int index = -1;
        if (ui->accountGroupBox->isChecked()) {
            // Try find the account by the override profile id setting
            // It'll be -1 if the account doesn't exist, which will just
            // Set the combobox to the "no option selected" state
            index = ui->accountComboBox->findData(accountProfileId);
        }
        ui->accountComboBox->setCurrentIndex(index);
    } else {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        ui->accountComboBox->setPlaceholderText(tr("No accounts available"));
        ui->accountComboBox->setCurrentIndex(-1);
#else
        ui->accountComboBox->addItem(tr("No accounts available"));
#endif
        ui->accountComboBox->setDisabled(true);
    }

    // If there isn't a currently selected account, the override should show as disabled
    if (ui->accountComboBox->currentIndex() == -1) {
        ui->accountGroupBox->setChecked(false);
    }
}

void InstanceSettingsPage::on_javaDetectBtn_clicked()
{
    if (JavaUtils::getJavaCheckPath().isEmpty()) {
        JavaCommon::javaCheckNotFound(this);
        return;
    }

    JavaInstallPtr java;

    VersionSelectDialog vselect(APPLICATION->javalist().get(), tr("Select a Java version"), this, true);
    vselect.setResizeOn(2);
    vselect.exec();

    if (vselect.result() == QDialog::Accepted && vselect.selectedVersion())
    {
        java = std::dynamic_pointer_cast<JavaInstall>(vselect.selectedVersion());
        ui->javaPathTextBox->setText(java->path);
        bool visible = java->id.requiresPermGen() && m_settings->get("OverrideMemory").toBool();
        ui->permGenSpinBox->setVisible(visible);
        ui->labelPermGen->setVisible(visible);
        ui->labelPermgenNote->setVisible(visible);
        m_settings->set("PermGenVisible", visible);
    }
}

void InstanceSettingsPage::on_javaBrowseBtn_clicked()
{
    QString raw_path = QFileDialog::getOpenFileName(this, tr("Find Java executable"));

    // do not allow current dir - it's dirty. Do not allow dirs that don't exist
    if(raw_path.isEmpty())
    {
        return;
    }
    QString cooked_path = FS::NormalizePath(raw_path);

    QFileInfo javaInfo(cooked_path);
    if(!javaInfo.exists() || !javaInfo.isExecutable())
    {
        return;
    }
    ui->javaPathTextBox->setText(cooked_path);

    // custom Java could be anything... enable perm gen option
    ui->permGenSpinBox->setVisible(true);
    ui->labelPermGen->setVisible(true);
    ui->labelPermgenNote->setVisible(true);
    m_settings->set("PermGenVisible", true);
}

void InstanceSettingsPage::on_javaTestBtn_clicked()
{
    if(checker)
    {
        return;
    }
    checker.reset(new JavaCommon::TestCheck(
        this, ui->javaPathTextBox->text(), ui->jvmArgsTextBox->toPlainText().replace("\n", " "),
        ui->minMemSpinBox->value(), ui->maxMemSpinBox->value(), ui->permGenSpinBox->value()));
    connect(checker.get(), SIGNAL(finished()), SLOT(checkerFinished()));
    checker->run();
}

void InstanceSettingsPage::checkerFinished()
{
    checker.reset();
}

void InstanceSettingsPage::retranslate()
{
    ui->retranslateUi(this);
    ui->customCommands->retranslate();  // TODO: why is this seperate from the others?
}