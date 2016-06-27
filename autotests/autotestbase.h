/*
    Test base class

    Copyright (c) 2004      by Richard Smith          <kde@metafoo.co.uk>
    Copyright (c) 2005      by Duncan Mac-Vicar       <duncan@kde.org>
    Copyright (c) 2014      by Alex Merry             <alex.merry@kde.org>

    Kopete    (c) 2002-2005 by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#ifndef AUTOTESTBASE_H
#define AUTOTESTBASE_H

#include <QDebug>
#include <QDir>
#include <QObject>

static bool copyTheme(const QString &dir, const QDir &baseThemeDir, const QString &themeName)
{
    QDir sourceThemeDir(dir);
    if (!sourceThemeDir.exists()) {
        return false;
    }
    QDir themeDir(baseThemeDir.absolutePath() + '/' + themeName);
    themeDir.removeRecursively();
    themeDir.mkpath(QStringLiteral("."));

    foreach (const QString &fileName, sourceThemeDir.entryList(QDir::Files)) {
        if (!QFile::copy(sourceThemeDir.filePath(fileName),
                         themeDir.filePath(fileName))) {
            qWarning() << "couldn't copy" << dir << "/" << fileName;
            return false;
        }
    }
    return true;
}

#endif // AUTOTESTBASE_H
