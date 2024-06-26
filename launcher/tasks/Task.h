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

#include <QRunnable>

#include "QObjectPtr.h"

class Task : public QObject, public QRunnable {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<Task>;

    enum class State { Inactive, Running, Succeeded, Failed, AbortedByUser };

   public:
    explicit Task(QObject* parent = 0, bool show_debug_log = true);
    virtual ~Task() = default;

    bool isRunning() const;
    bool isFinished() const;
    bool wasSuccessful() const;

    /*! 
     * MultiStep tasks are combinations of multiple tasks into a single logical task. 
     * The main usage of this is in SequencialTask.
     */
    virtual auto isMultiStep() const -> bool { return false; }

    /*!
     * Returns the string that was passed to emitFailed as the error message when the task failed.
     * If the task hasn't failed, returns an empty string.
     */
    QString failReason() const;

    virtual QStringList warnings() const;

    virtual bool canAbort() const { return m_can_abort; }

    auto getState() const -> State { return m_state; }

    QString getStatus() { return m_status; }
    virtual auto getStepStatus() const -> QString { return m_status; }

    qint64 getProgress() { return m_progress; }
    qint64 getTotalProgress() { return m_progressTotal; }
    virtual auto getStepProgress() const -> qint64 { return 0; }
    virtual auto getStepTotalProgress() const -> qint64 { return 100; }

   protected:
    void logWarning(const QString& line);

   private:
    QString describe();

   signals:
    void started();
    void progress(qint64 current, qint64 total);
    void finished();
    void succeeded();
    void aborted();
    void failed(QString reason);
    void status(QString status);
    void stepStatus(QString status);

    /** Emitted when the canAbort() status has changed.
     */
    void abortStatusChanged(bool can_abort);

   public slots:
    // QRunnable's interface
    void run() override { start(); }

    virtual void start();
    virtual bool abort() { if(canAbort()) emitAborted(); return canAbort(); };

    void setAbortable(bool can_abort) { m_can_abort = can_abort; emit abortStatusChanged(can_abort); }

   protected:
    virtual void executeTask() = 0;

   protected slots:
    virtual void emitSucceeded();
    virtual void emitAborted();
    virtual void emitFailed(QString reason = "");

   public slots:
    void setStatus(const QString& status);
    void setProgress(qint64 current, qint64 total);

   protected:
    State m_state = State::Inactive;
    QStringList m_Warnings;
    QString m_failReason = "";
    QString m_status;
    int m_progress = 0;
    int m_progressTotal = 100;

    // TODO: Nuke in favor of QLoggingCategory
    bool m_show_debug = true;

   private:
    // Change using setAbortStatus
    bool m_can_abort = false;
};
