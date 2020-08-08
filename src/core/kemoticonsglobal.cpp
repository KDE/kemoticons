/*
    SPDX-FileCopyrightText: 2017 David Faure <faure@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kemoticonsglobal_p.h"
#include <QDBusMessage>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QDBusConnection>
#include "kemoticons_core_debug.h"

static const char s_dbusPath[] = "/KEmoticons";
static const char s_dbusInterface[] = "org.kde.kf5.KEmoticons";
static const char s_themeChangedSignal[] = "emoticonsThemeChanged";
static const char s_parseModeChangedSignal[] = "emoticonsParseModeChanged";

KEmoticonsGlobal::KEmoticonsGlobal()
{
    KConfigGroup config(KSharedConfig::openConfig(), "Emoticons");
    m_themeName = config.readEntry("emoticonsTheme", "Breeze");
    m_parseMode = static_cast<KEmoticonsTheme::ParseMode>(config.readEntry("parseMode", int(KEmoticonsTheme::RelaxedParse)));

    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.connect(QString(), QString::fromLatin1(s_dbusPath), QString::fromLatin1(s_dbusInterface), QString::fromLatin1(s_themeChangedSignal), this, SLOT(slotEmoticonsThemeChanged(QString)));
    dbus.connect(QString(), QString::fromLatin1(s_dbusPath), QString::fromLatin1(s_dbusInterface), QString::fromLatin1(s_parseModeChangedSignal), this, SLOT(slotEmoticonsParseModeChanged(int)));
}

void KEmoticonsGlobal::setThemeName(const QString &name)
{
    m_themeName = name;

    KConfigGroup config(KSharedConfig::openConfig(), "Emoticons");
    config.writeEntry("emoticonsTheme", m_themeName, KConfigBase::Persistent | KConfigBase::Global);
    config.sync();

    // Inform running apps
    QDBusMessage message = QDBusMessage::createSignal(QString::fromLatin1(s_dbusPath), QString::fromLatin1(s_dbusInterface), QString::fromLatin1(s_themeChangedSignal));
    message << name;
    if (!QDBusConnection::sessionBus().send(message)) {
        qCWarning(KEMOTICONS_CORE) << "Error sending dbus signal" << s_themeChangedSignal;
    }
}

void KEmoticonsGlobal::setParseMode(KEmoticonsTheme::ParseMode mode)
{
    m_parseMode = mode;

    KConfigGroup config(KSharedConfig::openConfig(), "Emoticons");
    config.writeEntry("parseMode", static_cast<int>(mode), KConfigBase::Persistent | KConfigBase::Global);
    config.sync();

    // Inform running apps
    QDBusMessage message = QDBusMessage::createSignal(QString::fromLatin1(s_dbusPath), QString::fromLatin1(s_dbusInterface), QString::fromLatin1(s_parseModeChangedSignal));
    message << static_cast<int>(mode);
    if (!QDBusConnection::sessionBus().send(message)) {
        qCWarning(KEMOTICONS_CORE) << "Error sending dbus signal" << s_parseModeChangedSignal;
    }
}

void KEmoticonsGlobal::slotEmoticonsThemeChanged(const QString &name)
{
    m_themeName = name;
}

void KEmoticonsGlobal::slotEmoticonsParseModeChanged(int mode)
{
    m_parseMode = static_cast<KEmoticonsTheme::ParseMode>(mode);
}
