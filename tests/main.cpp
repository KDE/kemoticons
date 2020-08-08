/*
    SPDX-FileCopyrightText: 2008 Carlo Segato <brandon.ml@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <kemoticons.h>

#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QString>

class KEmoTest : public QWidget
{
    Q_OBJECT
public:
    KEmoTest();

public Q_SLOTS:
    void updateEmoticons();
    void changeTheme(const QString &emoticonTheme);

private:
    QLineEdit *lineEdit;
    QLabel *label;
    KEmoticons emoticons;
    KEmoticonsTheme emoticonTheme;
    QComboBox *comboBox;
};

KEmoTest::KEmoTest()
{
    lineEdit = new QLineEdit;
    label = new QLabel;
    QLabel *explanation = new QLabel;
    explanation->setText(QStringLiteral("Please enter text with emoticons. They will be parsed, "
                         "except <b>:-)</b> and <b>:)</b> which are excluded. "
                         "Emoticon theme can be chosen from the combo box."));
    explanation->setWordWrap(true);
    comboBox = new QComboBox;

    emoticonTheme = emoticons.theme();
    // Theme list is repeating three times the same two themes: "Oxygen" and "Glass"
    qDebug() << "Theme list: " << emoticons.themeList();
    // Theme name is empty!!
    qDebug() << "Theme name: " << emoticonTheme.themeName();

    comboBox->addItems(emoticons.themeList());
    comboBox->setCurrentIndex(emoticons.themeList().indexOf(emoticonTheme.themeName()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(explanation);
    layout->addWidget(lineEdit);
    layout->addWidget(comboBox);
    layout->addWidget(label);
    setLayout(layout);

    connect(lineEdit, &QLineEdit::textChanged, this, &KEmoTest::updateEmoticons);
    connect(comboBox, SIGNAL(activated(QString)), this, SLOT(changeTheme(QString)));
}

void KEmoTest::updateEmoticons()
{
    QStringList excludedEmoticons;
    excludedEmoticons << QStringLiteral(":)") << QStringLiteral(":-)");
    label->setText(emoticonTheme.parseEmoticons(lineEdit->text().toHtmlEscaped(), KEmoticonsTheme::DefaultParse, excludedEmoticons));
}

void KEmoTest::changeTheme(const QString &theme)
{
    emoticonTheme = emoticons.theme(theme);
    updateEmoticons();
}

int main(int argc, char **argv)
{
    QApplication::setApplicationName(QStringLiteral("kemoticonstest"));
    QApplication app(argc, argv);

    KEmoTest kt;

    kt.show();

    return app.exec();
}

#include "main.moc"
