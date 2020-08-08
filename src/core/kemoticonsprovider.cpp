/*
    SPDX-FileCopyrightText: 2008 Carlo Segato <brandon.ml@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kemoticonsprovider.h"
#include "kemoticons.h"

#include <QFileInfo>
#include <QDir>
#include <QPixmap>
#include "kemoticons_core_debug.h"

class KEmoticonsProviderPrivate
{
public:
    KEmoticonsProviderPrivate();
    QString m_themeName;
    QString m_fileName;
    QString m_themePath;
    QHash<QString, QStringList> m_emoticonsMap;
    QHash<QChar, QList<KEmoticonsProvider::Emoticon> > m_emoticonsIndex;
    QSize m_preferredSize;
};

KEmoticonsProviderPrivate::KEmoticonsProviderPrivate()
{
}

KEmoticonsProvider::KEmoticonsProvider(QObject *parent)
    : QObject(parent), d(new KEmoticonsProviderPrivate)
{
}

KEmoticonsProvider::~KEmoticonsProvider()
{
}

#if KEMOTICONS_BUILD_DEPRECATED_SINCE(5, 0)
void KEmoticonsProvider::save()
{
    saveTheme();
}
#endif

QString KEmoticonsProvider::themeName() const
{
    return d->m_themeName;
}

void KEmoticonsProvider::setThemeName(const QString &name)
{
    d->m_themeName = name;
}

QString KEmoticonsProvider::themePath() const
{
    return d->m_themePath;
}

QString KEmoticonsProvider::fileName() const
{
    return d->m_fileName;
}

void KEmoticonsProvider::clearEmoticonsMap()
{
    d->m_emoticonsMap.clear();
}

#if KEMOTICONS_BUILD_DEPRECATED_SINCE(5, 0)
void KEmoticonsProvider::addEmoticonsMap(QString key, QStringList value)
{
    addMapItem(key, value);
}
#endif

void KEmoticonsProvider::addMapItem(QString key, QStringList value)
{
    if (!value.isEmpty()) {
        d->m_emoticonsMap.insert(key, value);
    }
}

#if KEMOTICONS_BUILD_DEPRECATED_SINCE(5, 0)
void KEmoticonsProvider::removeEmoticonsMap(QString key)
{
    removeMapItem(key);
}
#endif

void KEmoticonsProvider::removeMapItem(QString key)
{
    d->m_emoticonsMap.remove(key);
}

QHash<QString, QStringList> KEmoticonsProvider::emoticonsMap() const
{
    return d->m_emoticonsMap;
}

QHash<QChar, QList<KEmoticonsProvider::Emoticon> > KEmoticonsProvider::emoticonsIndex() const
{
    return d->m_emoticonsIndex;
}

#if KEMOTICONS_BUILD_DEPRECATED_SINCE(5, 0)
void KEmoticonsProvider::createNew()
{
    newTheme();
}
#endif

void KEmoticonsProvider::setThemePath(const QString &path)
{
    QFileInfo info(path);
    d->m_fileName = info.fileName();
    d->m_themeName = info.dir().dirName();
    d->m_themePath = info.absolutePath();
}

bool KEmoticonsProvider::copyEmoticon(const QString &emo)
{
    QFile file(emo);
    QFileInfo info(file);
    QString newPath(d->m_themePath + QLatin1Char('/') + info.fileName());
    return file.copy(newPath);
}

#if KEMOTICONS_BUILD_DEPRECATED_SINCE(5, 0)
void KEmoticonsProvider::addEmoticonIndex(const QString &path, const QStringList &emoList)
{
    addIndexItem(path, emoList);
}
#endif

void KEmoticonsProvider::addIndexItem(const QString &path, const QStringList &emoList)
{
    for (const QString &s : emoList) {
        KEmoticonsProvider::Emoticon e;
        QPixmap p;

        QString escaped = s.toHtmlEscaped();
        e.picPath = path;
        p.load(path);

        const bool hasPreferredSize = d->m_preferredSize.isValid();
        const int preferredHeight = hasPreferredSize ? d->m_preferredSize.height() : p.height();
        const int preferredWidth = hasPreferredSize ? d->m_preferredSize.width() : p.width();

        e.picHTMLCode = QStringLiteral("<img align=\"center\" title=\"%1\" alt=\"%1\" src=\"file://%2\" width=\"%3\" height=\"%4\" />").arg(escaped, path, QString::number(preferredWidth), QString::number(preferredHeight));

        e.matchTextEscaped = escaped;
        e.matchText = s;

        if (!s.isEmpty() && !escaped.isEmpty()) {
            d->m_emoticonsIndex[escaped[0]].append(e);
            d->m_emoticonsIndex[s[0]].append(e);
        }
    }
}

#if KEMOTICONS_BUILD_DEPRECATED_SINCE(5, 0)
void KEmoticonsProvider::removeEmoticonIndex(const QString &path, const QStringList &emoList)
{
    removeIndexItem(path, emoList);
}
#endif

void KEmoticonsProvider::removeIndexItem(const QString &path, const QStringList &emoList)
{
    for (const QString &s : emoList) {
        QString escaped = s.toHtmlEscaped();

        if (s.isEmpty() || escaped.isEmpty()) {
            continue;
        }

        QList<Emoticon> ls = d->m_emoticonsIndex.value(escaped[0]);

        for (int i = 0; i < ls.size(); ++i) {
            if (ls.at(i).picPath == path) {
                ls.removeAt(i);
            }
        }

        ls = d->m_emoticonsIndex.value(s[0]);

        for (int i = 0; i < ls.size(); ++i) {
            if (ls.at(i).picPath == path) {
                ls.removeAt(i);
            }
        }
    }
}

void KEmoticonsProvider::setPreferredEmoticonSize(const QSize &size)
{
    d->m_preferredSize = size;
}

QSize KEmoticonsProvider::preferredEmoticonSize() const
{
    return d->m_preferredSize;
}
