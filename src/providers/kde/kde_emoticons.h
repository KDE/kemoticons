/*
    SPDX-FileCopyrightText: 2008 Carlo Segato <brandon.ml@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KDE_EMOTICONS_H
#define KDE_EMOTICONS_H

#include <kemoticonsprovider.h>

#include <QDomDocument>

class KdeEmoticons : public KEmoticonsProvider
{
    Q_OBJECT
public:
    KdeEmoticons(QObject *parent, const QVariantList &args);

    bool loadTheme(const QString &path) override;

    bool removeEmoticon(const QString &emo) override;
    bool addEmoticon(const QString &emo, const QString &text, AddEmoticonOption option = DoNotCopy) override;
    void saveTheme() override;
    void newTheme() override;

private:
    QDomDocument m_themeXml;
};

#endif /* KDE_EMOTICONS_H */

