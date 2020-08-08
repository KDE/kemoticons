/*
    Test base class

    SPDX-FileCopyrightText: 2002-2005 The Kopete developers <kopete-devel@kde.org>
    SPDX-FileCopyrightText: 2004 Richard Smith <kde@metafoo.co.uk>
    SPDX-FileCopyrightText: 2005 Duncan Mac-Vicar <duncan@kde.org>
    SPDX-FileCopyrightText: 2014 Alex Merry <alex.merry@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef AUTOTESTBASE_H
#define AUTOTESTBASE_H

#include <QDebug>
#include <QDir>

static bool copyTheme(const QString &dir, const QDir &baseThemeDir, const QString &themeName)
{
    QDir sourceThemeDir(dir);
    if (!sourceThemeDir.exists()) {
        return false;
    }
    QDir themeDir(baseThemeDir.absolutePath() + QLatin1Char('/') + themeName);
    themeDir.removeRecursively();
    themeDir.mkpath(QStringLiteral("."));

    const auto files = sourceThemeDir.entryList(QDir::Files);
    for (const QString &fileName : files) {
        if (!QFile::copy(sourceThemeDir.filePath(fileName),
                         themeDir.filePath(fileName))) {
            qWarning() << "couldn't copy" << dir << "/" << fileName;
            return false;
        }
    }
    return true;
}

#endif // AUTOTESTBASE_H
