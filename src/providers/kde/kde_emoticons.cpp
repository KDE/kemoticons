/*
    SPDX-FileCopyrightText: 2008 Carlo Segato <brandon.ml@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kde_emoticons.h"

#include <QFile>
#include <QDir>
#include "kemoticons_plugin_kde_debug.h"
#include <QFileInfo>
#include <QStandardPaths>
#include <QImageReader>

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(KdeEmoticons, "emoticonstheme_kde.json")

KdeEmoticons::KdeEmoticons(QObject *parent, const QVariantList &args)
    : KEmoticonsProvider(parent)
{
    Q_UNUSED(args);
}

bool KdeEmoticons::removeEmoticon(const QString &emo)
{
    const QString emoticon = QFileInfo(emoticonsMap().key(emo.split(QLatin1Char(' ')))).fileName();
    QDomElement fce = m_themeXml.firstChildElement(QStringLiteral("messaging-emoticon-map"));

    if (fce.isNull()) {
        return false;
    }

    QDomNodeList nl = fce.childNodes();
    for (int i = 0; i < nl.length(); i++) {
        QDomElement de = nl.item(i).toElement();
        if (!de.isNull() && de.tagName() == QLatin1String("emoticon") && (de.attribute(QStringLiteral("file")) == emoticon || de.attribute(QStringLiteral("file")) == QFileInfo(emoticon).baseName())) {
            fce.removeChild(de);
            removeMapItem(emoticonsMap().key(emo.split(QLatin1Char(' '))));
            removeIndexItem(emoticon, emo.split(QLatin1Char(' ')));
            return true;
        }
    }
    return false;
}

bool KdeEmoticons::addEmoticon(const QString &emo, const QString &text, AddEmoticonOption option)
{
    if (option == Copy) {
        bool result = copyEmoticon(emo);
        if (!result) {
            qCWarning(KEMOTICONS_PLUGIN_KDE) << "There was a problem copying the emoticon";
            return false;
        }
    }

    const QStringList splitted = text.split(QLatin1Char(' '));
    QDomElement fce = m_themeXml.firstChildElement(QStringLiteral("messaging-emoticon-map"));

    if (fce.isNull()) {
        return false;
    }

    QDomElement emoticon = m_themeXml.createElement(QStringLiteral("emoticon"));
    emoticon.setAttribute(QStringLiteral("file"), QFileInfo(emo).fileName());
    fce.appendChild(emoticon);
    QStringList::const_iterator constIterator;
    for (constIterator = splitted.begin(); constIterator != splitted.end(); ++constIterator) {
        QDomElement emoText = m_themeXml.createElement(QStringLiteral("string"));
        QDomText txt = m_themeXml.createTextNode((*constIterator).trimmed());
        emoText.appendChild(txt);
        emoticon.appendChild(emoText);
    }

    addIndexItem(emo, splitted);
    addMapItem(emo, splitted);
    return true;
}

void KdeEmoticons::saveTheme()
{
    QFile fp(themePath() + QLatin1Char('/') + fileName());

    if (!fp.exists()) {
        qCWarning(KEMOTICONS_PLUGIN_KDE) << fp.fileName() << "doesn't exist!";
        return;
    }

    if (!fp.open(QIODevice::WriteOnly)) {
        qCWarning(KEMOTICONS_PLUGIN_KDE) << fp.fileName() << "can't open WriteOnly!";
        return;
    }

    QTextStream emoStream(&fp);
    emoStream.setCodec("UTF-8");
    emoStream << m_themeXml.toString(4);
    fp.close();
}

bool KdeEmoticons::loadTheme(const QString &path)
{
    QFile file(path);

    if (!file.exists()) {
        qCWarning(KEMOTICONS_PLUGIN_KDE) << path << "doesn't exist!";
        return false;
    }

    setThemePath(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(KEMOTICONS_PLUGIN_KDE) << file.fileName() << "can't be open ReadOnly!";
        return false;
    }

    QString error;
    int eli, eco;
    if (!m_themeXml.setContent(&file, &error, &eli, &eco)) {
        qCWarning(KEMOTICONS_PLUGIN_KDE) << file.fileName() << "can't copy to xml!";
        qCWarning(KEMOTICONS_PLUGIN_KDE) << error << "line:" << eli << "column:" << eco;
        file.close();
        return false;
    }

    file.close();

    QDomElement fce = m_themeXml.firstChildElement(QStringLiteral("messaging-emoticon-map"));

    if (fce.isNull()) {
        return false;
    }

    const QDomNodeList nl = fce.childNodes();

    clearEmoticonsMap();

    for (int i = 0; i < nl.length(); i++) {
        QDomElement de = nl.item(i).toElement();

        if (!de.isNull() && de.tagName() == QLatin1String("emoticon")) {
            QDomNodeList snl = de.childNodes();
            QStringList sl;

            for (int k = 0; k < snl.length(); k++) {
                QDomElement sde = snl.item(k).toElement();

                if (!sde.isNull() && sde.tagName() == QLatin1String("string")) {
                    sl << sde.text();
                }
            }

            QString emo = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("emoticons/") + themeName() + QLatin1Char('/') + de.attribute(QLatin1String("file")));

            if (emo.isEmpty()) {
                QList<QByteArray> ext = QImageReader::supportedImageFormats();

                for (int j = 0; j < ext.size(); ++j) {
                    emo = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("emoticons/") +
                                                 themeName() + QLatin1Char('/') + de.attribute(QStringLiteral("file"))
                                                 + QLatin1Char('.') + QString::fromLatin1(ext.at(j)));
                    if (!emo.isEmpty()) {
                        break;
                    }
                }

                if (emo.isEmpty()) {
                    continue;
                }
            }

            addIndexItem(emo, sl);
            addMapItem(emo, sl);
        }
    }

    return true;
}

void KdeEmoticons::newTheme()
{
    const QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/emoticons/") + themeName();
    QDir().mkpath(path);

    QFile fp(path + QLatin1Char('/') + QStringLiteral("emoticons.xml"));

    if (!fp.open(QIODevice::WriteOnly)) {
        qCWarning(KEMOTICONS_PLUGIN_KDE) << fp.fileName() << "can't open WriteOnly!";
        return;
    }

    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction(QStringLiteral("xml"), QStringLiteral("version=\"1.0\"")));
    doc.appendChild(doc.createElement(QStringLiteral("messaging-emoticon-map")));

    QTextStream emoStream(&fp);
    emoStream.setCodec("UTF-8");
    emoStream << doc.toString(4);
    fp.close();
}

#include "kde_emoticons.moc"

