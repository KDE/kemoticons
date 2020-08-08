/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2014 Daniel Vrátil <dvratil@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "ktexttohtml.h"

#include <kemoticons.h>
#include <kemoticonstheme.h>


Q_GLOBAL_STATIC(KEmoticons, sEmoticons)


KTextToHTMLEmoticons::KTextToHTMLEmoticons()
{
}

QString KTextToHTMLEmoticons::parseEmoticons(const QString &text,
                                             bool strictParse,
                                             const QStringList &exclude)
{
    KEmoticonsTheme::ParseMode mode = KEmoticonsTheme::DefaultParse;
    if (strictParse) {
        mode = KEmoticonsTheme::StrictParse;
    }
    return sEmoticons->theme().parseEmoticons(text, mode, exclude);
}
