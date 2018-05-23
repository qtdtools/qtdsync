/*
 *  This file is part of "QtdSync v0.7" (http://www.qtdtools.de)
 *
 *  Copyright (C) 2009-2016 Thomas Doering
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
#ifndef QTDSYNC_LOCATION_H
#define QTDSYNC_LOCATION_H

#include "QtdSync.h"

namespace QtdSync {
    //----------------------------------------------------------------------------
    class Location {
    public:
        Location(const QString& identifier, const QString& label);
        Location(const Location& copyFrom);
        virtual ~Location();

        //------------------------------------------------------------------------
        QString             identifier() const;
        QString             label() const;
        virtual QIcon       icon() const;
        virtual QString     description() const;

        //------------------------------------------------------------------------
        virtual bool        set(const QString& entryName, QVariant entryValue);
        QVariant&           operator[](const QString& entryName);
        virtual bool        supports(const QString& entryName);
        virtual bool        remove(const QString& entryName);

        virtual QVariant    get(const QString& entryName);
        const QVariant&     operator[](const QString& entryName) const;
        virtual bool        contains(const QString& entryName);

        virtual bool        isValid();

        //------------------------------------------------------------------------
        virtual bool        toXml(QDomElement& elm);
        static bool         toXml(QDomElement& elm, Location& loc);
 
        virtual bool        fromXml(QDomElement& elm);
        static bool         fromXml(QDomElement& elm, Location& loc);

        virtual QString     toString();

    protected:
        Location();
        virtual void        addSupportedConfigEntry(QString type);
        virtual void        addSupportedConfigEntries(QStringList type);
        virtual QStringList supportedEntries();

        virtual void        reset(const QString& identifier, const QString& label);

        QTD_DECLAREPROTECTED(QtdSync::Location);
    };
}

#endif // QTDSYNC_LOCATION_H