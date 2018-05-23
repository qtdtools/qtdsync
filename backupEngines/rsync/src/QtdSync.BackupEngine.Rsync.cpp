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
#include "QtdSync.BackupEngine.Rsync.h"
#include "QtdSync.Location.Rsync.h"
#include "QtdCrypt.h"
#include <assert.h>

extern "C" {
//#include "ftpparse.h"
};

#define RSYNCBACKUPENGINE_ID        "Rsync"
#define RSYNCBACKUPENGINE_LABEL     "Rsync"

//============================================================================
class QtdSync::RsyncBackupEngine::Private
{
protected:
    //------------------------------------------------------------------------
    Private(QtdSync::RsyncBackupEngine* pParent = 0L) {
        m_pParent = pParent;
        m_binPath = qApp->applicationDirPath() + "/bin";
        m_pProcess = new QProcess(pParent);
    }

    //------------------------------------------------------------------------
    ~Private() {
        if (m_pProcess->state() != QProcess::NotRunning) {
            m_pProcess->close();
        }
        m_pProcess->deleteLater();
    }

    //------------------------------------------------------------------------
    QString     m_binPath;
    QStringList m_defaultRsyncParams;
    QProcess*   m_pProcess;
    QtdSync::RsyncBackupEngine* m_pParent;

    friend class QtdSync::RsyncBackupEngine;
};

//============================================================================
QtdSync::RsyncBackupEngine::RsyncBackupEngine(QObject* pParent)
: QtdSync::BackupEngine(RSYNCBACKUPENGINE_ID, RSYNCBACKUPENGINE_LABEL, pParent)
{
    p = new QtdSync::RsyncBackupEngine::Private(this);
}

//----------------------------------------------------------------------------
QtdSync::RsyncBackupEngine::~RsyncBackupEngine()
{
}

//----------------------------------------------------------------------------
QIcon QtdSync::RsyncBackupEngine::icon()
{
    return QIcon();
}

//----------------------------------------------------------------------------
QString QtdSync::RsyncBackupEngine::description()
{
    QString descr = "Rsync BackupEngine";
    QString context = "QtdSync.BackupEngine." + identifier();
    return qApp->translate(context.toLatin1().data(), descr.toLatin1().data());
}

//----------------------------------------------------------------------------
// SLOTS
//----------------------------------------------------------------------------
void QtdSync::RsyncBackupEngine::setProcessPriority(int prio)
{
    if (p->m_pProcess->state() == QProcess::Running) {
        QtdTools::setProcessPriority(p->m_pProcess, (QtdTools::ProcessPriority)prio);
    }
    QtdSync::BackupEngine::setProcessPriority(prio);
}

//----------------------------------------------------------------------------
// Rsync Tools
//----------------------------------------------------------------------------
void QtdSync::RsyncBackupEngine::setDefaultRsyncOptions(QStringList lst)
{
    p->m_defaultRsyncParams = lst;
}

