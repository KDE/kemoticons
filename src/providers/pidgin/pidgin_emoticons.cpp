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

#include "pidgin_emoticons.h"
#include "kemoticonsprovider.h"

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>

#include <kpluginfactory.h>

K_PLUGIN_FACTORY_WITH_JSON(PidginEmoticonsFactory, "emoticonstheme_pidgin.json", registerPlugin<PidginEmoticons>();)

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

        QStringList splitted = line.split(QLatin1Char(' '));
        QString emoName;

        if (splitted.at(0) == QLatin1String("!")) {
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
            qWarning() << "There was a problem copying the emoticon";
            return false;
        }
    }

    const QStringList splitted = text.split(QLatin1Char(' '));
    int i = m_text.indexOf(QRegExp(QStringLiteral("^\\[default\\]$"), Qt::CaseInsensitive));

    if (i == -1) {
        return false;
    }

    QString emoticon = QStringLiteral("%1 %2").arg(QFileInfo(emo).fileName(),
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
        qWarning() << fp.fileName() << "doesn't exist!";
        return;
    }

    if (!fp.open(QIODevice::WriteOnly)) {
        qWarning() << fp.fileName() << "can't open WriteOnly!";
        return;
    }

    QTextStream emoStream(&fp);

    if (m_text.indexOf(QRegExp(QStringLiteral("^Icon=.*"), Qt::CaseInsensitive)) == -1) {
        int i = m_text.indexOf(QRegExp(QStringLiteral("^Description=.*"), Qt::CaseInsensitive));
        QString file = QFileInfo(emoticonsMap().keys().value(0)).fileName();
        m_text.insert(i + 1, QStringLiteral("Icon=") + file);
    }

    emoStream << m_text.join(QStringLiteral("\n"));
    fp.close();
}

bool PidginEmoticons::loadTheme(const QString &path)
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
        if (splitted.at(0) == QLatin1String("!")) {
            i = 2;
            emo = splitted.at(1);
        } else {
            emo = splitted.at(0);
        }
        emo = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("emoticons/") + themeName() + QLatin1Char('/') + emo);

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
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/emoticons/") + themeName();
    QDir().mkpath(path);

    QFile fp(path + QLatin1Char('/') + QStringLiteral("theme"));

    if (!fp.open(QIODevice::WriteOnly)) {
        qWarning() << fp.fileName() << "can't open WriteOnly!";
        return;
    }

    QTextStream out(&fp);
    out.setCodec("UTF-8");

    out << QStringLiteral("Name=") + themeName() << endl;
    out << QStringLiteral("Description=") + themeName() << endl;
    out << "Author=" << endl;
    out << endl;
    out << "[default]" << endl;

    fp.close();
}

#include "pidgin_emoticons.moc"

