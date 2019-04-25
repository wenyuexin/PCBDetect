/********************************************************************************
** Form generated from reading UI file 'SerialNumberUI.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERIALNUMBERUI_H
#define UI_SERIALNUMBERUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SerialNumberUI
{
public:
    QGraphicsView *graphicsView;
    QLineEdit *lineEdit_roi_tl_x;
    QLabel *label;
    QLineEdit *lineEdit_roi_tl_y;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *lineEdit_roi_br_x;
    QLineEdit *lineEdit_roi_br_y;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QPushButton *pushButton_getROI;
    QFrame *line_2;
    QLabel *label_9;
    QLineEdit *lineEdit_serialNum;
    QPushButton *pushButton_recognize;
    QPushButton *pushButton_return;
    QFrame *line_3;
    QFrame *line_4;
    QLabel *label_title;
    QLabel *label_10;
    QPushButton *pushButton_confirm;
    QCheckBox *checkBox_roi_tl;
    QCheckBox *checkBox_roi_br;

    void setupUi(QWidget *SerialNumberUI)
    {
        if (SerialNumberUI->objectName().isEmpty())
            SerialNumberUI->setObjectName(QStringLiteral("SerialNumberUI"));
        SerialNumberUI->resize(1440, 900);
        graphicsView = new QGraphicsView(SerialNumberUI);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setGeometry(QRect(0, 40, 1101, 861));
        lineEdit_roi_tl_x = new QLineEdit(SerialNumberUI);
        lineEdit_roi_tl_x->setObjectName(QStringLiteral("lineEdit_roi_tl_x"));
        lineEdit_roi_tl_x->setGeometry(QRect(1250, 180, 65, 35));
        QFont font;
        font.setPointSize(16);
        lineEdit_roi_tl_x->setFont(font);
        lineEdit_roi_tl_x->setAlignment(Qt::AlignCenter);
        label = new QLabel(SerialNumberUI);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(1180, 100, 181, 35));
        QFont font1;
        font1.setPointSize(20);
        font1.setBold(false);
        font1.setWeight(50);
        label->setFont(font1);
        label->setAlignment(Qt::AlignCenter);
        lineEdit_roi_tl_y = new QLineEdit(SerialNumberUI);
        lineEdit_roi_tl_y->setObjectName(QStringLiteral("lineEdit_roi_tl_y"));
        lineEdit_roi_tl_y->setGeometry(QRect(1350, 180, 65, 35));
        lineEdit_roi_tl_y->setFont(font);
        lineEdit_roi_tl_y->setAlignment(Qt::AlignCenter);
        label_3 = new QLabel(SerialNumberUI);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(1140, 180, 71, 35));
        QFont font2;
        font2.setPointSize(18);
        label_3->setFont(font2);
        label_4 = new QLabel(SerialNumberUI);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(1140, 220, 71, 35));
        label_4->setFont(font2);
        lineEdit_roi_br_x = new QLineEdit(SerialNumberUI);
        lineEdit_roi_br_x->setObjectName(QStringLiteral("lineEdit_roi_br_x"));
        lineEdit_roi_br_x->setGeometry(QRect(1250, 220, 65, 35));
        lineEdit_roi_br_x->setFont(font);
        lineEdit_roi_br_x->setAlignment(Qt::AlignCenter);
        lineEdit_roi_br_y = new QLineEdit(SerialNumberUI);
        lineEdit_roi_br_y->setObjectName(QStringLiteral("lineEdit_roi_br_y"));
        lineEdit_roi_br_y->setGeometry(QRect(1350, 220, 65, 35));
        lineEdit_roi_br_y->setFont(font);
        lineEdit_roi_br_y->setAlignment(Qt::AlignCenter);
        label_5 = new QLabel(SerialNumberUI);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(1220, 180, 30, 35));
        label_5->setFont(font);
        label_5->setAlignment(Qt::AlignCenter);
        label_6 = new QLabel(SerialNumberUI);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(1320, 180, 30, 35));
        label_6->setFont(font);
        label_6->setAlignment(Qt::AlignCenter);
        label_7 = new QLabel(SerialNumberUI);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(1220, 220, 30, 35));
        label_7->setFont(font);
        label_7->setAlignment(Qt::AlignCenter);
        label_8 = new QLabel(SerialNumberUI);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(1320, 220, 30, 35));
        label_8->setFont(font);
        label_8->setAlignment(Qt::AlignCenter);
        pushButton_getROI = new QPushButton(SerialNumberUI);
        pushButton_getROI->setObjectName(QStringLiteral("pushButton_getROI"));
        pushButton_getROI->setGeometry(QRect(1140, 300, 121, 40));
        pushButton_getROI->setFont(font2);
        line_2 = new QFrame(SerialNumberUI);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setGeometry(QRect(1120, 420, 301, 20));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        label_9 = new QLabel(SerialNumberUI);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(1190, 470, 131, 35));
        label_9->setFont(font1);
        label_9->setAlignment(Qt::AlignCenter);
        lineEdit_serialNum = new QLineEdit(SerialNumberUI);
        lineEdit_serialNum->setObjectName(QStringLiteral("lineEdit_serialNum"));
        lineEdit_serialNum->setGeometry(QRect(1130, 530, 201, 40));
        lineEdit_serialNum->setFont(font);
        lineEdit_serialNum->setAlignment(Qt::AlignCenter);
        pushButton_recognize = new QPushButton(SerialNumberUI);
        pushButton_recognize->setObjectName(QStringLiteral("pushButton_recognize"));
        pushButton_recognize->setGeometry(QRect(1290, 300, 121, 40));
        pushButton_recognize->setFont(font2);
        pushButton_return = new QPushButton(SerialNumberUI);
        pushButton_return->setObjectName(QStringLiteral("pushButton_return"));
        pushButton_return->setGeometry(QRect(1200, 710, 121, 40));
        pushButton_return->setFont(font2);
        line_3 = new QFrame(SerialNumberUI);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setGeometry(QRect(1120, 660, 301, 20));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        line_4 = new QFrame(SerialNumberUI);
        line_4->setObjectName(QStringLiteral("line_4"));
        line_4->setGeometry(QRect(0, 30, 1441, 20));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);
        label_title = new QLabel(SerialNumberUI);
        label_title->setObjectName(QStringLiteral("label_title"));
        label_title->setGeometry(QRect(520, 1, 411, 41));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_title->sizePolicy().hasHeightForWidth());
        label_title->setSizePolicy(sizePolicy);
        label_title->setMinimumSize(QSize(60, 30));
        QFont font3;
        font3.setFamily(QStringLiteral("SimSun-ExtB"));
        font3.setPointSize(22);
        font3.setBold(false);
        font3.setItalic(false);
        font3.setWeight(12);
        label_title->setFont(font3);
        label_title->setStyleSheet(QStringLiteral("font: 100 22pt \"SimSun-ExtB\";"));
        label_title->setAlignment(Qt::AlignCenter);
        label_10 = new QLabel(SerialNumberUI);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(0, 0, 1441, 41));
        label_10->setStyleSheet(QStringLiteral("background-color: rgb(240, 242, 252);"));
        pushButton_confirm = new QPushButton(SerialNumberUI);
        pushButton_confirm->setObjectName(QStringLiteral("pushButton_confirm"));
        pushButton_confirm->setGeometry(QRect(1340, 530, 81, 40));
        pushButton_confirm->setFont(font2);
        checkBox_roi_tl = new QCheckBox(SerialNumberUI);
        checkBox_roi_tl->setObjectName(QStringLiteral("checkBox_roi_tl"));
        checkBox_roi_tl->setGeometry(QRect(1120, 190, 16, 16));
        checkBox_roi_br = new QCheckBox(SerialNumberUI);
        checkBox_roi_br->setObjectName(QStringLiteral("checkBox_roi_br"));
        checkBox_roi_br->setGeometry(QRect(1120, 230, 16, 16));
        label_10->raise();
        graphicsView->raise();
        lineEdit_roi_tl_x->raise();
        label->raise();
        lineEdit_roi_tl_y->raise();
        label_3->raise();
        label_4->raise();
        lineEdit_roi_br_x->raise();
        lineEdit_roi_br_y->raise();
        label_5->raise();
        label_6->raise();
        label_7->raise();
        label_8->raise();
        pushButton_getROI->raise();
        line_2->raise();
        label_9->raise();
        lineEdit_serialNum->raise();
        pushButton_recognize->raise();
        pushButton_return->raise();
        line_3->raise();
        line_4->raise();
        label_title->raise();
        pushButton_confirm->raise();
        checkBox_roi_tl->raise();
        checkBox_roi_br->raise();

        retranslateUi(SerialNumberUI);

        QMetaObject::connectSlotsByName(SerialNumberUI);
    } // setupUi

    void retranslateUi(QWidget *SerialNumberUI)
    {
        SerialNumberUI->setWindowTitle(QApplication::translate("SerialNumberUI", "SerialNumberUI", nullptr));
        label->setText(QApplication::translate("SerialNumberUI", "\345\272\217\345\217\267\345\214\272\345\237\237\345\235\220\346\240\207", nullptr));
        label_3->setText(QApplication::translate("SerialNumberUI", "\345\267\246\344\270\212\350\247\222", nullptr));
        label_4->setText(QApplication::translate("SerialNumberUI", "\345\217\263\344\270\213\350\247\222", nullptr));
        lineEdit_roi_br_x->setText(QString());
        label_5->setText(QApplication::translate("SerialNumberUI", "X", nullptr));
        label_6->setText(QApplication::translate("SerialNumberUI", "Y", nullptr));
        label_7->setText(QApplication::translate("SerialNumberUI", "X", nullptr));
        label_8->setText(QApplication::translate("SerialNumberUI", "Y", nullptr));
        pushButton_getROI->setText(QApplication::translate("SerialNumberUI", "\347\241\256\350\256\244\345\214\272\345\237\237", nullptr));
        label_9->setText(QApplication::translate("SerialNumberUI", "\344\272\247\345\223\201\345\272\217\345\217\267", nullptr));
        pushButton_recognize->setText(QApplication::translate("SerialNumberUI", "\350\257\206\345\210\253", nullptr));
        pushButton_return->setText(QApplication::translate("SerialNumberUI", "\350\277\224\345\233\236", nullptr));
        label_title->setText(QApplication::translate("SerialNumberUI", "PCB\346\243\200\346\265\213\347\263\273\347\273\237 - \344\272\247\345\223\201\345\272\217\345\217\267\350\257\206\345\210\253", nullptr));
        label_10->setText(QString());
        pushButton_confirm->setText(QApplication::translate("SerialNumberUI", "\347\241\256\345\256\232", nullptr));
        checkBox_roi_tl->setText(QString());
        checkBox_roi_br->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SerialNumberUI: public Ui_SerialNumberUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERIALNUMBERUI_H
