/*
 * Copyright (C) 2014  Daniel Vrátil <dvratil@redhat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
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
    return QStringLiteral("<img align=\"center\" title=\"%1\" alt=\"%1\" src=\"%2/%3.png\" width=\"22\" height=\"22\" />")
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
