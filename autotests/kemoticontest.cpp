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

#include <QTest>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
#include <QDebug>

#include <kemoticons.h>

static const char * default_theme = "Glass";

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
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

        QString destThemePath = dataPath + QLatin1String("/emoticons/") + QLatin1String(default_theme);
        QDir themeDir(destThemePath);
        if (themeDir.exists()) {
            QVERIFY(themeDir.removeRecursively());
        }
        QVERIFY(themeDir.mkpath("."));

        QDir sourceThemeDir(QFile::decodeName(LOCAL_THEMES_DIR) + QLatin1String("/") + default_theme);
        QVERIFY(sourceThemeDir.exists());

        foreach (QString fileName, sourceThemeDir.entryList(QDir::Files)) {
            QVERIFY(QFile::copy(sourceThemeDir.filePath(fileName),
                                themeDir.filePath(fileName)));
        }

        // check it can actually be found
        themePath = QStandardPaths::locate(
                QStandardPaths::GenericDataLocation,
                QString::fromLatin1("emoticons/") + default_theme,
                QStandardPaths::LocateDirectory);
        QVERIFY2(!themePath.isEmpty(), qPrintable(themePath));
        // testEmoticonParser() wants a trailing /
        themePath += "/";
    }

    void cleanupTestCase()
    {
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        QString themePath = dataPath + QLatin1String("/emoticons/") + QLatin1String(default_theme);
        QDir themeDir(themePath);
        QVERIFY(themeDir.removeRecursively());
    }

    void testEmoticonParser_data()
    {
        QTest::addColumn<QString>("inputFileName");
        QTest::addColumn<QString>("outputFileName");
        QTest::addColumn<bool>("xfail");

        QString basePath = QFINDTESTDATA("emoticon-parser-testcases");
        QVERIFY(!basePath.isEmpty());
        QDir testCasesDir(basePath);

        QStringList inputFileNames = testCasesDir.entryList(QStringList(QLatin1String("*.input")));
        Q_FOREACH (const QString &fileName, inputFileNames) {
            QString outputFileName = fileName;
            outputFileName.replace("input", "output");
            QTest::newRow(qPrintable(fileName.left(fileName.lastIndexOf('.'))))
                << basePath + QString::fromLatin1("/") + fileName
                << basePath + QString::fromLatin1("/") + outputFileName
                << (fileName.section("-", 0, 0) == QLatin1String("broken"));
        }
    }

    void testEmoticonParser()
    {
        KEmoticonsTheme emo = KEmoticons().theme(default_theme);

        QFETCH(QString, inputFileName);
        QFETCH(QString, outputFileName);
        QFETCH(bool, xfail);

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
            result.replace(themePath, QString());

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

private:
    QString themePath;
};

QTEST_MAIN(KEmoticonTest)

#include <kemoticontest.moc>

