/*
    SPDX-FileCopyrightText: 2008 Carlo Segato <brandon.ml@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "adium_emoticons.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include "kemoticons_plugin_adium_debug.h"

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(AdiumEmoticons, "emoticonstheme_adium.json")

AdiumEmoticons::AdiumEmoticons(QObject *parent, const QVariantList &args)
    : KEmoticonsProvider(parent)
{
    Q_UNUSED(args)
}

bool AdiumEmoticons::removeEmoticon(const QString &emo)
{
    const QString emoticon = QFileInfo(emoticonsMap().key(emo.split(QLatin1Char(' ')))).fileName();
    QDomElement fce = m_themeXml.firstChildElement(QStringLiteral("plist")).firstChildElement(QStringLiteral("dict")).firstChildElement(QStringLiteral("dict"));

    if (fce.isNull()) {
        return false;
    }

    const QDomNodeList nl = fce.childNodes();
    for (int i = 0; i < nl.length(); i++) {
        QDomElement de = nl.item(i).toElement();
        if (!de.isNull() && de.tagName() == QLatin1String("key") && (de.text() == emoticon)) {
            QDomElement dict = de.nextSiblingElement();
            if (!dict.isNull() && dict.tagName() == QLatin1String("dict")) {
                fce.removeChild(dict);
            }

            fce.removeChild(de);
            removeMapItem(emoticonsMap().key(emo.split(QLatin1Char(' '))));
            removeIndexItem(emoticon, emo.split(QLatin1Char(' ')));
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
            qCWarning(KEMOTICONS_PLUGIN_ADIUM) << "There was a problem copying the emoticon";
            return false;
        }
    }

    const QStringList splitted = text.split(QLatin1Char(' '));
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
    QFile fp(themePath() + QLatin1Char('/') + fileName());

    if (!fp.exists()) {
        qCWarning(KEMOTICONS_PLUGIN_ADIUM) << fp.fileName() << "doesn't exist!";
        return;
    }

    if (!fp.open(QIODevice::WriteOnly)) {
        qCWarning(KEMOTICONS_PLUGIN_ADIUM) << fp.fileName() << "can't open WriteOnly!";
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
        qCWarning(KEMOTICONS_PLUGIN_ADIUM) << path << "doesn't exist!";
        return false;
    }

    setThemePath(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(KEMOTICONS_PLUGIN_ADIUM) << file.fileName() << "can't be open ReadOnly!";
        return false;
    }

    QString error;
    int eli, eco;
    if (!m_themeXml.setContent(&file, &error, &eli, &eco)) {
        qCWarning(KEMOTICONS_PLUGIN_ADIUM) << file.fileName() << "can't copy to xml!";
        qCWarning(KEMOTICONS_PLUGIN_ADIUM) << error << "line:" << eli << "column:" << eco;
        file.close();
        return false;
    }

    file.close();

    QDomElement fce = m_themeXml.firstChildElement(QStringLiteral("plist")).firstChildElement(QStringLiteral("dict")).firstChildElement(QStringLiteral("dict"));

    if (fce.isNull()) {
        return false;
    }

    const QDomNodeList nl = fce.childNodes();

    clearEmoticonsMap();
    QString name;
    for (int i = 0; i < nl.length(); i++) {
        QDomElement de = nl.item(i).toElement();

        if (!de.isNull() && de.tagName() == QLatin1String("key")) {
            name = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("emoticons/") + themeName() + QLatin1Char('/') + de.text());
            continue;
        } else if (!de.isNull() && de.tagName() == QLatin1String("dict")) {
            QDomElement arr = de.firstChildElement(QStringLiteral("array"));
            const QDomNodeList snl = arr.childNodes();
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
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/emoticons/") + themeName();
    QDir().mkpath(path);

    QFile fp(path + QLatin1Char('/') + QStringLiteral("Emoticons.plist"));

    if (!fp.open(QIODevice::WriteOnly)) {
        qCWarning(KEMOTICONS_PLUGIN_ADIUM) << fp.fileName() << "can't open WriteOnly!";
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

