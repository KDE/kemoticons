/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2014 Daniel Vrátil <dvratil@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KTEXTTOHTML_INTEGRATION_H
#define KTEXTTOHTML_INTEGRATION_H


#include <KTextToHTMLEmoticonsInterface>

class KTextToHTMLEmoticons : public KTextToHTMLEmoticonsInterface
{
public:
    KTextToHTMLEmoticons();
    ~KTextToHTMLEmoticons() override {}

    virtual QString parseEmoticons(const QString &text,
                                   bool strictParse = false,
                                   const QStringList &exclude = QStringList()) override;
};

#endif
