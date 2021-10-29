/*
    SPDX-FileCopyrightText: 2008 Carlo Segato <brandon.ml@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KEMOTICONS_PROVIDER_H
#define KEMOTICONS_PROVIDER_H

#include <kemoticons_export.h>

#include <QObject>
#include <QStringList>

class KEmoticonsProviderPrivate;
struct Emoticon;

/**
 * This is the base abstract class for the emoticon provider plugins.
 */
class KEMOTICONS_EXPORT KEmoticonsProvider : public QObject
{
    Q_OBJECT
public:
    struct Emoticon {
        Emoticon() {}
        /* sort by longest to shortest matchText */
        bool operator < (const Emoticon &e) const
        {
            return matchText.length() > e.matchText.length();
        }
        QString matchText;
        QString matchTextEscaped;
        QString picPath;
        QString picHTMLCode;
    };

    /**
     * Options to pass to addEmoticon
     */
    enum AddEmoticonOption {
        DoNotCopy, /**<< Don't copy the emoticon file into the theme directory */
        Copy /**<< Copy the emoticon file into the theme directory */
    };

    /**
     * Default constructor
     */
    explicit KEmoticonsProvider(QObject *parent = nullptr);

    /**
     * Destructor
     */
    ~KEmoticonsProvider() override;

    /**
     * Loads the emoticon theme inside the directory @p path
     * @param path path to the directory
     * @return @c true if the emoticon theme is successfully loaded
     */
    virtual bool loadTheme(const QString &path) = 0;

    /**
     * Removes the emoticon @p emo. This doesn't delete the image file.
     * @param emo the emoticon text to remove
     * @return @c true if the emoticon theme is successfully removed
     */
    virtual bool removeEmoticon(const QString &emo) = 0;

    /**
     * Adds the emoticon @p emo with text @p text
     * @param emo path to the emoticon image
     * @param text the emoticon text. If alternative texts are to be added,
     * use spaces to separate them.
     * @param copy whether or not to copy @p emo into the theme directory
     * @return @c true if the emoticon is successfully added
     */
    virtual bool addEmoticon(const QString &emo, const QString &text, AddEmoticonOption option = DoNotCopy) = 0;

#if KEMOTICONS_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * Saves the emoticon theme
     *
     * @deprecated since 5.0, use saveTheme() instead
     */
    KEMOTICONS_DEPRECATED_VERSION(5, 0, "Use KEmoticonsProvider::saveTheme()")
    void save();
#endif

    /**
     * Saves the emoticon theme
     * @since 5.0
     */
    virtual void saveTheme() = 0;

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

    /**
     * Returns a QHash that contains emoticons indexed by the first char
     */
    QHash<QChar, QList<Emoticon> > emoticonsIndex() const;

#if KEMOTICONS_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * Creates a new theme
     *
     * @deprecated since 5.0, use newTheme() instead
     */
    KEMOTICONS_DEPRECATED_VERSION(5, 0, "Use KEmoticonsProvider::newTheme()")
    void createNew();
#endif

    /**
     * Creates a new theme
     * @since 5.0
     */
    virtual void newTheme() = 0;

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

protected:
    /**
     * Sets the theme inside the directory @p path
     * @param path path to the directory
     * @since 5.0
     */
    void setThemePath(const QString &path);

    /**
     * Copies the emoticon image to the theme directory
     * @param emo path to the emoticon image
     * @return true if the emoticon is successfully copied
     * @since 5.0
     */
    bool copyEmoticon(const QString &emo);

    /**
     * Clears the emoticons map
     */
    void clearEmoticonsMap();

#if KEMOTICONS_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * Inserts a new item in the emoticon map
     *
     * @deprecated since 5.0, use addMapItem() instead
     */
    KEMOTICONS_DEPRECATED_VERSION(5, 0, "Use KEmoticonsProvider::addMapItem(QString, QStringList)")
    void addEmoticonsMap(QString key, QStringList value);
#endif

    /**
     * Inserts a new item in the emoticon map
     * @since 5.0
     * @see emoticonsMap()
     */
    //FIXME kf6: use const'ref here
    void addMapItem(QString key, QStringList value);

#if KEMOTICONS_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * Removes an item from the emoticon map
     *
     * @deprecated since 5.0, use removeMapItem() instead
     */
    KEMOTICONS_DEPRECATED_VERSION(5, 0, "Use KEmoticonsProvider::removeMapItem(QString)")
    void removeEmoticonsMap(QString key);
#endif
    /**
     * Removes an item from the emoticon map
     * @since 5.0
     * @see emoticonsMap()
     */
    void removeMapItem(QString key);

#if KEMOTICONS_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * Adds an emoticon to the index
     * @param path path to the emoticon
     * @param emoList list of text associated with this emoticon
     *
     * @deprecated since 5.0, use addIndexItem() instead
     */
    KEMOTICONS_DEPRECATED_VERSION(5, 0, "Use KEmoticonsProvider::addIndexItem(const QString &, const QStringList &)")
    void addEmoticonIndex(const QString &path, const QStringList &emoList);
#endif
    /**
     * Adds an emoticon to the index
     * @param path path to the emoticon
     * @param emoList list of text associated with this emoticon
     * @since 5.0
     * @see emoticonsIndex()
     */
    void addIndexItem(const QString &path, const QStringList &emoList);

#if KEMOTICONS_ENABLE_DEPRECATED_SINCE(5, 0)
    /**
     * Removes an emoticon from the index
     * @param path path to the emoticon
     * @param emoList list of text associated with this emoticon
     *
     * @deprecated since 5.0, use removeIndexItem() instead
     */
    KEMOTICONS_DEPRECATED_VERSION(5, 0, "Use KEmoticonsProvider::removeIndexItem(const QString &, const QStringList &)")
    void removeEmoticonIndex(const QString &path, const QStringList &emoList);
#endif
    /**
     * Removes an emoticon from the index
     * @param path path to the emoticon
     * @param emoList list of text associated with this emoticon
     * @since 5.0
     * @see emoticonsIndex()
     */
    void removeIndexItem(const QString &path, const QStringList &emoList);

private:
    /**
     * Private implementation class
     */
    const QScopedPointer<KEmoticonsProviderPrivate> d;
};

#endif /* KEMOTICONS_PROVIDER_H */

