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
#ifndef QTDSYNC_BACKUPENGINE_H
#define QTDSYNC_BACKUPENGINE_H

#include "QtdSync.h"
#include "QtdTools.h"
#include "QtdWidgets.h"
#include "QtdSync.Location.h"

namespace QtdSync {
    //----------------------------------------------------------------------------
    class BackupEngine : public QObject
    {
        Q_OBJECT
    public:
        typedef enum {
             eNoError
            ,eConnectionFailed
            ,eAuthenticationFailed
        } Errors;

        BackupEngine(const QString& identifier, const QString& label, QObject* pParent = 0L);
        virtual ~BackupEngine();

        //------------------------------------------------------------------------
        QString             identifier();
        QString             label();
        virtual QIcon       icon() = 0;
        virtual QString     description() = 0;

        virtual QStringList supportedSourceTypes();
        virtual QStringList supportedDestinationTypes(const QString& srcType);

        virtual QStringList managedLocationTypes();
        //virtual QtdSync::Location createLocation(const QString& type) = 0;

        //virtual bool        run(Location& src, Location& dst, int msTimeout = -1) = 0;
        virtual int         processPriority();

        virtual int         lastError();    

        virtual QtdFileEntryInfoList dir(QtdSync::Location& config, QtdDirTreeModel* pDirTreeModel = 0L);

    public slots:
        virtual void        setProcessPriority(int);

    signals:
        void                authenticationRequired(QString host, QAuthenticator& auth);
        void                progress(QString info, double dFileProgress, double dTotalProgress);

    protected:
        void                addManagedLocationType(const QString& locType);
        void                addManagedLocationTypes(const QStringList& locTypeList);

        void                addSupportedBackupCombination(const QString& sourceType, const QString& destinationType);
        void                setLastError(int error);

        QTD_DECLAREPROTECTED(QtdSync::BackupEngine);
    };
}

#endif // QTDSYNC_BACKUPENGINE_H