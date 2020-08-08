/*
    SPDX-FileCopyrightText: 2017 David Faure <faure@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

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
