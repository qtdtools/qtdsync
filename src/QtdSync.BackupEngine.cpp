/*
 *  This file is part of "QtdSync v0.7" (http://www.qtdtools.de)
 *
 *  Copyright (C) 2009-2014 Thomas Doering
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 */
#include "QtdSync.BackupEngine.h"
#include "QtdTools.h"

//============================================================================
class QtdSync::BackupEngine::Private
{
protected:
    //------------------------------------------------------------------------
    Private(const QString& identifier, const QString& label) {
        m_identifier = identifier;
        m_label      = label;
        m_processPrio = QtdTools::eProcessPriority_Normal;
        m_lastError = eNoError;
    }

    //------------------------------------------------------------------------
    virtual ~Private() {}

    //------------------------------------------------------------------------
    QString     m_identifier;
    QString     m_label;
    int         m_lastError;

    QMap<QString, QStringList> m_supportedBackupCombinations;
    QStringList m_managedLocationTypes;
    QtdTools::ProcessPriority m_processPrio;

    friend class QtdSync::BackupEngine;
};

//============================================================================
QtdSync::BackupEngine::BackupEngine(const QString& identifier, const QString& label, QObject* pParent)
    : QObject(pParent)
{
    p = new QtdSync::BackupEngine::Private(identifier, label);
}

//----------------------------------------------------------------------------
QtdSync::BackupEngine::~BackupEngine()
{
    if (p) {
        delete p;
        p = 0L;
    }
}

//----------------------------------------------------------------------------
void QtdSync::BackupEngine::addManagedLocationType(const QString& locType)
{
    if (locType.isEmpty()) {
        return;

    }

    QTD_IF_D(BackupEngine) {
        if (!p->m_managedLocationTypes.contains(locType)) {
            p->m_managedLocationTypes << locType;
        }
    }
}

//----------------------------------------------------------------------------
void QtdSync::BackupEngine::addManagedLocationTypes(const QStringList& locTypes)
{
    foreach (QString locType, locTypes) {
        addManagedLocationType(locType);
    }
}

//----------------------------------------------------------------------------
QStringList QtdSync::BackupEngine::managedLocationTypes()
{
    QTD_IF_D(BackupEngine) {
        return p->m_managedLocationTypes;
    }
    return QStringList();
}

//----------------------------------------------------------------------------
void QtdSync::BackupEngine::addSupportedBackupCombination(const QString& sourceType, const QString& destinationType)
{
    if (sourceType.isEmpty() || destinationType.isEmpty()) {
        return;
    }

    QTD_IF_D(BackupEngine) {
        if (!p->m_supportedBackupCombinations[sourceType].contains(destinationType)) {
            p->m_supportedBackupCombinations[sourceType] << destinationType;
        }
    }
}

//----------------------------------------------------------------------------
QStringList QtdSync::BackupEngine::supportedSourceTypes()
{
    QTD_IF_D(BackupEngine) {
        return p->m_supportedBackupCombinations.keys();
    }
    return QStringList();
}

//----------------------------------------------------------------------------
QStringList QtdSync::BackupEngine::supportedDestinationTypes(const QString& srcType)
{
    QTD_IF_D(BackupEngine) {
        return p->m_supportedBackupCombinations[srcType];
    }
    return QStringList();
}

//----------------------------------------------------------------------------
void QtdSync::BackupEngine::setProcessPriority(int nPrio)
{
    if (nPrio < QtdTools::eProcessPriority_Idle) {
        nPrio = QtdTools::eProcessPriority_Idle;
    }

    if (nPrio > QtdTools::eProcessPriority_Realtime) {
        nPrio = QtdTools::eProcessPriority_Realtime;
    }

    p->m_processPrio = (QtdTools::ProcessPriority)nPrio;
}

//----------------------------------------------------------------------------
int QtdSync::BackupEngine::processPriority()
{
    return p->m_processPrio;
}

//----------------------------------------------------------------------------
QString QtdSync::BackupEngine::identifier()
{
    return p->m_identifier;
}

//----------------------------------------------------------------------------
QString QtdSync::BackupEngine::label()
{
    QString context = "QtdSync.BackupEngine." + identifier();
    return qApp->translate(context.toLatin1().data(), p->m_label.toLatin1().data());
}

//----------------------------------------------------------------------------
QtdFileEntryInfoList QtdSync::BackupEngine::dir(QtdSync::Location& config, QtdDirTreeModel* pDirTreeModel /* = 0L */)
{
    return QtdFileEntryInfoList();
}

//----------------------------------------------------------------------------
int QtdSync::BackupEngine::lastError()
{
    QTD_IF_D(BackupEngine) {
        return d->m_lastError;
    }
    return eNoError;
}

//----------------------------------------------------------------------------
void QtdSync::BackupEngine::setLastError(int error)
{
    QTD_IF_D(BackupEngine) {
        d->m_lastError = error;
    }
}