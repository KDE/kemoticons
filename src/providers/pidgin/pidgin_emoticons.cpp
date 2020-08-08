/*
    SPDX-FileCopyrightText: 2008 Carlo Segato <brandon.ml@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "pidgin_emoticons.h"
#include "kemoticonsprovider.h"

#include <QFile>
#include <QDir>
#include "kemoticons_plugin_pidgin_debug.h"
#include <QFileInfo>
#include <QStandardPaths>

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(PidginEmoticons, "emoticonstheme_pidgin.json")

PidginEmoticons::PidginEmoticons(QObject *parent, const QVariantList &args)
    : KEmoticonsProvider(parent)
{
    Q_UNUSED(args);
}

bool PidginEmoticons::removeEmoticon(const QString &emo)
{
    QString emoticon = QFileInfo(emoticonsMap().key(emo.split(QLatin1Char(' ')))).fileName();

    bool start = false;
    for (int i = 0; i < m_text.size(); ++i) {
        QString line = m_text.at(i);

        if (line.startsWith(QLatin1Char('#')) || line.isEmpty()) {
            continue;
        }

        QRegExp re(QStringLiteral("^\\[(.*)\\]$"));
        int pos = re.indexIn(line.trimmed());
        if (pos > -1) {
            if (!re.cap(1).compare(QStringLiteral("default"), Qt::CaseInsensitive)) {
                start = true;
            } else {
                start = false;
            }
            continue;
        }

        if (!start) {
            continue;
        }

        const QStringList splitted = line.split(QLatin1Char(' '));
        QString emoName;

        if (splitted.at(0) == QLatin1Char('!')) {
            emoName = splitted.at(1);
        } else {
            emoName = splitted.at(0);
        }

        if (emoName == emoticon) {
            m_text.removeAt(i);
            removeIndexItem(emoticon, emo.split(QLatin1Char(' ')));
            return true;
        }
    }

    return false;
}

bool PidginEmoticons::addEmoticon(const QString &emo, const QString &text, AddEmoticonOption option)
{
    if (option == Copy) {
        bool result = copyEmoticon(emo);
        if (!result) {
            qCWarning(KEMOTICONS_PLUGIN_PIDGIN) << "There was a problem copying the emoticon";
            return false;
        }
    }

    const QStringList splitted = text.split(QLatin1Char(' '));
    int i = m_text.indexOf(QRegExp(QStringLiteral("^\\[default\\]$"), Qt::CaseInsensitive));

    if (i == -1) {
        return false;
    }

    const QString emoticon = QStringLiteral("%1 %2").arg(QFileInfo(emo).fileName(),
                                            text);
    m_text.insert(i + 1, emoticon);

    addIndexItem(emo, splitted);
    addMapItem(emo, splitted);
    return true;
}

void PidginEmoticons::saveTheme()
{
    QFile fp(themePath() + QLatin1Char('/') + fileName());

    if (!fp.exists()) {
        qCWarning(KEMOTICONS_PLUGIN_PIDGIN) << fp.fileName() << "doesn't exist!";
        return;
    }

    if (!fp.open(QIODevice::WriteOnly)) {
        qCWarning(KEMOTICONS_PLUGIN_PIDGIN) << fp.fileName() << "can't open WriteOnly!";
        return;
    }

    QTextStream emoStream(&fp);

    if (m_text.indexOf(QRegExp(QStringLiteral("^Icon=.*"), Qt::CaseInsensitive)) == -1) {
        int i = m_text.indexOf(QRegExp(QStringLiteral("^Description=.*"), Qt::CaseInsensitive));
        QString file = QFileInfo(emoticonsMap().keys().value(0)).fileName();
        m_text.insert(i + 1, QStringLiteral("Icon=") + file);
    }

    emoStream << m_text.join(QLatin1Char('\n'));
    fp.close();
}

bool PidginEmoticons::loadTheme(const QString &path)
{
    QFile file(path);

    if (!file.exists()) {
        qCWarning(KEMOTICONS_PLUGIN_PIDGIN) << path << "doesn't exist!";
        return false;
    }

    setThemePath(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(KEMOTICONS_PLUGIN_PIDGIN) << file.fileName() << "can't be open ReadOnly!";
        return false;
    }

    QTextStream str(&file);
    bool start = false;
    m_text.clear();
    while (!str.atEnd()) {
        QString line = str.readLine();
        m_text << line;

        if (line.startsWith(QLatin1Char('#')) || line.isEmpty()) {
            continue;
        }

        QRegExp re(QStringLiteral("^\\[(.*)\\]$"));
        int pos = re.indexIn(line.trimmed());
        if (pos > -1) {
            if (!re.cap(1).compare(QStringLiteral("default"), Qt::CaseInsensitive)) {
                start = true;
            } else {
                start = false;
            }
            continue;
        }

        if (!start) {
            continue;
        }

        QStringList splitted = line.split(QRegExp(QStringLiteral("\\s+")));
        QString emo;
        int i = 1;
        if (splitted.at(0) == QLatin1Char('!')) {
            i = 2;
            emo = splitted.at(1);
        } else {
            emo = splitted.at(0);
        }
        emo = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("emoticons/") + themeName() + QLatin1Char('/') + emo);

        QStringList sl;
        for (; i < splitted.size(); ++i) {
            if (!splitted.at(i).isEmpty() && splitted.at(i) != QLatin1String(" ")) {
                sl << splitted.at(i);
            }
        }

        addIndexItem(emo, sl);
        addMapItem(emo, sl);
    }

    file.close();

    return true;
}

void PidginEmoticons::newTheme()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/emoticons/") + themeName();
    QDir().mkpath(path);

    QFile fp(path + QLatin1Char('/') + QStringLiteral("theme"));

    if (!fp.open(QIODevice::WriteOnly)) {
        qCWarning(KEMOTICONS_PLUGIN_PIDGIN) << fp.fileName() << "can't open WriteOnly!";
        return;
    }

    QTextStream out(&fp);
    out.setCodec("UTF-8");

    out << QStringLiteral("Name=") + themeName() << "\n";
    out << QStringLiteral("Description=") + themeName() << "\n";
    out << "Author=\n";
    out << "\n";
    out << "[default]\n";

    fp.close();
}

#include "pidgin_emoticons.moc"

