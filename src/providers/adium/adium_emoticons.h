/*
    SPDX-FileCopyrightText: 2008 Carlo Segato <brandon.ml@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ADIUM_EMOTICONS_H
#define ADIUM_EMOTICONS_H

#include <kemoticonsprovider.h>

#include <QDomDocument>

class AdiumEmoticons : public KEmoticonsProvider
{
    Q_OBJECT
public:
    AdiumEmoticons(QObject *parent, const QVariantList &args);

    bool loadTheme(const QString &path) override;

    bool removeEmoticon(const QString &emo) override;
    bool addEmoticon(const QString &emo, const QString &text, AddEmoticonOption option = DoNotCopy) override;
    void saveTheme() override;

    void newTheme() override;

private:
    QDomDocument m_themeXml;
};

#endif /* ADIUM_EMOTICONS_H */

