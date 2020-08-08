/*
    SPDX-FileCopyrightText: 2002-2008 The Kopete developers <kopete-devel@kde.org>
    SPDX-FileCopyrightText: 2008 Carlo Segato <brandon.ml@gmail.com>
    SPDX-FileCopyrightText: 2002-2003 Stefan Gehn <metz@gehn.net>
    SPDX-FileCopyrightText: 2005 Engin AYDOGAN <engin@bzzzt.biz>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KEMOTICONS_THEME_H
#define KEMOTICONS_THEME_H

#include "kemoticonsprovider.h"

#include <QSharedDataPointer>

class QString;

/**
 * This class contains the emoticons theme
 */
class KEMOTICONS_EXPORT KEmoticonsTheme
{
public:

    /**
     * The possible parse modes
     */
    enum ParseModeEnum {
        DefaultParse = 0x0,      /**< Use strict or relaxed according to the config  */
        StrictParse = 0x1,       /**< Strict parsing requires a space between each emoticon */
        RelaxedParse = 0x2,      /**< Parse mode where all possible emoticon matches are allowed */
        SkipHTML = 0x4           /**< Skip emoticons within HTML */
    };

    Q_DECLARE_FLAGS(ParseMode, ParseModeEnum)

    /**
     * TokenType, a token might be an image ( emoticon ) or text.
     */
    enum TokenType {
        Undefined, /**< Undefined, for completeness only */
        Image,     /**< Token contains a path to an image */
        Text       /**< Token contains text */
    };

    /**
     * A token consists of a QString text which is either a regular text
     * or a path to image depending on the type.
     * If type is Image the text refers to an image path.
     * If type is Text the text refers to a regular text.
     */
    struct Token {
        Token() : type(Undefined) {}
        /**
         * Creates a Token of type @p t, and text @p m
         */
        Token(TokenType t, const QString &m) : type(t), text(m) {}
        /**
         * Creates a Token of type @p t, text @p m, image path @p p and html code @p html
         */
        Token(TokenType t, const QString &m, const QString &p, const QString &html)
            : type(t), text(m), picPath(p), picHTMLCode(html) {}
        TokenType   type; /**< type */
        QString     text; /**< text */
        QString     picPath; /**< path to the image */
        QString     picHTMLCode; /**< \<img> html code */
    };

    /**
     * Default constructor, it creates a null emoticon theme
     * You should probably never use this, use KEmoticons::theme() instead
     */
    KEmoticonsTheme();

    /**
     * Copy constructor
     */
    KEmoticonsTheme(const KEmoticonsTheme &ket);

    /**
     * Another constructor where you set the KEmoticonsProvider @p p
     * You should probably never use this, use KEmoticons::theme() instead
     */
    KEmoticonsTheme(KEmoticonsProvider *p);

    /**
     * Destructor
     */
    ~KEmoticonsTheme();

    /**
     * Parses emoticons in text @p text with ParseMode @p mode and optionally excluding emoticons from @p exclude
     * @code
     * KEmoticonsTheme theme = KEmoticons().theme();
     * QString text = ":D hi :)";
     * QStringList exclude(":)");
     * QString parsed = theme.parseEmoticons(text, KEmoticonsTheme::DefaultParse, exclude);
     * // parsed will be "<img align="center" title=":D" alt=":D" src="/path/to/:D.png" width="24" height="24" /> hi :)"
     * @endcode
     * @param text the text to parse
     * @param mode how to parse the text
     * @param exclude a list of emoticons to exclude from the parsing
     * @return the text with emoticons replaced by html images
     * @note SkipHTML is forced when using this function
     */
    QString parseEmoticons(const QString &text, ParseMode mode = DefaultParse, const QStringList &exclude = QStringList()) const;

