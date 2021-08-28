/*
    SPDX-FileCopyrightText: 2007 Carlo Segato <brandon.ml@gmail.com>
    SPDX-FileCopyrightText: 2008 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kemoticons.h"
#include "kemoticonsglobal_p.h"
#include "kemoticonsprovider.h"

#include <QFile>
#include <QDir>
#include <QMimeDatabase>
#include <QStandardPaths>
#include "kemoticons_core_debug.h"
#include <QFileSystemWatcher>

#include <KPluginLoader>
#include <KPluginMetaData>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KTar>
#include <kzip.h>

Q_GLOBAL_STATIC(KEmoticonsGlobal, s_global)

class KEmoticonsPrivate
{
public:
    KEmoticonsPrivate(KEmoticons *parent);
    ~KEmoticonsPrivate();
    void loadServiceList();
    KEmoticonsProvider *loadProvider(const KPluginMetaData &plugin);
    KEmoticonsProvider *loadProvider(const KService::Ptr &service);
    KEmoticonsTheme loadTheme(const QString &name);

    QList<KService::Ptr> m_oldStylePlugins;
    QVector<KPluginMetaData> m_plugins;
    QHash<QString, KEmoticonsTheme> m_themes;
    QFileSystemWatcher m_fileWatcher;
    KEmoticons *q;
    QSize m_preferredSize;

    //private slots
    void changeTheme(const QString &path);
};

KEmoticonsPrivate::KEmoticonsPrivate(KEmoticons *parent)
    : q(parent)
{
}

KEmoticonsPrivate::~KEmoticonsPrivate()
{
}

static bool priorityLessThan(const KService::Ptr &s1, const KService::Ptr &s2)
{
    return (s1->property(QStringLiteral("X-KDE-Priority")).toInt() > s2->property(QStringLiteral("X-KDE-Priority")).toInt());
}

void KEmoticonsPrivate::loadServiceList()
{
    const QString constraint(QStringLiteral("(exist Library)"));
    m_oldStylePlugins = KServiceTypeTrader::self()->query(QStringLiteral("KEmoticons"), constraint);
    std::sort(m_oldStylePlugins.begin(), m_oldStylePlugins.end(), priorityLessThan);

    m_plugins = KPluginLoader::findPlugins(QStringLiteral("kf5/emoticonsthemes"));
    std::sort(m_plugins.begin(), m_plugins.end(), [](const KPluginMetaData &s1, const KPluginMetaData &s2) {
            return s1.rawData().value(QStringLiteral("X-KDE-Priority")).toInt() > s2.rawData().value(QStringLiteral("X-KDE-Priority")).toInt();
    });
}

KEmoticonsProvider *KEmoticonsPrivate::loadProvider(const KPluginMetaData &plugin)
{
    KPluginFactory *factory = qobject_cast<KPluginFactory *>(plugin.instantiate());
    if (!factory) {
        qCWarning(KEMOTICONS_CORE) << "Invalid plugin factory for" << plugin.fileName();
        return nullptr;
    }
    KEmoticonsProvider *provider = factory->create<KEmoticonsProvider>(nullptr);
    return provider;
}

KEmoticonsProvider *KEmoticonsPrivate::loadProvider(const KService::Ptr &service)
{
    KPluginFactory *factory = KPluginLoader(service->library()).factory();
    if (!factory) {
        qCWarning(KEMOTICONS_CORE) << "Invalid plugin factory for" << service->library();
        return nullptr;
    }
    KEmoticonsProvider *provider = factory->create<KEmoticonsProvider>(nullptr);
    return provider;
}

void KEmoticonsPrivate::changeTheme(const QString &path)
{
    const QFileInfo info(path);
    const QString name = info.dir().dirName();

    if (m_themes.contains(name)) {
        loadTheme(name);
    }
}

KEmoticonsTheme KEmoticonsPrivate::loadTheme(const QString &name)
{
    const auto registerProvider = [this](const QString &name, const QString &path, KEmoticonsProvider *provider) {
        if (m_preferredSize.isValid()) {
            provider->setPreferredEmoticonSize(m_preferredSize);
        }
        KEmoticonsTheme theme(provider);
        provider->loadTheme(path);
        m_themes.insert(name, theme);
        m_fileWatcher.addPath(path);
        return theme;
    };

    for (const KPluginMetaData &plugin : std::as_const(m_plugins)) {
        const QString fName = plugin.rawData().value(QStringLiteral("X-KDE-EmoticonsFileName")).toString();
        const QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("emoticons/") + name + QLatin1Char('/') + fName);

        if (QFile::exists(path)) {
            KEmoticonsProvider *provider = loadProvider(plugin);
            if (provider) {
                return registerProvider(name, path, provider);
            }
        }
    }
    // KF6: remove support for old plugins
    for (const KService::Ptr &service : std::as_const(m_oldStylePlugins)) {
        const QString fName = service->property(QStringLiteral("X-KDE-EmoticonsFileName")).toString();
        const QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("emoticons/") + name + QLatin1Char('/') + fName);

        if (QFile::exists(path)) {
            KEmoticonsProvider *provider = loadProvider(service);
            if (provider) {
                return registerProvider(name, path, provider);
            }
        }
    }
    return KEmoticonsTheme();
}

KEmoticons::KEmoticons()
    : d(new KEmoticonsPrivate(this))
{
    d->loadServiceList();
    connect(&d->m_fileWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &str) {d->changeTheme(str);});
}

KEmoticons::~KEmoticons()
{
}

KEmoticonsTheme KEmoticons::theme() const
{
    return theme(currentThemeName());
}

KEmoticonsTheme KEmoticons::theme(const QString &name) const
{
    KEmoticonsTheme theme = d->m_themes.value(name);
    if (!theme.isNull()) {
        return theme;
    }

    return d->loadTheme(name);
}

QString KEmoticons::currentThemeName()
{
    return s_global()->m_themeName;
}

QStringList KEmoticons::themeList()
{
    const QStringList themeDirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("emoticons"), QStandardPaths::LocateDirectory);
    QStringList ls;
    ls.reserve(themeDirs.count());

    for (int i = 0; i < themeDirs.count(); ++i) {
        QDir themeQDir(themeDirs[i]);
        themeQDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        themeQDir.setSorting(QDir::Name);
        ls << themeQDir.entryList();
    }
    return ls;
}

void KEmoticons::setTheme(const KEmoticonsTheme &theme)
{
    setTheme(theme.themeName());
}

void KEmoticons::setTheme(const QString &theme)
{
    s_global()->setThemeName(theme);
}

KEmoticonsTheme::ParseMode KEmoticons::parseMode()
{
    return s_global()->m_parseMode;
}

void KEmoticons::setParseMode(KEmoticonsTheme::ParseMode mode)
{
    s_global()->setParseMode(mode);
}

KEmoticonsTheme KEmoticons::newTheme(const QString &name, const KService::Ptr &service)
{
    KEmoticonsProvider *provider = d->loadProvider(service);
    if (provider) {
        KEmoticonsTheme theme(provider);
        theme.setThemeName(name);

        provider->newTheme();

        return theme;
    }
    return KEmoticonsTheme();
}

QStringList KEmoticons::installTheme(const QString &archiveName)
{
    QStringList foundThemes;
    const KArchiveEntry *currentEntry = nullptr;
    const KArchiveDirectory *currentDir = nullptr;
    KArchive *archive = nullptr;

    const QString localThemesDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/emoticons"));

    if (localThemesDir.isEmpty()) {
        qCCritical(KEMOTICONS_CORE) << "Could not find a suitable place in which to install the emoticon theme";
        return QStringList();
    }

    QMimeDatabase db;
    const QString currentBundleMimeType = db.mimeTypeForFile(archiveName).name();

    if (currentBundleMimeType == QLatin1String("application/zip") ||
            currentBundleMimeType == QLatin1String("application/x-zip") ||
            currentBundleMimeType == QLatin1String("application/x-zip-compressed")) {
        archive = new KZip(archiveName);
    } else if (currentBundleMimeType == QLatin1String("application/x-compressed-tar") ||
               currentBundleMimeType == QLatin1String("application/x-bzip-compressed-tar") ||
               currentBundleMimeType == QLatin1String("application/x-lzma-compressed-tar") ||
               currentBundleMimeType == QLatin1String("application/x-xz-compressed-tar") ||
               currentBundleMimeType == QLatin1String("application/x-gzip") ||
               currentBundleMimeType == QLatin1String("application/x-bzip") ||
               currentBundleMimeType == QLatin1String("application/x-lzma") ||
               currentBundleMimeType == QLatin1String("application/x-xz")) {
        archive = new KTar(archiveName);
    } else if (archiveName.endsWith(QLatin1String("jisp")) || archiveName.endsWith(QLatin1String("zip"))) {
        archive = new KZip(archiveName);
    } else {
        archive = new KTar(archiveName);
    }

    if (!archive || !archive->open(QIODevice::ReadOnly)) {
        qCCritical(KEMOTICONS_CORE) << "Could not open" << archiveName << "for unpacking";
        delete archive;
        return QStringList();
    }

    const KArchiveDirectory *rootDir = archive->directory();

    // iterate all the dirs looking for an emoticons.xml file
    const QStringList entries = rootDir->entries();
    for (QStringList::ConstIterator it = entries.begin(); it != entries.end(); ++it) {
        currentEntry = const_cast<KArchiveEntry *>(rootDir->entry(*it));

        if (currentEntry->isDirectory()) {
            currentDir = dynamic_cast<const KArchiveDirectory *>(currentEntry);

            for (const KPluginMetaData &plugin : std::as_const(d->m_plugins)) {
                const QString fName = plugin.rawData().value(QStringLiteral("X-KDE-EmoticonsFileName")).toString();
                if (currentDir && currentDir->entry(fName) != nullptr) {
                    foundThemes.append(currentDir->name());
                }
            }
            for (const KService::Ptr &service : std::as_const(d->m_oldStylePlugins)) {
                const QString fName = service->property(QStringLiteral("X-KDE-EmoticonsFileName")).toString();
                if (currentDir && currentDir->entry(fName) != nullptr) {
                    foundThemes.append(currentDir->name());
                }
            }
        }
    }

    if (foundThemes.isEmpty()) {
        qCCritical(KEMOTICONS_CORE) << "The file" << archiveName << "is not a valid emoticon theme archive";
        archive->close();
        delete archive;
        return QStringList();
    }

    for (int themeIndex = 0; themeIndex < foundThemes.size(); ++themeIndex) {
        const QString &theme = foundThemes[themeIndex];

        currentEntry = const_cast<KArchiveEntry *>(rootDir->entry(theme));
        if (currentEntry == nullptr) {
            // qCDebug(KEMOTICONS_CORE) << "couldn't get next archive entry";
            continue;
        }

        if (currentEntry->isDirectory()) {
            currentDir = dynamic_cast<const KArchiveDirectory *>(currentEntry);

            if (currentDir == nullptr) {
                // qCDebug(KEMOTICONS_CORE) << "couldn't cast archive entry to KArchiveDirectory";
                continue;
            }

            currentDir->copyTo(localThemesDir + theme);
        }
    }

    archive->close();
    delete archive;

    return foundThemes;
}

void KEmoticons::setPreferredEmoticonSize(const QSize &size)
{
    d->m_preferredSize = size;
}

QSize KEmoticons::preferredEmoticonSize() const
{
    return d->m_preferredSize;
}

#include "moc_kemoticons.cpp"
