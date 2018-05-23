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
#ifndef QTDSYNC_BACKUPENGINE_RSYNC_H
#define QTDSYNC_BACKUPENGINE_RSYNC_H

#include "QtdSync.h"
#include "QtdSync.BackupEngine.h"
#include "QtdTools.h"
#include "QtdWidgets.h"

namespace QtdSync {

    //----------------------------------------------------------------------------
    class RsyncLocation;
    class RsyncBackupEngine : public BackupEngine
    {
        Q_OBJECT
    public:
        //----------------------------------------------------------------------------
        // Type definitions
        //----------------------------------------------------------------------------
        typedef enum {
             eNoError           = 0
            ,eTimedOut
            ,eAuthentication
            ,eRemoteRemote
        } EnRsyncError;

        //----------------------------------------------------------------------------
        // Member functions
        //----------------------------------------------------------------------------
        RsyncBackupEngine(QObject* pParent = 0L);
        ~RsyncBackupEngine();


        //----------------------------------------------------------------------------
        // Base class reimplementations
        //----------------------------------------------------------------------------
        virtual QIcon       icon();
        virtual QString     description();
        //virtual QtdSync::Location createLocation(const QString& type);
        //virtual bool        run(Location& src, Location& dst, int msTimeout = -1);
        
        virtual bool        mkdir(QtdSync::RsyncLocation& config, QString name);
        virtual bool        rmdir(QtdSync::RsyncLocation& config, QString name);

        virtual QtdFileEntryInfoList dir(QtdSync::Location& config, QtdDirTreeModel* pDirTreeModel = 0L);
 
        /*
        QtdDirTreeModel*    getDirTreeModel(QtdSync::Location& config);
        QtdDirTreeModel*    getDirTreeModel();
        */

        /*
        bool                toXml(QDomElement& elm);
        bool                fromXml(QDomElement elem);
        */

    public slots:
        virtual void        setProcessPriority(int);


    public:
        QStringList         getRsyncModules(QtdSync::RsyncLocation& config);

        void                setDefaultRsyncOptions(QStringList lst);
        EnRsyncError        doRsync(QtdSync::RsyncLocation& src, QtdSync::RsyncLocation& dst, int msTimeout = -1);
 
        QTD_DECLAREPROTECTED(QtdSync::RsyncBackupEngine);
    };

} // namespace QtdSync

#endif // QTDSYNC_BACKUPENGINE_RSYNC_H