    /**
     * Tokenizes the message @p message with ParseMode @p mode
     * @code
     * KEmoticonsTheme theme = KEmoticons().theme();
     * QString text = "hi :)";
     * QList<Token> tokens = theme.tokenize(text, KEmoticonsTheme::DefaultParse);
     * // tokens[0].text = "hi "
     * // tokens[1].text = ":)"
     * // tokens[1].picPath = "/path/to/:).png"
     * // tokens[1].picHTMLCode = "<img align="center" title=":)" alt=":)" src="/path/to/:).png" width="24" height="24" />"
     * @endcode
     */
    QList<Token> tokenize(const QString &message, ParseMode mode = DefaultParse) const;

#if KEMOTICONS_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * Loads the emoticon theme inside the directory @p path
     * @param path path to the directory
     * @return @c true if the emoticon is successfully loaded
     *
     * @deprecated since 5.0, subclass KEmoticonsProvider instead
     */
    KEMOTICONS_DEPRECATED_VERSION(5, 0, "Subclass KEmoticonsProvider")
    bool loadTheme(const QString &path);
#endif

#if KEMOTICONS_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * Removes the emoticon @p emo. This doesn't delete the image file.
     * @code
     * KEmoticonsTheme theme = KEmoticons().theme();
     * theme.removeEmoticon(":)");
     * @endcode
     * @param emo the emoticon text to remove
     * @return @c true if the emoticon is successfully removed
     *
     * @deprecated since 5.0, subclass KEmoticonsProvider instead
     */
    KEMOTICONS_DEPRECATED_VERSION(5, 0, "Subclass KEmoticonsProvider")
    bool removeEmoticon(const QString &emo);
#endif

#if KEMOTICONS_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * Adds the emoticon @p emo with text @p text
     * @code
     * KEmoticonsTheme theme = KEmoticons().theme();
     * theme.addEmoticon("/path/to/smiley.png", ":) :-)");
     * @endcode
     * @param emo path to the emoticon image
     * @param text the emoticon text. If alternative texts are to be added,
     * use spaces to separate them.
     * @param copy whether or not to copy @p emo into the theme directory
     * @return @c true if the emoticon is successfully added
     *
     * @deprecated since 5.0, subclass KEmoticonsProvider instead
     */
    KEMOTICONS_DEPRECATED_VERSION(5, 0, "Subclass KEmoticonsProvider")
    bool addEmoticon(const QString &emo,
                     const QString &text,
                     KEmoticonsProvider::AddEmoticonOption option = KEmoticonsProvider::DoNotCopy);
#endif

#if KEMOTICONS_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * Saves the emoticon theme
     *
     * @deprecated since 5.0, subclass KEmoticonsProvider instead
     */
    KEMOTICONS_DEPRECATED_VERSION(5, 0, "Subclass KEmoticonsProvider")
    void save();
#endif

    /**
     * Returns the theme name
     */
    QString themeName() const;

    /**
     * Sets the emoticon theme name
     * @param name name of the theme
     */
    void setThemeName(const QString &name);

    /**
     * Returns the emoticon theme path
     */
    QString themePath() const;

    /**
     * Returns the file name of the emoticon theme
     */
    QString fileName() const;

    /**
     * Returns a QHash that contains the emoticon path as keys and the text as values
     */
    QHash<QString, QStringList> emoticonsMap() const;

#if KEMOTICONS_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * Creates a new theme
     *
     * @deprecated since 5.0, subclass KEmoticonsProvider instead
     */
    KEMOTICONS_DEPRECATED_VERSION(5, 0, "Subclass KEmoticonsProvider")
    void createNew();
#endif

    /**
     * Checks if the emoticon theme has a valid provider
     * @return true if it can't find a valid provider
     */
    bool isNull() const;

    /**
     * @internal
     */
    KEmoticonsTheme &operator=(const KEmoticonsTheme &ket);
private:
    class KEmoticonsThemeData;
    /**
     * Private implementation class
     */
    QSharedDataPointer<KEmoticonsThemeData> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KEmoticonsTheme::ParseMode)

#endif /* KEMOTICONS_THEME_H */

