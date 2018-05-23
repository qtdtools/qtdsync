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
#include "QtdSync.Location.Rsync.h"
#include "QtdCrypt.h"
#include "QtdTools.h"

#include <assert.h>

#define QTDSYNC_RSYNCLOCATION_ID        "Rsync"
#define QTDSYNC_RSYNCLOCATION_LABEL     "rsync"

//----------------------------------------------------------------------------
QtdSync::RsyncLocation::RsyncLocation()
    : QtdSync::Location(QTDSYNC_RSYNCLOCATION_ID, QTDSYNC_RSYNCLOCATION_LABEL)
{
    initializeSupportedConfigEntries();
}

//----------------------------------------------------------------------------
QtdSync::RsyncLocation::RsyncLocation(const QtdSync::Location& copy)
    : QtdSync::Location(QTDSYNC_RSYNCLOCATION_ID, QTDSYNC_RSYNCLOCATION_LABEL)
{
    initializeSupportedConfigEntries();

    QStringList entries = supportedEntries();
    QtdSync::Location loc(copy);
    foreach(QString entryName, entries) {
        if (loc.contains(entryName)) {
            set(entryName, loc.get(entryName));
        }
    }
}

//----------------------------------------------------------------------------
QtdSync::RsyncLocation::RsyncLocation(const QtdSync::RsyncLocation& copy)
    : QtdSync::Location(copy)
{
    if (identifier() != QTDSYNC_RSYNCLOCATION_ID) {
        QtdSync::Location::reset(QTDSYNC_RSYNCLOCATION_ID, QTDSYNC_RSYNCLOCATION_LABEL);
        initializeSupportedConfigEntries();
    }
}

//----------------------------------------------------------------------------
QtdSync::RsyncLocation::RsyncLocation(const QtdSync::LocalLocation& copy)
    : QtdSync::Location(QTDSYNC_RSYNCLOCATION_ID, QTDSYNC_RSYNCLOCATION_LABEL)
{
    QtdSync::LocalLocation* pCopyLoc = (QtdSync::LocalLocation*)&copy;
    set("path", pCopyLoc->get("path"));
}

//----------------------------------------------------------------------------
void QtdSync::RsyncLocation::initializeSupportedConfigEntries()
{
    QStringList configEntries;
    configEntries << "path";
    configEntries << "useSSH";
    configEntries << "username";
    configEntries << "password";
    configEntries << "keyfile";
    configEntries << "excludes";
    configEntries << "excludeFilters";
    configEntries << "rsyncFlags";
    configEntries << "abortOnAuthenticationError";

    addSupportedConfigEntries(configEntries);
}

//----------------------------------------------------------------------------
QtdSync::RsyncLocation::~RsyncLocation()
{
}

//----------------------------------------------------------------------------
QIcon QtdSync::RsyncLocation::icon() const
{
    return icon(eRsync);
}
 
//----------------------------------------------------------------------------
QString QtdSync::RsyncLocation::description() const
{
    QString descr = "Location for Rsync BackupEngine";
    QString context = "QtdSync.Location." + identifier();
    return qApp->translate(context.toLatin1().data(), descr.toLatin1().data());
}

//----------------------------------------------------------------------------
QIcon QtdSync::RsyncLocation::icon(QString path)
{
    return icon(type(path));
}

//----------------------------------------------------------------------------
QIcon QtdSync::RsyncLocation::icon(EnType eType)
{
    switch (eType) {
        case eRsync: 
            return QIcon(":/images/icon_yellow.png");
        case eRsyncSSH:   
            return QIcon(":/images/icon_green.png");
        case eLocal:   
        default:
            return QIcon(":/images/icon_blue.png");
    }
}
 

//----------------------------------------------------------------------------
QtdSync::RsyncLocation::EnType QtdSync::RsyncLocation::type()
{
    if (!contains("path") || get("path").toString().isEmpty()) {
        return eUndefined;
    } else {
        return type(get("path").toString());
    }  
}

//----------------------------------------------------------------------------
QtdSync::RsyncLocation::EnType QtdSync::RsyncLocation::type(QString path)
{
    if (path.startsWith("rsync://")) {
        return eRsync;
    } else if (path.startsWith("ssh://")) {
        return eRsyncSSH;
    } else if (!path.isEmpty()) {
        return eLocal;
    } else {
        return eUndefined;
    }
}

//----------------------------------------------------------------------------
bool QtdSync::RsyncLocation::isValid()
{
    return type() != eUndefined;
}

//----------------------------------------------------------------------------
QtdSync::RsyncLocation QtdSync::RsyncLocation::local(QString path)
{
    QtdSync::RsyncLocation config;
    config.set("path", path);
    return config;
}

//----------------------------------------------------------------------------
QtdSync::RsyncLocation QtdSync::RsyncLocation::ssh(QString host, QString path, QString user, QString passwd, QString keyfile, int nPort)
{
    QtdSync::RsyncLocation config;
    config.set("path", "ssh://" + host + (nPort != -1 ? QString("%1").arg(nPort) : "") + (path != "" ? "/" + path : ""));
    config.set("username", user);
    config.set("useSSH", true);

    if (passwd != "") {
        config.set("password", passwd);
    }

    if (keyfile != "") {
        config.set("keyfile", keyfile);
    }

    return config;
}

