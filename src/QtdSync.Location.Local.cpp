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

#include "QtdSync.Location.Local.h"

#define QTDSYNC_LOCALLOCATION_ID        "Local"
#define QTDSYNC_LOCALLOCATION_LABEL     "Local"

//============================================================================
QtdSync::LocalLocation::LocalLocation()
    : QtdSync::Location(QTDSYNC_LOCALLOCATION_ID, QTDSYNC_LOCALLOCATION_LABEL)
{
    initializeSupportedConfigEntries();
}

//----------------------------------------------------------------------------
QtdSync::LocalLocation::LocalLocation(const Location& copyFrom)
    : QtdSync::Location(copyFrom)
{
    if (identifier() != QTDSYNC_LOCALLOCATION_ID) {
        reset(QTDSYNC_LOCALLOCATION_ID, QTDSYNC_LOCALLOCATION_LABEL);
        initializeSupportedConfigEntries();
    }
}

//----------------------------------------------------------------------------
QtdSync::LocalLocation::~LocalLocation()
{}

//----------------------------------------------------------------------------
void QtdSync::LocalLocation::initializeSupportedConfigEntries()
{
    QStringList supportedEntries;
    supportedEntries << "path";
    supportedEntries << "excludes";
    supportedEntries << "excludeFilters";

    addSupportedConfigEntries(supportedEntries);
}

//----------------------------------------------------------------------------
QString QtdSync::LocalLocation::description() const
{
    static QString descr = "Represents a location on the local file system.";
    return qApp->translate("QtdSync.Location." QTDSYNC_LOCALLOCATION_ID, descr.toLatin1().data());
}

//----------------------------------------------------------------------------
QString QtdSync::LocalLocation::toString()
{
    return get("path").toString();
}

//----------------------------------------------------------------------------
bool QtdSync::LocalLocation::isValid()
{
    return QFileInfo(get("path").toString()).exists();
}

//----------------------------------------------------------------------------
QIcon QtdSync::LocalLocation::icon() const
{
    return QIcon();
}