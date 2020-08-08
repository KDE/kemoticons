/*
    SPDX-FileCopyrightText: 2014 Daniel Vrátil <dvratil@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KEMOTICONSINTEGRATIONPLUGIN_H
#define KEMOTICONSINTEGRATIONPLUGIN_H

#include <QObject>

#include "ktexttohtml.h"

class KEmoticonsIntegrationPlugin : public QObject
{
    Q_PLUGIN_METADATA(IID "org.kde.KEmoticonsIntegrationPlugin")
    Q_OBJECT

public:
    KEmoticonsIntegrationPlugin();

private:
    KTextToHTMLEmoticons mTextToHTMLEmoticons;
};

#endif // KEMOTICONSINTEGRATIONPLUGIN_H
