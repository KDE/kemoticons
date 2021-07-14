/*
    SPDX-FileCopyrightText: 2002-2008 The Kopete developers <kopete-devel@kde.org>
    SPDX-FileCopyrightText: 2008 Carlo Segato <brandon.ml@gmail.com>
    SPDX-FileCopyrightText: 2002-2003 Stefan Gehn <metz@gehn.net>
    SPDX-FileCopyrightText: 2005 Engin AYDOGAN <engin@bzzzt.biz>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kemoticonstheme.h"
#include "kemoticons.h"

#include "kemoticons_core_debug.h"

class Q_DECL_HIDDEN KEmoticonsTheme::KEmoticonsThemeData : public QSharedData
{
public:
    KEmoticonsThemeData();
    ~KEmoticonsThemeData();
    KEmoticonsProvider *provider = nullptr;
};

KEmoticonsTheme::KEmoticonsThemeData::KEmoticonsThemeData()
{
}

KEmoticonsTheme::KEmoticonsThemeData::~KEmoticonsThemeData()
{
//     delete provider;
}

KEmoticonsTheme::KEmoticonsTheme()
{
    d = new KEmoticonsThemeData;
}

KEmoticonsTheme::KEmoticonsTheme(const KEmoticonsTheme &ket)
{
    d = ket.d;
}

KEmoticonsTheme::KEmoticonsTheme(KEmoticonsProvider *p)
{
    d = new KEmoticonsThemeData;
    d->provider = p;
}

KEmoticonsTheme::~KEmoticonsTheme()
{
}

#if KEMOTICONS_BUILD_DEPRECATED_SINCE(5, 0)
bool KEmoticonsTheme::loadTheme(const QString &path)
{
    if (!d->provider) {
        return false;
    }

    return d->provider->loadTheme(path);
}
#endif

#if KEMOTICONS_BUILD_DEPRECATED_SINCE(5, 0)
bool KEmoticonsTheme::removeEmoticon(const QString &emo)
{
    if (!d->provider) {
        return false;
    }

    return d->provider->removeEmoticon(emo);
}
#endif

#if KEMOTICONS_BUILD_DEPRECATED_SINCE(5, 0)
bool KEmoticonsTheme::addEmoticon(const QString &emo, const QString &text, KEmoticonsProvider::AddEmoticonOption option)
{
    if (!d->provider) {
        return false;
    }

    return d->provider->addEmoticon(emo, text, option);
}
#endif

#if KEMOTICONS_BUILD_DEPRECATED_SINCE(5, 0)
void KEmoticonsTheme::save()
{
    if (!d->provider) {
        return;
    }

    d->provider->saveTheme();
}
#endif

QString KEmoticonsTheme::themeName() const
{
    if (!d->provider) {
        return QString();
    }

    return d->provider->themeName();
}

void KEmoticonsTheme::setThemeName(const QString &name)
{
    if (!d->provider) {
        return;
    }

    d->provider->setThemeName(name);
}

QString KEmoticonsTheme::themePath() const
{
    if (!d->provider) {
        return QString();
    }

    return d->provider->themePath();
}

QString KEmoticonsTheme::fileName() const
{
    if (!d->provider) {
        return QString();
    }

    return d->provider->fileName();
}

QHash<QString, QStringList> KEmoticonsTheme::emoticonsMap() const
{
    if (!d->provider) {
        return QHash<QString, QStringList>();
    }

    return d->provider->emoticonsMap();
}

#if KEMOTICONS_BUILD_DEPRECATED_SINCE(5, 0)
void KEmoticonsTheme::createNew()
{
    if (!d->provider) {
        return;
    }

    d->provider->newTheme();
}
#endif

QString KEmoticonsTheme::parseEmoticons(const QString &text, ParseMode mode, const QStringList &exclude) const
{
    const QList<Token> tokens = tokenize(text, mode | SkipHTML);
    if (tokens.isEmpty() && !text.isEmpty()) {
        return text;
    }

    QString result;

    for (const Token &token : tokens) {
        switch (token.type) {
        case Text:
            result += token.text;
            break;
        case Image:
            if (!exclude.contains(token.text)) {
                result += token.picHTMLCode;
            } else {
                result += token.text;
            }
            break;
        default:
            qCWarning(KEMOTICONS_CORE) << "Unknown token type. Something's broken.";
            break;
        }
    }
    return result;
}

bool EmoticonCompareEscaped(const KEmoticonsProvider::Emoticon &s1, const KEmoticonsProvider::Emoticon &s2)
{
    return s1.matchTextEscaped.length() > s2.matchTextEscaped.length();
}
bool EmoticonCompare(const KEmoticonsProvider::Emoticon &s1, const KEmoticonsProvider::Emoticon &s2)
{
    return s1.matchText.length() > s2.matchText.length();
}

QList<KEmoticonsTheme::Token> KEmoticonsTheme::tokenize(const QString &message, ParseMode mode) const
{
    if (!d->provider) {
        return QList<KEmoticonsTheme::Token>();
    }

    if (!(mode & (StrictParse | RelaxedParse))) {
        //if none of theses two mode are selected, use the mode from the config
        mode |=  KEmoticons::parseMode();
    }

    QList<Token> result;

    /* previous char, in the firs iteration assume that it is space since we want
     * to let emoticons at the beginning, the very first previous QChar must be a space. */
    QChar p = QLatin1Char(' ');
    QChar c; /* current char */
    QChar n;

    /* This is the EmoticonNode container, it will represent each matched emoticon */
    typedef QPair<KEmoticonsProvider::Emoticon, int> EmoticonNode;
    QList<EmoticonNode> foundEmoticons;
    /* First-pass, store the matched emoticon locations in foundEmoticons */
    QList<KEmoticonsProvider::Emoticon> emoticonList;
    QList<KEmoticonsProvider::Emoticon>::const_iterator it;
    int pos;

    bool inHTMLTag = false;
    bool inHTMLLink = false;
    bool inHTMLEntity = false;
    QString needle; // search for this

    for (pos = 0; pos < message.length(); ++pos) {
        c = message[pos];

        if (mode & SkipHTML) { // Shall we skip HTML ?
            if (!inHTMLTag) { // Are we already in an HTML tag ?
                if (c == QLatin1Char('<')) { // If not check if are going into one
                    inHTMLTag = true; // If we are, change the state to inHTML
                    p = c;
                    continue;
                }
            } else { // We are already in a HTML tag
                if (c == QLatin1Char('>')) { // Check if it ends
                    inHTMLTag = false;   // If so, change the state

                    if (p == QLatin1Char('a')) {
                        inHTMLLink = false;
                    }
                } else if (c == QLatin1Char('a') && p == QLatin1Char('<')) { // check if we just entered an anchor tag
                    inHTMLLink = true; // don't put smileys in urls
                }
                p = c;
                continue;
            }

            if (!inHTMLEntity) { // are we
                if (c == QLatin1Char('&')) {
                    inHTMLEntity = true;
                }
            }
        }

        if (inHTMLLink) { // i can't think of any situation where a link address might need emoticons
            p = c;
            continue;
        }

        if ((mode & StrictParse)  &&  !p.isSpace() && p != QLatin1Char('>')) {  // '>' may mark the end of an html tag
            p = c;
            continue;
        } /* strict requires space before the emoticon */

        if (d->provider->emoticonsIndex().contains(c)) {
            emoticonList = d->provider->emoticonsIndex().value(c);
            if (mode & SkipHTML) {
                std::sort(emoticonList.begin(), emoticonList.end(), EmoticonCompareEscaped);
            } else {
                std::sort(emoticonList.begin(), emoticonList.end(), EmoticonCompare);
            }
            bool found = false;
            QList<KEmoticonsProvider::Emoticon>::const_iterator end = emoticonList.constEnd();
            for (it = emoticonList.constBegin(); it != end; ++it) {
                // If this is an HTML, then search for the HTML form of the emoticon.
                // For instance <o) => &gt;o)
                needle = (mode & SkipHTML) ? (*it).matchTextEscaped : (*it).matchText;
                if (pos == message.indexOf(needle, pos)) {
                    if (mode & StrictParse) {
                        /* check if the character after this match is space or end of string*/
                        if (message.length() > pos + needle.length()) {
                            n = message[pos + needle.length()];
                            //<br/> marks the end of a line
                            if (n != QLatin1Char('<') && !n.isSpace() &&  !n.isNull() && n != QLatin1Char('&')) {
                                break;
                            }
                        }
                    }
                    /* Perfect match */
                    foundEmoticons.append(EmoticonNode((*it), pos));
                    found = true;
                    /* Skip the matched emoticon's matchText */
                    pos += needle.length() - 1;
                    break;
                }

                if (found) {
                    break;
                }
            }

            if (!found) {
                if (inHTMLEntity) {
                    // If we are in an HTML entity such as &gt;
                    const int htmlEnd = message.indexOf(QLatin1Char(';'), pos);
                    // Search for where it ends
                    if (htmlEnd == -1) {
                        // Apparently this HTML entity isn't ended, something is wrong, try skip the '&'
                        // and continue
                        // qCDebug(KEMOTICONS_CORE) << "Broken HTML entity, trying to recover.";
                        inHTMLEntity = false;
                        pos++;
                    } else {
                        pos = htmlEnd;
                        inHTMLEntity = false;
                    }
                }
            }
        } /* else no emoticons begin with this character, so don't do anything */
        p = c;
    }

    /* if no emoticons found just return the text */
    if (foundEmoticons.isEmpty()) {
        result.append(Token(Text, message));
        return result;
    }

    /* Second-pass, generate tokens based on the matches */

    pos = 0;
    int length;

    for (int i = 0, total = foundEmoticons.size(); i < total; ++i) {
        EmoticonNode itFound = foundEmoticons.at(i);
        needle = (mode & SkipHTML) ? itFound.first.matchTextEscaped : itFound.first.matchText;

        if ((length = (itFound.second - pos))) {
            result.append(Token(Text, message.mid(pos, length)));
            result.append(Token(Image, itFound.first.matchTextEscaped, itFound.first.picPath, itFound.first.picHTMLCode));
            pos += length + needle.length();
        } else {
            result.append(Token(Image, itFound.first.matchTextEscaped, itFound.first.picPath, itFound.first.picHTMLCode));
            pos += needle.length();
        }
    }

    if (message.length() - pos) { // if there is remaining regular text
        result.append(Token(Text, message.mid(pos)));
    }

    return result;
}

bool KEmoticonsTheme::isNull() const
{
    return d->provider ? false : true;
}

KEmoticonsTheme &KEmoticonsTheme::operator=(const KEmoticonsTheme &ket)
{
    if (d == ket.d) {
        return *this;
    }

    d = ket.d;
    return *this;
}

