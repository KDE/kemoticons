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

static const char * default_theme = "Breeze";

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

private:
    bool copyTheme(const QString &dir, const QDir &baseThemeDir, const QString &themeName)
    {
        QDir sourceThemeDir(dir);
        if (!sourceThemeDir.exists()) {
            return false;
        }
        QDir themeDir(baseThemeDir.absolutePath() + '/' + themeName);
        themeDir.removeRecursively();
        themeDir.mkpath(".");

        foreach (const QString &fileName, sourceThemeDir.entryList(QDir::Files)) {
            if (!QFile::copy(sourceThemeDir.filePath(fileName),
                                themeDir.filePath(fileName))) {
                qWarning() << "couldn't copy" << dir << "/" << fileName;
                return false;
            }
        }
        return true;
    }

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

        QString destThemePath = dataPath + QLatin1String("/emoticons/");
        QDir themeDir(destThemePath);
        if (themeDir.exists()) {
            QVERIFY(themeDir.removeRecursively());
        }
        QVERIFY(themeDir.mkpath("."));

        QVERIFY(copyTheme(QFile::decodeName(LOCAL_THEMES_DIR) + QLatin1String("/") + default_theme, themeDir, default_theme));

        // check it can actually be found
        themePath = QStandardPaths::locate(
                QStandardPaths::GenericDataLocation,
                QString::fromLatin1("emoticons/"),
                QStandardPaths::LocateDirectory);
        QVERIFY2(!themePath.isEmpty(), qPrintable(themePath));

        // also copy the xmpp theme
        QVERIFY(copyTheme(QFINDTESTDATA("xmpp-testtheme"), themeDir, "xmpp-testtheme"));
    }

    void cleanupTestCase()
    {
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        const QString themePath = dataPath + QLatin1String("/emoticons/");
        QDir themeDir(themePath);
        QVERIFY(themeDir.removeRecursively());
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

        QStringList inputFileNames = testCasesDir.entryList(QStringList(QLatin1String("*.input")));
        Q_FOREACH (const QString &fileName, inputFileNames) {
            QString outputFileName = fileName;
            outputFileName.replace("input", "output");
            const QString baseName = fileName.section("-", 0, 0);
            QTest::newRow(qPrintable(fileName.left(fileName.lastIndexOf('.'))))
                << basePath + QString::fromLatin1("/") + fileName
                << basePath + QString::fromLatin1("/") + outputFileName
                << (baseName == QLatin1String("xmpp") ? "xmpp-testtheme" : default_theme)
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
            result.replace(themePath + themeName + '/', QString());

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

