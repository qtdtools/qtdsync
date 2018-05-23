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

#include "QtdSync.Location.h"
#include "QtdTools.h"

#include <assert.h>

//============================================================================
class QtdSync::Location::Private
{
protected:
    //--------------------------------------------------------------------
    Private(const QString& identifier, const QString& label) {
        m_identifier = identifier;
        m_label      = label;
    }

    //--------------------------------------------------------------------
    Private(const Private& copyFrom) {
        m_identifier = copyFrom.m_identifier;
        m_label = copyFrom.m_label;
        m_configEntries = copyFrom.m_configEntries;
        m_supportedConfigEntries = copyFrom.m_supportedConfigEntries;
    }

    //--------------------------------------------------------------------
    virtual ~Private() {

    }

    //--------------------------------------------------------------------
    QMap<QString, QVariant> m_configEntries;
    QVariant                m_emptyEntry;
    QStringList             m_supportedConfigEntries;

    QString                 m_identifier;
    QString                 m_label;
    friend class QtdSync::Location;
};

//============================================================================
QtdSync::Location::Location()
{
    QString randNum = QString("%1").arg(rand());
    p = new QtdSync::Location::Private("location_" + randNum, "label_" + randNum);
}

//----------------------------------------------------------------------------
QtdSync::Location::Location(const QString& identifier, const QString& label)
{
    p = new QtdSync::Location::Private(identifier, label);
}

//----------------------------------------------------------------------------
QtdSync::Location::Location(const Location& copyFrom)
{
    p = new QtdSync::Location::Private(*(copyFrom.p));
}

//----------------------------------------------------------------------------
QtdSync::Location::~Location()
{
    if (p) {
        delete p;
        p = 0L;
    }
}

//----------------------------------------------------------------------------
QString QtdSync::Location::identifier() const
{
    return p->m_identifier;
}

//----------------------------------------------------------------------------
QString QtdSync::Location::label() const
{
    QString context = "QtdSync.Location." + identifier();
    return qApp->translate(context.toLatin1().data(), p->m_label.toLatin1().data());
}

//----------------------------------------------------------------------------
QString QtdSync::Location::description() const
{
#if defined(WIN32) && !defined(NDEBUG)
    _wassert(_CRT_WIDE("Missing QtdSync::Location::description() reimplementation!"), _CRT_WIDE(__FILE__), __LINE__);
#endif
    return "";
}

//----------------------------------------------------------------------------
QString QtdSync::Location::toString()
{
#if defined(WIN32) && !defined(NDEBUG)
    _wassert(_CRT_WIDE("Missing QtdSync::Location::toString() reimplementation!"), _CRT_WIDE(__FILE__), __LINE__);
#endif
    return "";
}

//----------------------------------------------------------------------------
bool QtdSync::Location::isValid()
{
#if defined(WIN32) && !defined(NDEBUG)
    _wassert(_CRT_WIDE("Missing QtdSync::Location::isValid() reimplementation!"), _CRT_WIDE(__FILE__), __LINE__);
#endif
    return false;
}

//----------------------------------------------------------------------------
QIcon QtdSync::Location::icon() const
{
#if defined(WIN32) && !defined(NDEBUG)
    _wassert(_CRT_WIDE("Missing QtdSync::Location::icon() reimplementation!"), _CRT_WIDE(__FILE__), __LINE__);
#endif
    return QIcon();
}

/*
//----------------------------------------------------------------------------
QtdSync::Location& QtdSync::Location::operator=(const QtdSync::Location& copyFrom)
{
    if (this != &copyFrom && identifier() == copyFrom.p->m_identifier) {
        p->m_configEntries = copyFrom.p->m_configEntries;
        p->m_supportedConfigEntries = copyFrom.p->m_supportedConfigEntries;
    }
    return *this;
}
*/

//----------------------------------------------------------------------------
bool QtdSync::Location::set(const QString& entryName, QVariant entry)
{
    if (!supports(entryName)) {
        assert(0);
        return false;
    }

    p->m_configEntries[entryName] = entry;
    return true;
}

