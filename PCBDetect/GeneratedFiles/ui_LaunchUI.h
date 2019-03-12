/********************************************************************************
** Form generated from reading UI file 'LaunchUI.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LAUNCHUI_H
#define UI_LAUNCHUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LaunchUI
{
public:
    QLabel *label_background;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_status;

    void setupUi(QWidget *LaunchUI)
    {
        if (LaunchUI->objectName().isEmpty())
            LaunchUI->setObjectName(QStringLiteral("LaunchUI"));
        LaunchUI->resize(1366, 768);
        label_background = new QLabel(LaunchUI);
        label_background->setObjectName(QStringLiteral("label_background"));
        label_background->setGeometry(QRect(0, 0, 1366, 768));
        label_background->setStyleSheet(QStringLiteral("border-image: url(:/PCBDetect/Icons/screen.png);"));
        label = new QLabel(LaunchUI);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(180, 169, 991, 131));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(76);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        label->setFont(font);
        label->setStyleSheet(QString::fromUtf8("font: 76pt \"\346\245\267\344\275\223\";\n"
"color: rgb(0, 0, 00);\n"
"background-color: rgba(250, 245, 240, 10);"));
        label->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(LaunchUI);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(400, 500, 591, 71));
        QFont font1;
        font1.setFamily(QStringLiteral("SimSun-ExtB"));
        font1.setPointSize(42);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(15);
        label_2->setFont(font1);
        label_2->setStyleSheet(QLatin1String("background-color: rgba(250, 245, 240, 0);\n"
"font: 120 42pt \"SimSun-ExtB\";\n"
"color: rgb(0, 0, 50);"));
        label_2->setAlignment(Qt::AlignCenter);
        label_status = new QLabel(LaunchUI);
        label_status->setObjectName(QStringLiteral("label_status"));
        label_status->setGeometry(QRect(330, 590, 771, 51));
        QFont font2;
        font2.setFamily(QStringLiteral("SimSun-ExtB FB"));
        font2.setPointSize(35);
        font2.setBold(false);
        font2.setItalic(false);
        font2.setWeight(12);
        label_status->setFont(font2);
        label_status->setStyleSheet(QLatin1String("background-color: rgba(255, 245, 240, 0);\n"
"font: 100 35pt \"SimSun-ExtB FB\";\n"
"color: rgb(0, 0, 50);"));
        label_status->setAlignment(Qt::AlignCenter);

        retranslateUi(LaunchUI);

        QMetaObject::connectSlotsByName(LaunchUI);
    } // setupUi

    void retranslateUi(QWidget *LaunchUI)
    {
        LaunchUI->setWindowTitle(QApplication::translate("LaunchUI", "LaunchUI", nullptr));
        label_background->setText(QString());
        label->setText(QApplication::translate("LaunchUI", "\346\254\242\350\277\216\344\275\277\347\224\250PCB\346\243\200\346\265\213\347\263\273\347\273\237", nullptr));
        label_2->setText(QApplication::translate("LaunchUI", "\345\210\235\345\247\213\345\214\226\350\277\233\350\241\214\344\270\255,\350\257\267\347\250\215\345\220\216", nullptr));
        label_status->setText(QApplication::translate("LaunchUI", "00:00:00 \345\210\235\345\247\213\345\214\226\350\277\233\350\241\214\344\270\255 ...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LaunchUI: public Ui_LaunchUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LAUNCHUI_H
