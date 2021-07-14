/*
    Tests for Kopete::Message::parseEmoticons

    SPDX-FileCopyrightText: 2002-2005 The Kopete developers <kopete-devel@kde.org>
    SPDX-FileCopyrightText: 2004 Richard Smith <kde@metafoo.co.uk>
    SPDX-FileCopyrightText: 2005 Duncan Mac-Vicar <duncan@kde.org>
    SPDX-FileCopyrightText: 2014 Alex Merry <alex.merry@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "autotestbase.h"

#include <QTest>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include <kemoticons.h>

static const char * default_theme = "__woopwoop__";

/*
  There are three sets of tests, the Kopete 0.7 baseline with tests that were
  working properly in Kopete 0.7.x. When these fail it's a real regression.

  The second set are those known to work in the current codebase.
  The last set is the set with tests that are known to fail right now.

   the name convention is (working|broken)-number.input|output
*/

class KEmoticonTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
        cleanupTestCase();

        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        QString destThemePath = dataPath + QLatin1String("/emoticons/");
        QVERIFY(QDir().mkpath(destThemePath));
        QDir themeDir(destThemePath);
        QVERIFY(copyTheme(QFINDTESTDATA("default-testtheme"), themeDir, QString::fromLatin1(default_theme)));

        // check it can actually be found
        themePath = QStandardPaths::locate(
                QStandardPaths::GenericDataLocation,
                QStringLiteral("emoticons/"),
                QStandardPaths::LocateDirectory);
        QVERIFY2(!themePath.isEmpty(), qPrintable(themePath));
        QCOMPARE(themePath, destThemePath);

        // also copy the xmpp theme
        QVERIFY(copyTheme(QFINDTESTDATA("xmpp-testtheme"), themeDir, QStringLiteral("xmpp-testtheme")));
    }

    void cleanupTestCase()
    {
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        const QString themePath = dataPath + QLatin1String("/emoticons/");
        QVERIFY(QDir(themePath + QString::fromLatin1(default_theme)).removeRecursively());
        QVERIFY(QDir(themePath + QStringLiteral("xmpp-testtheme")).removeRecursively());
    }

    void testEmoticonParser_data()
    {
        QTest::addColumn<QString>("inputFileName");
        QTest::addColumn<QString>("outputFileName");
        QTest::addColumn<QString>("themeName");
        QTest::addColumn<bool>("xfail");

        QString basePath = QFINDTESTDATA("emoticon-parser-testcases");
        QVERIFY(!basePath.isEmpty());
        QDir testCasesDir(basePath);

        const QStringList inputFileNames = testCasesDir.entryList(QStringList(QStringLiteral("*.input")));
        for (const QString &fileName : inputFileNames) {
            QString outputFileName = fileName;
            outputFileName.replace(QStringLiteral("input"), QStringLiteral("output"));
            const QString baseName = fileName.section(QLatin1Char('-'), 0, 0);
            QTest::newRow(qPrintable(fileName.left(fileName.lastIndexOf(QLatin1Char('.')))))
                << basePath + QLatin1Char('/') + fileName
                << basePath + QLatin1Char('/') + outputFileName
                << (baseName == QLatin1String("xmpp") ? QStringLiteral("xmpp-testtheme") : QString::fromLatin1(default_theme))
                << (baseName == QLatin1String("broken"));
        }
    }

    void testEmoticonParser()
    {
        QFETCH(QString, inputFileName);
        QFETCH(QString, outputFileName);
        QFETCH(QString, themeName);
        QFETCH(bool, xfail);

        KEmoticonsTheme emo = KEmoticons().theme(themeName);

        QFile inputFile(inputFileName);
        QFile expectedFile(outputFileName);
        if (! expectedFile.exists()) {
            QSKIP("Warning! expected output for testcase not found. Skipping testcase");
        } else if (inputFile.open(QIODevice::ReadOnly) && expectedFile.open(QIODevice::ReadOnly)) {
            const QString inputData = QString::fromLatin1(inputFile.readAll().constData());
            const QString expectedData = QString::fromLatin1(expectedFile.readAll().constData());

            inputFile.close();
            expectedFile.close();

            QString result = emo.parseEmoticons(inputData,
                    KEmoticonsTheme::RelaxedParse | KEmoticonsTheme::SkipHTML);
            result.remove(QStringLiteral("file://") + themePath + themeName + QLatin1Char('/'));

            if (xfail) {
                QEXPECT_FAIL("", "Checking known-broken testcase", Continue);
                QCOMPARE(result, expectedData);
            } else {
                QCOMPARE(result, expectedData);
            }
        } else {
            QSKIP("Warning! can't open testcase files. Skipping testcase");
        }
    }

    void testPreferredSizes_data()
    {
        QTest::addColumn<QString>("themeName");

        QString basePath = QFINDTESTDATA("emoticon-parser-testcases");
        QVERIFY(!basePath.isEmpty());
        QDir testCasesDir(basePath);

        const QStringList inputFileNames = testCasesDir.entryList(QStringList(QStringLiteral("*.input")));
        for (const QString &fileName : inputFileNames) {
            QString outputFileName = fileName;
            outputFileName.replace(QStringLiteral("input"), QStringLiteral("output"));
            const QString baseName = fileName.section(QLatin1Char('-'), 0, 0);
            QTest::newRow(qPrintable(fileName.left(fileName.lastIndexOf(QLatin1Char('.')))))
            << (baseName == QLatin1String("xmpp") ? QStringLiteral("xmpp-testtheme") : QString::fromLatin1(default_theme));
        }
    }

    void testPreferredSizes()
    {
        QFETCH(QString, themeName);
        KEmoticons kemoticons;
        kemoticons.setPreferredEmoticonSize(QSize(99, 77));

        KEmoticonsTheme theme = kemoticons.theme(themeName);

        const QString parsed = theme.parseEmoticons(QStringLiteral(":)"));

        QVERIFY(parsed.contains(QLatin1String("width=\"99\"")));
        QVERIFY(parsed.contains(QLatin1String("height=\"77\"")));
    }

private:
    QString themePath;
};

QTEST_MAIN(KEmoticonTest)

#include <kemoticontest.moc>