//----------------------------------------------------------------------------
bool QtdSync::Location::remove(const QString& entryName)
{
    if (supports(entryName)) {
        p->m_configEntries.remove(entryName);
        return !supports(entryName);
    }
    return false;
}

//----------------------------------------------------------------------------
QVariant& QtdSync::Location::operator[](const QString& entryName)
{
    if (!contains(entryName)) {
        p->m_emptyEntry = QVariant();
        return p->m_emptyEntry;
    }

    return p->m_configEntries[entryName];
}

//----------------------------------------------------------------------------
bool QtdSync::Location::supports(const QString& entryName)
{
    return p->m_supportedConfigEntries.contains(entryName);
}

//----------------------------------------------------------------------------
QVariant QtdSync::Location::get(const QString& entryName)
{
    if (contains(entryName)) {
        return p->m_configEntries[entryName];
    }
    return p->m_emptyEntry;
}


//----------------------------------------------------------------------------
bool QtdSync::Location::contains(const QString& entryName)
{
    return p->m_configEntries.contains(entryName);
}

//----------------------------------------------------------------------------
const QVariant& QtdSync::Location::operator[](const QString& entryName) const
{
    if (!p->m_configEntries.contains(entryName)) {
        p->m_emptyEntry = QVariant();
        return p->m_emptyEntry;
    }

    return p->m_configEntries[entryName];
}


//----------------------------------------------------------------------------
void QtdSync::Location::addSupportedConfigEntry(QString entryName)
{
    if (!entryName.isEmpty() && !supports(entryName)) {
        p->m_supportedConfigEntries << entryName;
    }
}

//----------------------------------------------------------------------------
void QtdSync::Location::addSupportedConfigEntries(QStringList entryNames)
{
    foreach(QString entryName, entryNames) {
        addSupportedConfigEntry(entryName);
    }
}

//----------------------------------------------------------------------------
QStringList QtdSync::Location::supportedEntries()
{
    return p->m_supportedConfigEntries;
}

//----------------------------------------------------------------------------
void QtdSync::Location::reset(const QString& identifier, const QString& label)
{
    p->m_supportedConfigEntries.clear();
    p->m_configEntries.clear();
    p->m_identifier = identifier;
    p->m_label = label;
}

//----------------------------------------------------------------------------
bool QtdSync::Location::toXml(QDomElement& node, Location& loc)
{
    QDomDocument& doc = node.ownerDocument();

    node.setAttribute("id", QtdTools::codecSaveString(loc.identifier()));

    QMap<QString, QVariant>::Iterator it = loc.p->m_configEntries.begin();
    for (; it != loc.p->m_configEntries.end(); it++) {
        QDomElement entryNode = doc.createElement("entry");
        entryNode.setAttribute("name", it.key());
        entryNode.setAttribute("type", (*it).typeName());
        entryNode.appendChild(doc.createTextNode(QtdTools::codecSaveString((*it).toString())));
    }
    return true;
}

//----------------------------------------------------------------------------
bool QtdSync::Location::toXml(QDomElement& node)
{
    return toXml(node, *this);
}

//----------------------------------------------------------------------------
bool QtdSync::Location::fromXml(QDomElement& node, Location& loc)
{
    if (!node.hasAttribute("id") || QtdTools::codecSaveString(node.attribute("id")) != loc.identifier()) {
        // incompatible identifier
        return false;
    }

    // clear config entries
    loc.p->m_configEntries.clear();

    QDomElement entryNode = node.firstChildElement("entry");
    while (!entryNode.isNull()) {
        if (!entryNode.hasAttribute("name") || !entryNode.hasAttribute("type")) {
            continue;
        }

        QString key = entryNode.attribute("name", "");
        QString typeStr = entryNode.attribute("type", "");
        QVariant::Type type = QVariant::nameToType(typeStr.toLatin1().data());

        if (loc.supports(key) && type != QVariant::Invalid) {
            QVariant value = entryNode.text();
            if (value.convert(type)) {
                loc.set(key, value);
            }
        }

        entryNode = entryNode.nextSiblingElement("entry");
    }
    return true;
}

//----------------------------------------------------------------------------
bool QtdSync::Location::fromXml(QDomElement& node)
{
    return fromXml(node, *this);
}
