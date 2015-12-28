/**********************************************************************************
 *   Copyright (C) 2008 by Carlo Segato <brandon.ml@gmail.com>                    *
 *                                                                                *
 *   This library is free software; you can redistribute it and/or                *
 *   modify it under the terms of the GNU Lesser General Public                   *
 *   License as published by the Free Software Foundation; either                 *
 *   version 2.1 of the License, or (at your option) any later version.           *
 *                                                                                *
 *   This library is distributed in the hope that it will be useful,              *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU            *
 *   Lesser General Public License for more details.                              *
 *                                                                                *
 *   You should have received a copy of the GNU Lesser General Public             *
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>.*
 *                                                                                *
 **********************************************************************************/

#include "adium_emoticons.h"

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>
#include <QDebug>

#include <kpluginfactory.h>

K_PLUGIN_FACTORY(AdiumEmoticonsFactory, registerPlugin<AdiumEmoticons>();)

AdiumEmoticons::AdiumEmoticons(QObject *parent, const QVariantList &args)
    : KEmoticonsProvider(parent)
{
    Q_UNUSED(args)
}

bool AdiumEmoticons::removeEmoticon(const QString &emo)
{
    QString emoticon = QFileInfo(emoticonsMap().key(emo.split(' '))).fileName();
    QDomElement fce = m_themeXml.firstChildElement(QStringLiteral("plist")).firstChildElement(QStringLiteral("dict")).firstChildElement(QStringLiteral("dict"));

    if (fce.isNull()) {
        return false;
    }

    QDomNodeList nl = fce.childNodes();
    for (int i = 0; i < nl.length(); i++) {
        QDomElement de = nl.item(i).toElement();
        if (!de.isNull() && de.tagName() == QLatin1String("key") && (de.text() == emoticon)) {
            QDomElement dict = de.nextSiblingElement();
            if (!dict.isNull() && dict.tagName() == QLatin1String("dict")) {
                fce.removeChild(dict);
            }

            fce.removeChild(de);
            removeMapItem(emoticonsMap().key(emo.split(' ')));
            removeIndexItem(emoticon, emo.split(' '));
            return true;
        }
    }
    return false;
}

bool AdiumEmoticons::addEmoticon(const QString &emo, const QString &text, AddEmoticonOption option)
{
    if (option == Copy) {
        bool result = copyEmoticon(emo);
        if (!result) {
            qWarning() << "There was a problem copying the emoticon";
            return false;
        }
    }

    const QStringList splitted = text.split(' ');
    QDomElement fce = m_themeXml.firstChildElement(QStringLiteral("plist")).firstChildElement(QStringLiteral("dict")).firstChildElement(QStringLiteral("dict"));

    if (fce.isNull()) {
        return false;
    }

    QDomElement emoticon = m_themeXml.createElement(QStringLiteral("key"));
    emoticon.appendChild(m_themeXml.createTextNode(QFileInfo(emo).fileName()));
    fce.appendChild(emoticon);

    QDomElement dict = m_themeXml.createElement(QStringLiteral("dict"));
    QDomElement el = m_themeXml.createElement(QStringLiteral("key"));
    el.appendChild(m_themeXml.createTextNode(QStringLiteral("Equivalents")));
    dict.appendChild(el);

    QDomElement arr = m_themeXml.createElement(QStringLiteral("array"));

    QStringList::const_iterator constIterator;
    for (constIterator = splitted.begin(); constIterator != splitted.end(); ++constIterator) {
        QDomElement emoText = m_themeXml.createElement(QStringLiteral("string"));
        QDomText txt = m_themeXml.createTextNode((*constIterator).trimmed());
        emoText.appendChild(txt);
        arr.appendChild(emoText);
    }

    dict.appendChild(arr);

    el = m_themeXml.createElement(QStringLiteral("key"));
    el.appendChild(m_themeXml.createTextNode(QStringLiteral("Name")));
    dict.appendChild(el);

    el = m_themeXml.createElement(QStringLiteral("string"));
    el.appendChild(m_themeXml.createTextNode(QFileInfo(emo).baseName()));
    dict.appendChild(el);

    fce.appendChild(dict);

    addIndexItem(emo, splitted);
    addMapItem(emo, splitted);
    return true;
}

