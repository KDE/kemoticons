/**********************************************************************************
 *   Copyright (C) 2017 David Faure <faure@kde.org>                               *
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

#ifndef KEMOTICONSGLOBAL_P_H
#define KEMOTICONSGLOBAL_P_H

#include <QObject>
#include "kemoticonstheme.h"

class KEmoticonsGlobal : public QObject
{
    Q_OBJECT
public:
    KEmoticonsGlobal();

    QString m_themeName;
    KEmoticonsTheme::ParseMode m_parseMode;

    void setThemeName(const QString &name);
    void setParseMode(KEmoticonsTheme::ParseMode mode);

private Q_SLOTS:
    void slotEmoticonsThemeChanged(const QString &name);
    void slotEmoticonsParseModeChanged(int mode);

};

#endif // KEMOTICONSGLOBAL_P_H
