/********************************************************************************
** Form generated from reading UI file 'AdminSettingUI.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADMINSETTINGUI_H
#define UI_ADMINSETTINGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AdminSettingUI
{
public:
    QWidget *layoutWidget_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_roiHeight_L;
    QSpacerItem *horizontalSpacer_4;
    QLineEdit *lineEdit_MaxMotionStroke;
    QLabel *label_3;
    QLabel *label_title;
    QPushButton *pushButton_return;
    QPushButton *pushButton_confirm;
    QWidget *layoutWidget_7;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_roiHeight_L_5;
    QSpacerItem *horizontalSpacer_8;
    QLineEdit *lineEdit_MaxCameraNum;
    QFrame *line;
    QWidget *layoutWidget_9;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_roiHeight_L_7;
    QSpacerItem *horizontalSpacer_10;
    QLineEdit *lineEdit_PixelsNumPerUnitLength;
    QWidget *layoutWidget_10;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_roiHeight_L_8;
    QSpacerItem *horizontalSpacer_11;
    QLineEdit *lineEdit_ImageOverlappingRate;
    QWidget *layoutWidget_6;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_roiHeight_L_3;
    QSpacerItem *horizontalSpacer_6;
    QLabel *label_roiHeight_L_9;
    QLineEdit *lineEdit_ImageSize_W;
    QSpacerItem *horizontalSpacer_7;
    QLabel *label_roiHeight_L_10;
    QLineEdit *lineEdit_ImageSize_H;

    void setupUi(QWidget *AdminSettingUI)
    {
        if (AdminSettingUI->objectName().isEmpty())
            AdminSettingUI->setObjectName(QStringLiteral("AdminSettingUI"));
        AdminSettingUI->resize(1366, 768);
        layoutWidget_4 = new QWidget(AdminSettingUI);
        layoutWidget_4->setObjectName(QStringLiteral("layoutWidget_4"));
        layoutWidget_4->setGeometry(QRect(70, 80, 361, 37));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget_4);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L = new QLabel(layoutWidget_4);
        label_roiHeight_L->setObjectName(QStringLiteral("label_roiHeight_L"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_roiHeight_L->sizePolicy().hasHeightForWidth());
        label_roiHeight_L->setSizePolicy(sizePolicy);
        label_roiHeight_L->setMinimumSize(QSize(140, 35));
        label_roiHeight_L->setMaximumSize(QSize(140, 35));
        QFont font;
        font.setPointSize(16);
        label_roiHeight_L->setFont(font);
        label_roiHeight_L->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(label_roiHeight_L);

        horizontalSpacer_4 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);

        lineEdit_MaxMotionStroke = new QLineEdit(layoutWidget_4);
        lineEdit_MaxMotionStroke->setObjectName(QStringLiteral("lineEdit_MaxMotionStroke"));
        sizePolicy.setHeightForWidth(lineEdit_MaxMotionStroke->sizePolicy().hasHeightForWidth());
        lineEdit_MaxMotionStroke->setSizePolicy(sizePolicy);
        lineEdit_MaxMotionStroke->setMinimumSize(QSize(180, 35));
        lineEdit_MaxMotionStroke->setMaximumSize(QSize(160, 35));
        lineEdit_MaxMotionStroke->setFont(font);
        lineEdit_MaxMotionStroke->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(lineEdit_MaxMotionStroke);

        label_3 = new QLabel(AdminSettingUI);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(0, -1, 1371, 41));
        label_3->setStyleSheet(QStringLiteral("background-color: rgb(240, 240, 250);"));
        label_title = new QLabel(AdminSettingUI);
        label_title->setObjectName(QStringLiteral("label_title"));
        label_title->setGeometry(QRect(560, 0, 361, 41));
        sizePolicy.setHeightForWidth(label_title->sizePolicy().hasHeightForWidth());
        label_title->setSizePolicy(sizePolicy);
        label_title->setMinimumSize(QSize(60, 30));
        QFont font1;
        font1.setFamily(QStringLiteral("SimSun-ExtB"));
        font1.setPointSize(22);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(12);
        label_title->setFont(font1);
        label_title->setStyleSheet(QStringLiteral("font: 100 22pt \"SimSun-ExtB\";"));
        label_title->setAlignment(Qt::AlignCenter);
        pushButton_return = new QPushButton(AdminSettingUI);
        pushButton_return->setObjectName(QStringLiteral("pushButton_return"));
        pushButton_return->setGeometry(QRect(780, 690, 112, 40));
        sizePolicy.setHeightForWidth(pushButton_return->sizePolicy().hasHeightForWidth());
        pushButton_return->setSizePolicy(sizePolicy);
        pushButton_return->setFont(font);
        pushButton_confirm = new QPushButton(AdminSettingUI);
        pushButton_confirm->setObjectName(QStringLiteral("pushButton_confirm"));
        pushButton_confirm->setGeometry(QRect(490, 690, 112, 40));
        sizePolicy.setHeightForWidth(pushButton_confirm->sizePolicy().hasHeightForWidth());
        pushButton_confirm->setSizePolicy(sizePolicy);
        pushButton_confirm->setFont(font);
        layoutWidget_7 = new QWidget(AdminSettingUI);
        layoutWidget_7->setObjectName(QStringLiteral("layoutWidget_7"));
        layoutWidget_7->setGeometry(QRect(70, 150, 361, 37));
        horizontalLayout_8 = new QHBoxLayout(layoutWidget_7);
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_5 = new QLabel(layoutWidget_7);
        label_roiHeight_L_5->setObjectName(QStringLiteral("label_roiHeight_L_5"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_5->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_5->setSizePolicy(sizePolicy);
        label_roiHeight_L_5->setMinimumSize(QSize(140, 35));
        label_roiHeight_L_5->setMaximumSize(QSize(140, 35));
        label_roiHeight_L_5->setFont(font);
        label_roiHeight_L_5->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(label_roiHeight_L_5);

        horizontalSpacer_8 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_8);

        lineEdit_MaxCameraNum = new QLineEdit(layoutWidget_7);
        lineEdit_MaxCameraNum->setObjectName(QStringLiteral("lineEdit_MaxCameraNum"));
        sizePolicy.setHeightForWidth(lineEdit_MaxCameraNum->sizePolicy().hasHeightForWidth());
        lineEdit_MaxCameraNum->setSizePolicy(sizePolicy);
        lineEdit_MaxCameraNum->setMinimumSize(QSize(180, 35));
        lineEdit_MaxCameraNum->setMaximumSize(QSize(160, 35));
        lineEdit_MaxCameraNum->setFont(font);
        lineEdit_MaxCameraNum->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(lineEdit_MaxCameraNum);

        line = new QFrame(AdminSettingUI);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(0, 30, 1371, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        layoutWidget_9 = new QWidget(AdminSettingUI);
        layoutWidget_9->setObjectName(QStringLiteral("layoutWidget_9"));
        layoutWidget_9->setGeometry(QRect(70, 220, 361, 37));
        horizontalLayout_10 = new QHBoxLayout(layoutWidget_9);
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalLayout_10->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_7 = new QLabel(layoutWidget_9);
        label_roiHeight_L_7->setObjectName(QStringLiteral("label_roiHeight_L_7"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_7->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_7->setSizePolicy(sizePolicy);
        label_roiHeight_L_7->setMinimumSize(QSize(140, 35));
        label_roiHeight_L_7->setMaximumSize(QSize(140, 35));
        label_roiHeight_L_7->setFont(font);
        label_roiHeight_L_7->setAlignment(Qt::AlignCenter);

        horizontalLayout_10->addWidget(label_roiHeight_L_7);

        horizontalSpacer_10 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_10);

        lineEdit_PixelsNumPerUnitLength = new QLineEdit(layoutWidget_9);
        lineEdit_PixelsNumPerUnitLength->setObjectName(QStringLiteral("lineEdit_PixelsNumPerUnitLength"));
        sizePolicy.setHeightForWidth(lineEdit_PixelsNumPerUnitLength->sizePolicy().hasHeightForWidth());
        lineEdit_PixelsNumPerUnitLength->setSizePolicy(sizePolicy);
        lineEdit_PixelsNumPerUnitLength->setMinimumSize(QSize(180, 35));
        lineEdit_PixelsNumPerUnitLength->setMaximumSize(QSize(160, 35));
        lineEdit_PixelsNumPerUnitLength->setFont(font);
        lineEdit_PixelsNumPerUnitLength->setAlignment(Qt::AlignCenter);

        horizontalLayout_10->addWidget(lineEdit_PixelsNumPerUnitLength);

        layoutWidget_10 = new QWidget(AdminSettingUI);
        layoutWidget_10->setObjectName(QStringLiteral("layoutWidget_10"));
        layoutWidget_10->setGeometry(QRect(70, 290, 361, 37));
        horizontalLayout_11 = new QHBoxLayout(layoutWidget_10);
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        horizontalLayout_11->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_8 = new QLabel(layoutWidget_10);
        label_roiHeight_L_8->setObjectName(QStringLiteral("label_roiHeight_L_8"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_8->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_8->setSizePolicy(sizePolicy);
        label_roiHeight_L_8->setMinimumSize(QSize(140, 35));
        label_roiHeight_L_8->setMaximumSize(QSize(140, 35));
        label_roiHeight_L_8->setFont(font);
        label_roiHeight_L_8->setAlignment(Qt::AlignCenter);

        horizontalLayout_11->addWidget(label_roiHeight_L_8);

        horizontalSpacer_11 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_11);

        lineEdit_ImageOverlappingRate = new QLineEdit(layoutWidget_10);
        lineEdit_ImageOverlappingRate->setObjectName(QStringLiteral("lineEdit_ImageOverlappingRate"));
        sizePolicy.setHeightForWidth(lineEdit_ImageOverlappingRate->sizePolicy().hasHeightForWidth());
        lineEdit_ImageOverlappingRate->setSizePolicy(sizePolicy);
        lineEdit_ImageOverlappingRate->setMinimumSize(QSize(180, 35));
        lineEdit_ImageOverlappingRate->setMaximumSize(QSize(160, 35));
        lineEdit_ImageOverlappingRate->setFont(font);
        lineEdit_ImageOverlappingRate->setAlignment(Qt::AlignCenter);

        horizontalLayout_11->addWidget(lineEdit_ImageOverlappingRate);

        layoutWidget_6 = new QWidget(AdminSettingUI);
        layoutWidget_6->setObjectName(QStringLiteral("layoutWidget_6"));
        layoutWidget_6->setGeometry(QRect(70, 360, 371, 37));
        horizontalLayout_6 = new QHBoxLayout(layoutWidget_6);
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_3 = new QLabel(layoutWidget_6);
        label_roiHeight_L_3->setObjectName(QStringLiteral("label_roiHeight_L_3"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_3->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_3->setSizePolicy(sizePolicy);
        label_roiHeight_L_3->setMinimumSize(QSize(140, 35));
        label_roiHeight_L_3->setMaximumSize(QSize(140, 35));
        label_roiHeight_L_3->setFont(font);
        label_roiHeight_L_3->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(label_roiHeight_L_3);

        horizontalSpacer_6 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_6);

        label_roiHeight_L_9 = new QLabel(layoutWidget_6);
        label_roiHeight_L_9->setObjectName(QStringLiteral("label_roiHeight_L_9"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_9->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_9->setSizePolicy(sizePolicy);
        label_roiHeight_L_9->setMinimumSize(QSize(25, 35));
        label_roiHeight_L_9->setMaximumSize(QSize(25, 35));
        QFont font2;
        font2.setPointSize(15);
        label_roiHeight_L_9->setFont(font2);
        label_roiHeight_L_9->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(label_roiHeight_L_9);

        lineEdit_ImageSize_W = new QLineEdit(layoutWidget_6);
        lineEdit_ImageSize_W->setObjectName(QStringLiteral("lineEdit_ImageSize_W"));
        sizePolicy.setHeightForWidth(lineEdit_ImageSize_W->sizePolicy().hasHeightForWidth());
        lineEdit_ImageSize_W->setSizePolicy(sizePolicy);
        lineEdit_ImageSize_W->setMinimumSize(QSize(65, 35));
        lineEdit_ImageSize_W->setMaximumSize(QSize(60, 35));
        lineEdit_ImageSize_W->setFont(font);
        lineEdit_ImageSize_W->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(lineEdit_ImageSize_W);

        horizontalSpacer_7 = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_7);

        label_roiHeight_L_10 = new QLabel(layoutWidget_6);
        label_roiHeight_L_10->setObjectName(QStringLiteral("label_roiHeight_L_10"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_10->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_10->setSizePolicy(sizePolicy);
        label_roiHeight_L_10->setMinimumSize(QSize(25, 35));
        label_roiHeight_L_10->setMaximumSize(QSize(25, 35));
        label_roiHeight_L_10->setFont(font2);
        label_roiHeight_L_10->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(label_roiHeight_L_10);

        lineEdit_ImageSize_H = new QLineEdit(layoutWidget_6);
        lineEdit_ImageSize_H->setObjectName(QStringLiteral("lineEdit_ImageSize_H"));
        sizePolicy.setHeightForWidth(lineEdit_ImageSize_H->sizePolicy().hasHeightForWidth());
        lineEdit_ImageSize_H->setSizePolicy(sizePolicy);
        lineEdit_ImageSize_H->setMinimumSize(QSize(65, 35));
        lineEdit_ImageSize_H->setMaximumSize(QSize(60, 35));
        lineEdit_ImageSize_H->setFont(font);
        lineEdit_ImageSize_H->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(lineEdit_ImageSize_H);


        retranslateUi(AdminSettingUI);

        QMetaObject::connectSlotsByName(AdminSettingUI);
    } // setupUi

    void retranslateUi(QWidget *AdminSettingUI)
    {
        AdminSettingUI->setWindowTitle(QApplication::translate("AdminSettingUI", "AdminSettingUI", nullptr));
        label_roiHeight_L->setText(QApplication::translate("AdminSettingUI", "\346\234\200\345\244\247\350\277\220\345\212\250\350\241\214\347\250\213", nullptr));
        label_3->setText(QString());
        label_title->setText(QApplication::translate("AdminSettingUI", "PCB\346\243\200\346\265\213\347\263\273\347\273\237 - \347\263\273\347\273\237\350\256\276\347\275\256", nullptr));
        pushButton_return->setText(QApplication::translate("AdminSettingUI", "\350\277\224\345\233\236", nullptr));
        pushButton_confirm->setText(QApplication::translate("AdminSettingUI", "\347\241\256\345\256\232", nullptr));
        label_roiHeight_L_5->setText(QApplication::translate("AdminSettingUI", "\347\233\270\346\234\272\346\200\273\346\225\260", nullptr));
        label_roiHeight_L_7->setText(QApplication::translate("AdminSettingUI", "\346\257\217\346\257\253\347\261\263\345\203\217\347\264\240\346\225\260", nullptr));
        label_roiHeight_L_8->setText(QApplication::translate("AdminSettingUI", "\345\210\206\345\233\276\351\207\215\345\217\240\347\216\207", nullptr));
        label_roiHeight_L_3->setText(QApplication::translate("AdminSettingUI", "\345\210\206\345\233\276\345\260\272\345\257\270(pix)", nullptr));
        label_roiHeight_L_9->setText(QApplication::translate("AdminSettingUI", "\345\256\275", nullptr));
        lineEdit_ImageSize_W->setText(QString());
        label_roiHeight_L_10->setText(QApplication::translate("AdminSettingUI", "\351\253\230", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AdminSettingUI: public Ui_AdminSettingUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADMINSETTINGUI_H
