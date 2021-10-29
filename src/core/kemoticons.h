/*
    SPDX-FileCopyrightText: 2008 Carlo Segato <brandon.ml@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KEMOTICONS_H
#define KEMOTICONS_H

#include "kemoticons_export.h"
#include "kemoticonstheme.h"

#include <QObject>

#include <KServiceTypeTrader>

class KEmoticonsPrivate;

/**
 * This class can be used to retrieve, install, create emoticons theme.
 * For example, if you want to get the current emoticon theme
 * @code
 * KEmoticons ke;
 * KEmoticonsTheme et = ke.theme();
 * //do whatever you want with the theme
 * @endcode
 * It can also be used to set the emoticon theme and the parse mode in the config file
 * @author Carlo Segato (brandon.ml@gmail.com)
 */

class KEMOTICONS_EXPORT KEmoticons : public QObject
{
    Q_OBJECT
public:

    /**
     * Default constructor
     */
    KEmoticons();

    /**
     * Destructor
     */
    ~KEmoticons() override;

    /**
     * Retrieves the current emoticon theme
     * @return the current KEmoticonsTheme
     */
    KEmoticonsTheme theme() const;

    /**
    * Retrieves the emoticon theme with name @p name
    * @param name name of the theme
    * @return the KEmoticonsTheme with name @p name
    */
    KEmoticonsTheme theme(const QString &name) const;

    /**
     * Retrieves the current emoticon theme name
     */
    static QString currentThemeName();

    /**
     * Returns a list of installed emoticon themes
     */
    static QStringList themeList();

    /**
     * Sets @p theme as the current emoticon theme
     * @param theme a reference to a KEmoticonsTheme object
     */
    static void setTheme(const KEmoticonsTheme &theme);

    /**
     * Sets @p theme as the current emoticon theme
     * @param theme the name of a theme
     */
    static void setTheme(const QString &theme);

    /**
     * Creates a new emoticon theme
     * @code
     * KEmoticonsTheme theme;
     * KService::List srv = KServiceTypeTrader::self()->query("KEmoticons");
     * for (int i = 0; i < srv.size(); ++i) {
     *     // we want to create a kde emoticons theme
     *     if (srv.at(i)->property("X-KDE-EmoticonsFileName").toString() == "emoticons.xml") {
     *         theme = KEmoticons().newTheme("test", srv.at(i));
     *     }
     * }
     * @endcode
     * @param name the name of the new emoticon theme
     * @param service the kind of emoticon theme to create
     */
    KEmoticonsTheme newTheme(const QString &name, const KService::Ptr &service);

    /**
     * Installs all emoticon themes inside the archive @p archiveName
     * @param archiveName path to the archive
     * @return a list of installed themes
     */
    QStringList installTheme(const QString &archiveName);

    /**
     * Sets the parse mode to @p mode
     */
    static void setParseMode(KEmoticonsTheme::ParseMode mode);

    /**
     * Returns the current parse mode
     */
    static KEmoticonsTheme::ParseMode parseMode();

    /**
     * If a preferred size is set, all parsed emoticons will be
     * returned with the @p size
     *
     * @param size The desired QSize of parsed emoticons
     * @since 5.23
     */
    void setPreferredEmoticonSize(const QSize &size);

    /**
     * Returns size in which parsed emoticons will be returned.
     *
     * If the QSize returned is not valid (isValid() == false),
     * then the default will be used, that is the actual file size.
     *
     * @since 5.23
     */
    QSize preferredEmoticonSize() const;

private:
    /**
     * Private implementation class
     */
    const QScopedPointer<KEmoticonsPrivate> d;
};

#endif /* KEMOTICONS_H */