//----------------------------------------------------------------------------
QtdSync::RsyncBackupEngine::EnRsyncError QtdSync::RsyncBackupEngine::doRsync(QtdSync::RsyncLocation& src, QtdSync::RsyncLocation& dst, int msTimeout)
{
    QStringList env;
    QStringList dstArgs = dst.composeInitialRsyncArgs(p->m_binPath, env);
    QStringList srcArgs = src.composeInitialRsyncArgs(p->m_binPath, env);
    
    if (dst.type() != RsyncLocation::eLocal && src.type() != RsyncLocation::eLocal) {
        // remote to remote connections are not allowed
        return eRemoteRemote;
    }

    QStringList rsyncArgs("-rltv");
    int         nPort       = -1;
    QString     dstStr  = dst.composeLocationString(&nPort);
    QString     srcStr  = src.composeLocationString(&nPort);

    rsyncArgs << srcArgs << dstArgs;
    rsyncArgs << p->m_defaultRsyncParams;

#ifdef WIN32
    if (rsyncArgs.indexOf(QRegExp("--chmod=.*")) == -1 && rsyncArgs.indexOf("-p") == -1) {
        rsyncArgs << "--chmod=+rwX";
    }
#else
    if (rsyncArgs.indexOf(QRegExp("--chmod=.*")) == -1 && rsyncArgs.indexOf("-no-p") == -1) {
        rsyncArgs << "-p";
    }
#endif


    if (src.contains("rsyncFlags")) {
        QStringList flags = src.get("rsyncFlags").toStringList();
        if (flags.first() == "use-global-settings") {
            flags.clear();
        }
        if (flags.count() > 0) {
            rsyncArgs << flags;
        }
    }
    
    if (dst.contains("rsyncFlags")) {
        QStringList flags = dst.get("rsyncFlags").toStringList();
        if (flags.first() == "use-global-settings") {
            flags.clear();
        }
        if (flags.count() > 0) {
            rsyncArgs << flags;
        }
    }

#ifdef WIN32
    rsyncArgs << "--exclude=/cygdrive";
#endif
    rsyncArgs << "--exclude=/proc";
    rsyncArgs << "--progress";

    if (src.contains("excludes")) {
        QStringList excludes = src.get("excludes").toStringList();
        foreach (QString exFile, excludes) {
            QString::iterator it = exFile.begin();
            for (it; it != exFile.end(); it++) {
                unsigned char t = it->toLatin1();
                if (t > char(127)) {
                    *it = QChar('*');
                }
            }

            rsyncArgs << "--exclude=" + exFile;
        }
    }

    if (src.contains("excludeFilters")) {
        QStringList excludeFilters = src.get("excludeFilters").toStringList();
        foreach (QString exFile, excludeFilters) {
            QString::iterator it = exFile.begin();
            for (it; it != exFile.end(); it++) {
                unsigned char t = it->toLatin1();
                if (t > char(127)) {
                    *it = QChar('*');
                }
            }

            rsyncArgs << "--exclude=" + exFile;
        }
    }

    if (dst.contains("excludes")) {
        QStringList excludes = dst.get("excludes").toStringList();
        foreach (QString exFile, excludes) {
            QString::iterator it = exFile.begin();
            for (it; it != exFile.end(); it++) {
                unsigned char t = it->toLatin1();
                if (t > char(127)) {
                    *it = QChar('*');
                }
            }

            rsyncArgs << "--exclude=" + exFile;
        }
    }

    if (srcStr != "") {
        rsyncArgs << srcStr;
    }

    if (dstStr != "") {
        rsyncArgs << dstStr;
    }

    p->m_pProcess->setEnvironment(env);
    p->m_pProcess->start(RsyncLocation::execPath(p->m_binPath, "rsync"), rsyncArgs);
    p->m_pProcess->waitForStarted(-1);
    setProcessPriority(processPriority());

    int nWaitForReadReady = msTimeout;
    if (nWaitForReadReady < 0) {
        nWaitForReadReady = 100;
    }

    bool bFinished = (msTimeout >= 0);
    bool bAuthError = false;
    QString allErrors;

    int                 rangeMax = 1;
    int                 nCurCount = 1;
    double              dPerCentFolder   = 0.0;
    double              dPerCentFile     = 0.0;
    QString             curLine;
    unsigned long long  nTotalFileSize   = 0;
    unsigned long long  nAllFilesSize    = 0;
    EnRsyncError        eSuccess         = eNoError;

    do {
        qApp->processEvents();
        bFinished |= (p->m_pProcess->state() == QProcess::NotRunning);

        if (bFinished) {
            p->m_pProcess->waitForFinished(nWaitForReadReady);
        } else {
            p->m_pProcess->waitForReadyRead(nWaitForReadReady);
        }
        QString out = QString::fromUtf8(p->m_pProcess->readAllStandardOutput());
        QStringList outLines = out.split("\n");
                   
        foreach (QString outLine, outLines) {
            qApp->processEvents();
            bool bGotSomeProgress = false;

            outLine = outLine.replace("\r", "");
            if (outLine == "") continue;

            if (!outLine.startsWith(" ")) {
                bGotSomeProgress = true;
                curLine = outLine;
            }

            QRegExp regExp("to-check=(\\d+)/(\\d+)");
            if (regExp.indexIn(outLine) != -1 && regExp.captureCount() > 1) {
                bGotSomeProgress = true;
                rangeMax = regExp.cap(2).toInt();
                nCurCount = (rangeMax - regExp.cap(1).toInt());

                // calc progress stats
                dPerCentFile   = 0.0;
                dPerCentFolder = ((double)(nCurCount - 1) * 100.0)       / (double)(rangeMax*100.0);
            }

            regExp.setPattern("\\s(\\d+)%\\s");
            if (regExp.indexIn(outLine) != -1 && regExp.captureCount() > 0) {
                unsigned int nValue = regExp.cap(1).toInt();
                bGotSomeProgress = true;

                dPerCentFile   = ((double)nValue) / 100.0;
                dPerCentFolder = ((double)(nCurCount - 1) * 100.0 + nValue) / (double)(rangeMax*100.0);
            }

            regExp.setPattern("Total\\stransferred\\sfile\\ssize:\\s(\\d+)\\sbytes");
            if (regExp.indexIn(outLine) != -1 && regExp.captureCount() > 0) {
                nTotalFileSize += regExp.cap(1).toULongLong() / 1024;
            }

            regExp.setPattern("Total\\sfile\\ssize:\\s(\\d+)\\sbytes");
            if (regExp.indexIn(outLine) != -1 && regExp.captureCount() > 0) {
                nAllFilesSize += regExp.cap(1).toULongLong() / 1024;
            }

            if (bGotSomeProgress) {
                emit progress(curLine, dPerCentFile, dPerCentFolder);
            }
        }

        QString errors = p->m_pProcess->readAllStandardError();
        allErrors += errors;
        if (!dst.contains("abortOnAuthenticationError") && !src.contains("abortOnAuthenticationError") && errors.toLower().contains("auth failed")) {
            QString username, password;
            QAuthenticator auth;
            QString host;

            eSuccess = eAuthentication;

            if (dstArgs.count() > 0) {
                host = dst.get("path").toString();
            } else if (srcArgs.count() > 0) {
                host = src.get("path").toString();
            }
            emit authenticationRequired(host, auth);

            username = auth.user();
            password = auth.password();

            if (p->m_pProcess->state() == QProcess::Running) {
                p->m_pProcess->terminate();
            }

            if (dst.type() != RsyncLocation::eLocal) {
                RsyncLocation newDst(dst);
                newDst.set("username", username);
                newDst.set("password", password);
                newDst.set("abortOnAuthenticationError", true);
                if (doRsync(src, newDst, msTimeout) == eNoError) {
                    dst.set("username", username);
                    dst.set("password", password);
                    eSuccess = eNoError;
                }
            } else if (src.type() != RsyncLocation::eLocal) {
                RsyncLocation newSrc(src);
                newSrc.set("username", username);
                newSrc.set("password", password);
                newSrc.set("abortOnAuthenticationError", true);
                if (doRsync(newSrc, dst, msTimeout) == eNoError) {
                    src.set("username", username);
                    src.set("password", password);
                    eSuccess = eNoError;
                }
            }
            setLastError(eSuccess);
            return eSuccess;
        }

    } while (!bFinished);

    if (p->m_pProcess->state() == QProcess::Running) {
        p->m_pProcess->terminate();
        eSuccess = eTimedOut;
    }

    setLastError(eSuccess);
    return eSuccess;

}

