/********************************************************************************
** Form generated from reading UI file 'PassWordUI.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PASSWORDUI_H
#define UI_PASSWORDUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PassWordUI
{
public:
    QPushButton *pushButton_confirm;
    QPushButton *pushButton_cancel;
    QLabel *label;
    QLineEdit *lineEdit_pswd;

    void setupUi(QWidget *PassWordUI)
    {
        if (PassWordUI->objectName().isEmpty())
            PassWordUI->setObjectName(QStringLiteral("PassWordUI"));
        PassWordUI->resize(360, 168);
        pushButton_confirm = new QPushButton(PassWordUI);
        pushButton_confirm->setObjectName(QStringLiteral("pushButton_confirm"));
        pushButton_confirm->setGeometry(QRect(90, 100, 75, 33));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font.setPointSize(17);
        font.setBold(false);
        font.setWeight(50);
        pushButton_confirm->setFont(font);
        pushButton_cancel = new QPushButton(PassWordUI);
        pushButton_cancel->setObjectName(QStringLiteral("pushButton_cancel"));
        pushButton_cancel->setGeometry(QRect(200, 100, 75, 33));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font1.setPointSize(16);
        pushButton_cancel->setFont(font1);
        label = new QLabel(PassWordUI);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(40, 40, 61, 37));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font2.setPointSize(20);
        label->setFont(font2);
        lineEdit_pswd = new QLineEdit(PassWordUI);
        lineEdit_pswd->setObjectName(QStringLiteral("lineEdit_pswd"));
        lineEdit_pswd->setGeometry(QRect(100, 40, 201, 37));
        QFont font3;
        font3.setPointSize(9);
        lineEdit_pswd->setFont(font3);
        lineEdit_pswd->setAlignment(Qt::AlignCenter);

        retranslateUi(PassWordUI);

        QMetaObject::connectSlotsByName(PassWordUI);
    } // setupUi

    void retranslateUi(QWidget *PassWordUI)
    {
        PassWordUI->setWindowTitle(QApplication::translate("PassWordUI", "PassWordUI", nullptr));
        pushButton_confirm->setText(QApplication::translate("PassWordUI", "\347\241\256\345\256\232", nullptr));
        pushButton_cancel->setText(QApplication::translate("PassWordUI", "\345\217\226\346\266\210", nullptr));
        label->setText(QApplication::translate("PassWordUI", "<html><head/><body><p><span style=\" font-size:16pt;\">\345\257\206\347\240\201\357\274\232</span></p></body></html>", nullptr));
        lineEdit_pswd->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class PassWordUI: public Ui_PassWordUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PASSWORDUI_H
