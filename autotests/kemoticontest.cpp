/*
    Tests for Kopete::Message::parseEmoticons

    Copyright (c) 2004      by Richard Smith          <kde@metafoo.co.uk>
    Copyright (c) 2005      by Duncan Mac-Vicar       <duncan@kde.org>
    Copyright (c) 2014      by Alex Merry             <alex.merry@kde.org>

    Kopete    (c) 2002-2005 by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#include "autotestbase.h"

#include <QTest>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
#include <QDebug>

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
        const QString destPath = destThemePath + QString::fromLatin1(default_theme);
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

        QStringList inputFileNames = testCasesDir.entryList(QStringList(QStringLiteral("*.input")));
        Q_FOREACH (const QString &fileName, inputFileNames) {
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
            QSKIP("Warning! expected output for testcase not found. Skiping testcase");
        } else if (inputFile.open(QIODevice::ReadOnly) && expectedFile.open(QIODevice::ReadOnly)) {
            const QString inputData = QString::fromLatin1(inputFile.readAll().constData());
            const QString expectedData = QString::fromLatin1(expectedFile.readAll().constData());

            inputFile.close();
            expectedFile.close();

            QString result = emo.parseEmoticons(inputData,
                    KEmoticonsTheme::RelaxedParse | KEmoticonsTheme::SkipHTML);
            result.replace(themePath + themeName + QLatin1Char('/'), QString());

            if (xfail) {
                QEXPECT_FAIL("", "Checking known-broken testcase", Continue);
                QCOMPARE(result, expectedData);
            } else {
                QCOMPARE(result, expectedData);
            }
        } else {
            QSKIP("Warning! can't open testcase files. Skiping testcase");
        }
    }

    void testPreferredSizes_data()
    {
        QTest::addColumn<QString>("themeName");

        QString basePath = QFINDTESTDATA("emoticon-parser-testcases");
        QVERIFY(!basePath.isEmpty());
        QDir testCasesDir(basePath);

        QStringList inputFileNames = testCasesDir.entryList(QStringList(QStringLiteral("*.input")));
        Q_FOREACH (const QString &fileName, inputFileNames) {
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

        QVERIFY(parsed.contains(QStringLiteral("width=\"99\"")));
        QVERIFY(parsed.contains(QStringLiteral("height=\"77\"")));
    }

private:
    QString themePath;
};

QTEST_MAIN(KEmoticonTest)

#include <kemoticontest.moc>

