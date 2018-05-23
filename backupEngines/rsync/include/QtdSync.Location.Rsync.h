/*
 *  This file is part of "QtdSync v0.7" (https://www.qtdtools.de)
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
#ifndef QTDSYNC_LOCATION_RSYNC_H
#define QTDSYNC_LOCATION_RSYNC_H

#include "QtdSync.Location.h"
#include "QtdSync.Location.Local.h"
#include "QtdSync.BackupEngine.Rsync.h"

namespace QtdSync {
    //----------------------------------------------------------------------------
    class RsyncLocation : public QtdSync::Location
    {
    public:
        typedef enum {
             eUndefined = 0
            ,eLocal
            ,eRsync
            ,eRsyncSSH
        } EnType;

        //----------------------------------------------------------------------------
        // Member functions
        //----------------------------------------------------------------------------
        RsyncLocation(const Location&);
        RsyncLocation(const RsyncLocation&);
        RsyncLocation(const LocalLocation&);

        virtual ~RsyncLocation();

        //----------------------------------------------------------------------------
        // QtdSync.Location reimplementations
        virtual bool                isValid();
        virtual QIcon               icon() const;
        virtual QString             toString();
        virtual QString             description() const;

        //----------------------------------------------------------------------------
        EnType                      type();
        static EnType               type(QString path);

        static QIcon                icon(QString path);
        static QIcon                icon(EnType eType);

        static RsyncLocation        local(QString path);
        static RsyncLocation        ssh(QString host, QString path, QString user, QString passwd = "", QString keyfile = "", int nPort = -1);
        static RsyncLocation        rsync(QString host, QString path = "", QString user = "", QString passwd = "", int nPort = -1);

        RsyncLocation&              operator=(const RsyncLocation&);
        RsyncLocation&              operator=(const LocalLocation&);

    protected:
        RsyncLocation();

        QStringList                 composeInitialRsyncArgs(QString m_binPath, QStringList& env);
        QString                     composeLocationString(int* pnPort = 0L);
        void                        initializeSupportedConfigEntries();
        static QString              execPath(QString binPath, QString app);

        friend class QtdSync::RsyncBackupEngine;
    };
} // namespace QtdSync

#endif // QTDSYNC_LOCATION_RSYNC_H