//----------------------------------------------------------------------------
bool QtdSync::RsyncBackupEngine::mkdir(QtdSync::RsyncLocation& parent, QString name)
{
    RsyncLocation src = RsyncLocation::local("./");
    RsyncLocation config(parent);

    src.set("excludeFilters", QStringList("*"));

    QString     builtPath("");
    QString     dest = config.get("path").toString();

    name.replace("\\", "/");
    foreach (QString dir, name.split("/", QString::SkipEmptyParts)) {
        builtPath += dir;
        config.set("path", dest + "/" + builtPath);
        builtPath += "/";

        if (doRsync(src, config, 2000) != eNoError) {
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
bool QtdSync::RsyncBackupEngine::rmdir(QtdSync::RsyncLocation& config, QString name)
{
    RsyncLocation src = RsyncLocation::local("./");
    src.set("excludeFilters", QStringList("*"));
    src.set("rsyncFlags", QStringList("--delete"));

    QString dst     = config.get("path").toString();

    name = QDir::fromNativeSeparators(name);

    // first, we clear the folder
    config.set("rsyncFlags", QStringList("--delete-excluded"));
    config.set("path", dst + "/" + name);

    if (doRsync(src, config) == eNoError) {
        // second, we delete the folder
        QStringList names = name.split("/", QString::SkipEmptyParts);
        QString last = names.last();
        names.pop_back();

        config.set("rsyncFlags", QStringList("--include=" + last));

        if (names.count() > 0) {
            last = "/" + names.join("/");
        } else {
            last = "";
        }
        config.set("path", dst + last);

        return (doRsync(src, config) == eNoError);
    }
    return false;
}

//----------------------------------------------------------------------------
QStringList QtdSync::RsyncBackupEngine::getRsyncModules(QtdSync::RsyncLocation& config)
{
    if (!config.contains("path") || (config.type() != RsyncLocation::eRsync && config.type() != RsyncLocation::eRsyncSSH)) {
        return QStringList();
    }

    QStringList ret;
    QStringList env;
    int nPort = -1;
    QStringList args = config.composeInitialRsyncArgs(p->m_binPath, env);
    QString     dest = config.composeLocationString(&nPort);

    dest = dest.split("::").at(0) + "::";
    args << dest;

    p->m_pProcess->setEnvironment(env);
    p->m_pProcess->start(RsyncLocation::execPath(p->m_binPath, "rsync"), args);
    p->m_pProcess->waitForFinished(30000);

    if (p->m_pProcess->state() == QProcess::Running) {
        p->m_pProcess->terminate();
    } else if (p->m_pProcess->exitCode() != 0) {
        QString errors = p->m_pProcess->readAllStandardError();
        if (!config.contains("abortOnAuthenticationError") && (errors.toLower().contains("auth failed") || errors.toLower().contains("permission denied"))) {
            setLastError(eAuthenticationFailed);
            QString username, password;
            QAuthenticator auth;
            emit authenticationRequired(config.get("path").toString(), auth);
            config.set("username", auth.user());
            config.set("password", auth.password());
            config.set("abortOnAuthenticationError", true);
            return getRsyncModules(config);
        } else if (errors.toLower().contains("connection timed out")) {
            setLastError(eConnectionFailed);
        }
    } else {
        QString out = p->m_pProcess->readAllStandardOutput();
        foreach (QString line, out.split("\n")) {
            if (line.trimmed() != "") {
                QString name = line.split("\t", QString::SkipEmptyParts)[0].trimmed();
                ret << name;
            }
        }
    }

    return ret;
}

//----------------------------------------------------------------------------
QtdFileEntryInfoList QtdSync::RsyncBackupEngine::dir(QtdSync::Location& loc, QtdDirTreeModel* pDirTreeModel)
{
    QtdFileEntryInfo   root;
    RsyncLocation config(loc);
    if (!config.contains("path")) {
        return root.files;
    }

    int nPort = -1;
    QString     dest = config.composeLocationString(&nPort);

    // first, get top parent dirs
    if (config.type() == RsyncLocation::eRsync && dest.endsWith("::")) {
        // we are in the rsync "root dir"
        QStringList modules = getRsyncModules(config);
        foreach (QString mod, modules) {
            RsyncLocation curConf(config);
            curConf.set("path", config.get("path").toString() + "/" + mod);
            QtdFileEntryInfo info = {mod, curConf.get("path").toString(), true, 0xFFFFFFFF, QDateTime::currentDateTime(), 0, dir(curConf)};
            root.files.append(info);
        }
    } else {
        QList<QtdFileEntryInfo*> dirQueue;
        QStringList env;
        QStringList args = config.composeInitialRsyncArgs(p->m_binPath, env);

        args << "-r";
        args << dest;


        p->m_pProcess->setEnvironment(env);
        p->m_pProcess->start(RsyncLocation::execPath(p->m_binPath, "rsync"), args);
        p->m_pProcess->waitForFinished(-1);

        if (p->m_pProcess->exitCode() != 0) {
            QString errors = p->m_pProcess->readAllStandardError();
            if (!config.contains("abortOnAuthenticationError") && (errors.toLower().contains("auth failed") || errors.toLower().contains("permission denied"))) {
                QAuthenticator auth;
                emit authenticationRequired(config.get("path").toString(), auth);

                config.set("username", auth.user());
                config.set("password", auth.password());
                config.set("abortOnAuthenticationError", true);
                return dir(config);
            }
        } else {
            QString         out     = p->m_pProcess->readAllStandardOutput();

            root.name = "";
            dirQueue.append(&root);

            QStringList outLines = out.split("\n", QString::SkipEmptyParts);

            foreach (QString line, out.split("\n")) {
                QtdFileEntryInfo info;
                QStringList lineParts = line.split(" ", QString::SkipEmptyParts);

                if (lineParts.count() >= 5 && lineParts.at(4).trimmed() != ".") {
                    info.nSize = lineParts.at(1).trimmed().toLongLong();
                    info.timeStamp = QDateTime::fromString(lineParts.at(2).trimmed() + " " + lineParts.at(3).trimmed(), "yyyy/MM/ss hh:mm:ss");
                    info.bIsDir = lineParts.first().startsWith("d");

                    QString name = QStringList(lineParts.mid(4)).join(" ");
                    while (!name.startsWith(dirQueue.last()->path) && dirQueue.count() > 0) dirQueue.removeLast();

                    assert(dirQueue.count());

                    info.path = QDir::fromNativeSeparators(config.get("path").toString() + "/" + name);
                    QStringList nameParts = name.split("/");
                    info.name = nameParts.last();

                    dirQueue.last()->files.append(info);
                    if (info.bIsDir) dirQueue.append(&dirQueue.last()->files.last());
                }
            }
        }
    }
   
    return root.files;
 
}

/*
//----------------------------------------------------------------------------
QtdDirTreeModel* QtdSync::RsyncBackupEngine::getDirTreeModel(QtdRsyncLocation config)
{
    QtdFileEntryInfo root;
    root.bIsDir = true;
    root.name   = config[QtdRsyncLocation::ePath].toString();
    root.nSize  = 0;
    root.timeStamp = QDateTime::currentDateTime();
    root.path   = config[QtdRsyncLocation::ePath].toString();
    root.files.clear();

    QtdDirTreeModel* pModel = new QtdDirTreeModel(root);
    dir(config, pModel);

    return new QtdDirTreeModel(root);

}
*/