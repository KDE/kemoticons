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

#include "xmpp_emoticons.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QStandardPaths>
#include <QImageReader>
#include <QMimeDatabase>

#include <kpluginfactory.h>

K_PLUGIN_FACTORY_WITH_JSON(XmppEmoticonsFactory, "emoticonstheme_xmpp.json", registerPlugin<XmppEmoticons>();)

XmppEmoticons::XmppEmoticons(QObject *parent, const QVariantList &args)
    : KEmoticonsProvider(parent)
{
    Q_UNUSED(args);
}

bool XmppEmoticons::removeEmoticon(const QString &emo)
{
    const QString emoticon = QFileInfo(emoticonsMap().key(emo.split(QLatin1Char(' ')))).fileName();
    QDomElement fce = m_themeXml.firstChildElement(QStringLiteral("icondef"));

    if (fce.isNull()) {
        return false;
    }

    const QDomNodeList nl = fce.childNodes();
    for (int i = 0; i < nl.length(); i++) {
        QDomElement de = nl.item(i).toElement();
        if (!de.isNull() && de.tagName() == QLatin1String("icon")) {
            QDomNodeList snl = de.childNodes();

            for (int k = 0; k < snl.length(); k++) {
                QDomElement sde = snl.item(k).toElement();

                if (!sde.isNull() && sde.tagName() == QLatin1String("object") && sde.text() == emoticon) {
                    fce.removeChild(de);
                    removeMapItem(emoticonsMap().key(emo.split(QLatin1Char(' '))));
                    removeIndexItem(emoticon, emo.split(QLatin1Char(' ')));
                    return true;
                }
            }
        }
    }
    return false;
}

bool XmppEmoticons::addEmoticon(const QString &emo, const QString &text, AddEmoticonOption option)
{
    if (option == Copy) {
        bool result = copyEmoticon(emo);
        if (!result) {
            qWarning() << "There was a problem copying the emoticon";
            return false;
        }
    }

    const QStringList splitted = text.split(QLatin1Char(' '));
    QDomElement fce = m_themeXml.firstChildElement(QStringLiteral("icondef"));

    if (fce.isNull()) {
        return false;
    }

    QDomElement emoticon = m_themeXml.createElement(QStringLiteral("icon"));
    fce.appendChild(emoticon);
    QStringList::const_iterator constIterator;

    for (constIterator = splitted.begin(); constIterator != splitted.end(); ++constIterator) {
        QDomElement emotext = m_themeXml.createElement(QStringLiteral("text"));
        const QDomText txt = m_themeXml.createTextNode((*constIterator).trimmed());
        emotext.appendChild(txt);
        emoticon.appendChild(emotext);
    }

    QDomElement emoElement = m_themeXml.createElement(QStringLiteral("object"));
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(emo, QMimeDatabase::MatchExtension);
    emoElement.setAttribute(QStringLiteral("mime"), mime.name());
    const QDomText txt = m_themeXml.createTextNode(QFileInfo(emo).fileName());

    emoElement.appendChild(txt);
    emoticon.appendChild(emoElement);

    addIndexItem(emo, splitted);
    addMapItem(emo, splitted);
    return true;
}

void XmppEmoticons::saveTheme()
{
    QFile fp(themePath() + QLatin1Char('/') + fileName());

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

bool XmppEmoticons::loadTheme(const QString &path)
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

    QDomElement fce = m_themeXml.firstChildElement(QStringLiteral("icondef"));

    if (fce.isNull()) {
        return false;
    }

    const QDomNodeList nl = fce.childNodes();

    clearEmoticonsMap();

    for (int i = 0; i < nl.length(); i++) {
        QDomElement de = nl.item(i).toElement();

        if (!de.isNull() && de.tagName() == QLatin1String("icon")) {
            QDomNodeList snl = de.childNodes();
            QStringList sl;
            QString emo;
            QStringList mime;
            mime << QStringLiteral("image/png") << QStringLiteral("image/gif") << QStringLiteral("image/bmp") << QStringLiteral("image/jpeg");

            for (int k = 0; k < snl.length(); k++) {
                QDomElement sde = snl.item(k).toElement();

                if (!sde.isNull() && sde.tagName() == QLatin1String("text")) {
                    sl << sde.text();
                } else if (!sde.isNull() && sde.tagName() == QLatin1String("object") && mime.contains(sde.attribute(QStringLiteral("mime")))) {
                    emo = sde.text();
                }
            }

            emo = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("emoticons/") + themeName() + QLatin1Char('/') + emo);

            if (emo.isEmpty()) {
                continue;
            }

            addIndexItem(emo, sl);
            addMapItem(emo, sl);
        }
    }

    return true;
}

void XmppEmoticons::newTheme()
{
    const QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/emoticons/") + themeName();
    QDir().mkpath(path);

    QFile fp(path + QLatin1Char('/') + QStringLiteral("icondef.xml"));

    if (!fp.open(QIODevice::WriteOnly)) {
        qWarning() << fp.fileName() << "can't open WriteOnly!";
        return;
    }

    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction(QStringLiteral("xml"), QStringLiteral("version=\"1.0\" encoding=\"UTF-8\"")));
    doc.appendChild(doc.createElement(QStringLiteral("icondef")));

    QTextStream emoStream(&fp);
    emoStream.setCodec("UTF-8");
    emoStream << doc.toString(4);
    fp.close();
}

#include "xmpp_emoticons.moc"

