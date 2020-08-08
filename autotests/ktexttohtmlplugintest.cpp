/*
    SPDX-FileCopyrightText: 2014 Daniel Vrátil <dvratil@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "autotestbase.h"

#include <QObject>
#include <QTest>
#include <QStandardPaths>

#include "../src/integrationplugin/ktexttohtml.h"
#include <kemoticons.h>

class KTextToHTMLPluginTest : public QObject
{
    Q_OBJECT

private:
    QString htmlForSmiley(const QString &emoticon, const QString &name) const;

private Q_SLOTS:
    void initTestCase();
    void parseEmoticonsTest_data();
    void parseEmoticonsTest();

private:
    QString mEmoticonsThemePath;
};

QTEST_MAIN(KTextToHTMLPluginTest)

void KTextToHTMLPluginTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    QString destThemePath = dataPath + QLatin1String("/emoticons/");
    QVERIFY(QDir().mkpath(destThemePath));
    mEmoticonsThemePath = destThemePath + QStringLiteral("default-testtheme");
    if (QFileInfo::exists(mEmoticonsThemePath)) {
        QVERIFY(QDir(mEmoticonsThemePath).removeRecursively());
    }
    QDir themeDir(destThemePath);
    QVERIFY(copyTheme(QFINDTESTDATA("default-testtheme"), themeDir, QStringLiteral("default-testtheme")));

    KEmoticons::setTheme(QStringLiteral("default-testtheme"));
    QVERIFY(!mEmoticonsThemePath.isEmpty());
}

QString KTextToHTMLPluginTest::htmlForSmiley(const QString &emoticon, const QString &name) const
{
    return QStringLiteral("<img align=\"center\" title=\"%1\" alt=\"%1\" src=\"file://%2/%3.png\" width=\"22\" height=\"22\" />")
                .arg(emoticon,
                     mEmoticonsThemePath,
                     name);
}


void KTextToHTMLPluginTest::parseEmoticonsTest_data()
{

    QTest::addColumn<QString>("input");
    QTest::addColumn<bool>("strict");
    QTest::addColumn<QStringList>("exclude");
    QTest::addColumn<QString>("expected");

    QTest::newRow("simple")
        << "Hello :-)"
        << false << QStringList()
        << QStringLiteral("Hello %1").arg(htmlForSmiley(QStringLiteral(":-)"), QStringLiteral("smile")));

    QTest::newRow("between strings")
        << "Hello :-) How are you?"
        << false << QStringList()
        << QStringLiteral("Hello %1 How are you?").arg(htmlForSmiley(QStringLiteral(":-)"), QStringLiteral("smile")));

    QTest::newRow("excluded")
        << "Bye :-("
        << false << (QStringList() << QStringLiteral(":-("))
        << "Bye :-(";

    QTest::newRow("don't mix in HTML")
        << "<b>:(</b>"
        << false << QStringList()
        << QStringLiteral("<b>%1</b>").arg(htmlForSmiley(QStringLiteral(":("), QStringLiteral("sad")));

    QTest::newRow("strict parsing of smileys without space")
        << "Very happy! :-):-)"
        << true << QStringList()
        << "Very happy! :-):-)";

    QTest::newRow("nonstrict parsing of smileys without space")
        << "Very happy! :-):-)"
        << false << QStringList()
        << QStringLiteral("Very happy! %1%1").arg(htmlForSmiley(QStringLiteral(":-)"), QStringLiteral("smile")));

    QTest::newRow("smiley in HTML")
        << "<img src=\"...\" title=\":-)\" />"
        << false << QStringList()
        << "<img src=\"...\" title=\":-)\" />";
}


void KTextToHTMLPluginTest::parseEmoticonsTest()
{
    QFETCH(QString, input);
    QFETCH(bool, strict);
    QFETCH(QStringList, exclude);
    QFETCH(QString, expected);

    KTextToHTMLEmoticons emoticons;
    const QString actual = emoticons.parseEmoticons(input, strict, exclude);

    QCOMPARE(actual, expected);
}

#include "ktexttohtmlplugintest.moc"