//----------------------------------------------------------------------------
QtdSync::RsyncLocation QtdSync::RsyncLocation::rsync(QString host, QString path, QString user, QString passwd, int nPort)
{
    QtdSync::RsyncLocation config;
    config.set("path", "rsync://" + host + (nPort != -1 ? QString("%1").arg(nPort) : "") + (path != "" ? "/" + path : ""));

    if (user != "") {
        config.set("username", user);
        if (passwd != "") {
            config.set("password", passwd);
        }
    }
    
    return config;
}

//----------------------------------------------------------------------------
QStringList QtdSync::RsyncLocation::composeInitialRsyncArgs(QString m_binPath, QStringList& env)
{
    QStringList     args;
    if (!contains("path") || get("path").toString().isEmpty()) {
        return args;
    }

    int             nPort       = -1;
    EnType          eType       = type();
    QString         password    = "";
    QString         keyfile     = "";
    QString         path        = get("path").toString();
    QString         user        = "";

    if (eType <= eLocal) {
        return args;
    }

    password = get("password").toString();
    
    if (eType == eRsyncSSH) {
        keyfile  = get("keyfile").toString();
    }

    user = get("username").toString();
    if (eType == eRsyncSSH && 
        (user.isEmpty() || (password.isEmpty() && keyfile.isEmpty()))) {
        return args;
    }

    path = composeLocationString(&nPort);
    QString sshPath = execPath(m_binPath, "ssh");
    QString sshPassPath = execPath(m_binPath, "sshpass");

    if (password != "") {

        QStringList oldEnv = env;
        env.clear();
        foreach (QString e, oldEnv) {
            if (!e.startsWith("SSHPASS") && !e.startsWith("RSYNC_PASSWORD")) {
                env << e;
            }
        }

        if (eType == eRsyncSSH) {
            env << "SSHPASS=" + password;
            args << "-e" << QString("\"" + sshPassPath + "\" -e \"" + sshPath + "\" %1-o StrictHostKeyChecking=no -o NumberOfPasswordPrompts=1").arg(nPort != -1 ? QString("-p %1 ").arg(nPort) : "");
        } else {
            env << "RSYNC_PASSWORD=" + password;
        }

    } else if (eType != eRsyncSSH) {

        env << "RSYNC_PASSWORD=dummypw";

    }
    
    if (keyfile != "") {
        QFileInfo fi(keyfile);
        QString keyfile = fi.absoluteFilePath();
#ifdef WIN32
        if (keyfile.at(1) == ':') {
            keyfile = QString("/cygdrive/%1/%2").arg(keyfile.at(0)).arg(keyfile.mid(2));
        }
#endif
        args << "-e" << QString("\"" + sshPath + "\" %1-i \"%2\" -o StrictHostKeyChecking=no -o PreferredAuthentications=hostbased,publickey -o NumberOfPasswordPrompts=0").arg(nPort != -1 ? QString("-p %1 ").arg(nPort) : "").arg(keyfile); 
    }

    if (eType == eRsyncSSH && nPort != -1) {
        args << QString("--port=%1").arg(nPort);
    }

    return args;

}

//----------------------------------------------------------------------------
QString QtdSync::RsyncLocation::composeLocationString(int* pnPort)
{
    if (!contains("path") || get("path").isNull()) {
        return "";
    }

    QString path = QDir::fromNativeSeparators(get("path").toString());
    EnType  eType = type();

    if (eType <= eLocal) {
        if (!QFileInfo(path).isAbsolute()) {
            path = QDir::current().absoluteFilePath(path);
        }
#ifdef WIN32
        if (path.at(1) == QChar(':')) {
            path = QString("/cygdrive/%1/%2").arg(path.at(0)).arg(path.mid(3));
        }
#endif
        if (!path.endsWith("/")) path += "/";

    } else {
        QUrl url(path);
        QString host = url.host();
        int     port = url.port();
        QString schm = url.scheme();
        QString fpath = url.path();
        QString user = "";
        bool    bUseSSH = (schm == "ssh");

        if (!bUseSSH && schm != "rsync") {
            return "";
        }

        if (contains("username")) {
            user = get("username").toString();
        }

        if (pnPort) {
            *pnPort = port;
        }
        if (fpath != "") fpath += "/";

        path = QString("%1%2%3%4%5")
                .arg(user != "" ? user + "@" : "")
                .arg(host)
                .arg(!pnPort ? QString(":%1").arg(port) : "")
                .arg(bUseSSH ? ":" : "::")
                .arg(fpath != "" ? fpath.mid(1) : "");
    }

    return path;

}

//----------------------------------------------------------------------------
QString QtdSync::RsyncLocation::toString()
{
    return composeLocationString();
}

//----------------------------------------------------------------------------
QString QtdSync::RsyncLocation::execPath(QString m_binPath, QString app)
{
    return QDir::toNativeSeparators(QDir(m_binPath).absoluteFilePath(EXEC(app)));
}