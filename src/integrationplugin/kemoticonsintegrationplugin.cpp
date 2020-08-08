/*
    SPDX-FileCopyrightText: 2014 Daniel Vrátil <dvratil@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kemoticonsintegrationplugin.h"

#include <QVariant>

KEmoticonsIntegrationPlugin::KEmoticonsIntegrationPlugin()
    : QObject()
{
    setProperty(KTEXTTOHTMLEMOTICONS_PROPERTY, QVariant::fromValue<KTextToHTMLEmoticonsInterface *>(&mTextToHTMLEmoticons));
}