void AdiumEmoticons::saveTheme()
{
    QFile fp(themePath() + '/' + fileName());

    if (!fp.exists()) {
        qWarning() << fp.fileName() << "doesn't exist!";
        return;
    }

    if (!fp.open(QIODevice::WriteOnly)) {
        qWarning() << fp.fileName() << "can't open WriteOnly!";
        return;
    }

    QTextStream emoStream(&fp);
    emoStream.setCodec("UTF-8");
    emoStream << m_themeXml.toString(4);
    fp.close();
}

bool AdiumEmoticons::loadTheme(const QString &path)
{
    QFile file(path);

    if (!file.exists()) {
        qWarning() << path << "doesn't exist!";
        return false;
    }

    setThemePath(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << file.fileName() << "can't be open ReadOnly!";
        return false;
    }

    QString error;
    int eli, eco;
    if (!m_themeXml.setContent(&file, &error, &eli, &eco)) {
        qWarning() << file.fileName() << "can't copy to xml!";
        qWarning() << error << "line:" << eli << "column:" << eco;
        file.close();
        return false;
    }

    file.close();

    QDomElement fce = m_themeXml.firstChildElement(QStringLiteral("plist")).firstChildElement(QStringLiteral("dict")).firstChildElement(QStringLiteral("dict"));

    if (fce.isNull()) {
        return false;
    }

    QDomNodeList nl = fce.childNodes();

    clearEmoticonsMap();
    QString name;
    for (int i = 0; i < nl.length(); i++) {
        QDomElement de = nl.item(i).toElement();

        if (!de.isNull() && de.tagName() == QLatin1String("key")) {
            name = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "emoticons/" + themeName() + '/' + de.text());
            continue;
        } else if (!de.isNull() && de.tagName() == QLatin1String("dict")) {
            QDomElement arr = de.firstChildElement(QStringLiteral("array"));
            QDomNodeList snl = arr.childNodes();
            QStringList sl;

            for (int k = 0; k < snl.length(); k++) {
                QDomElement sde = snl.item(k).toElement();

                if (!sde.isNull() && sde.tagName() == QLatin1String("string")) {
                    sl << sde.text();
                }
            }
            if (!name.isEmpty()) {
                addIndexItem(name, sl);
                addMapItem(name, sl);
                name.clear();
            }
        }
    }

    return true;
}

void AdiumEmoticons::newTheme()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/emoticons/" + themeName();
    QDir().mkpath(path);

    QFile fp(path + '/' + "Emoticons.plist");

    if (!fp.open(QIODevice::WriteOnly)) {
        qWarning() << fp.fileName() << "can't open WriteOnly!";
        return;
    }

    QDomDocumentType ty = QDomImplementation().createDocumentType(QStringLiteral("plist"), QStringLiteral("-//Apple Computer//DTD PLIST 1.0//EN"), QStringLiteral("http://www.apple.com/DTDs/PropertyList-1.0.dtd"));
    QDomDocument doc(ty);
    doc.appendChild(doc.createProcessingInstruction(QStringLiteral("xml"), QStringLiteral("version=\"1.0\" encoding=\"UTF-8\"")));

    QDomElement plist = doc.createElement(QStringLiteral("plist"));
    plist.setAttribute(QStringLiteral("version"), QStringLiteral("1.0"));
    doc.appendChild(plist);

    QDomElement dict = doc.createElement(QStringLiteral("dict"));
    plist.appendChild(dict);

    QDomElement el = doc.createElement(QStringLiteral("key"));
    el.appendChild(doc.createTextNode(QStringLiteral("AdiumSetVersion")));
    dict.appendChild(el);

    el = doc.createElement(QStringLiteral("integer"));
    el.appendChild(doc.createTextNode(QStringLiteral("1")));
    dict.appendChild(el);

    el = doc.createElement(QStringLiteral("key"));
    el.appendChild(doc.createTextNode(QStringLiteral("Emoticons")));
    dict.appendChild(el);

    dict.appendChild(doc.createElement(QStringLiteral("dict")));

    QTextStream emoStream(&fp);
    emoStream.setCodec("UTF-8");
    emoStream << doc.toString(4);
    fp.close();
}

#include "adium_emoticons.moc"

