/*
    SPDX-FileCopyrightText: 2008 Carlo Segato <brandon.ml@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef PIDGIN_EMOTICONS_H
#define PIDGIN_EMOTICONS_H

#include <kemoticonsprovider.h>

class PidginEmoticons : public KEmoticonsProvider
{
    Q_OBJECT
public:
    PidginEmoticons(QObject *parent, const QVariantList &args);

    bool loadTheme(const QString &path) override;

    bool removeEmoticon(const QString &emo) override;
    bool addEmoticon(const QString &emo, const QString &text, AddEmoticonOption option = DoNotCopy) override;
    void saveTheme() override;

    void newTheme() override;

private:
    QStringList m_text;
};

#endif /* PIDGIN_EMOTICONS_H */

