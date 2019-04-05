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
    QWidget *layoutWidget_8;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_roiHeight_L_6;
    QSpacerItem *horizontalSpacer_9;
    QLineEdit *lineEdit_MaxPhotographingNum;
    QFrame *line;

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

        layoutWidget_8 = new QWidget(AdminSettingUI);
        layoutWidget_8->setObjectName(QStringLiteral("layoutWidget_8"));
        layoutWidget_8->setGeometry(QRect(70, 220, 361, 37));
        horizontalLayout_9 = new QHBoxLayout(layoutWidget_8);
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        horizontalLayout_9->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_6 = new QLabel(layoutWidget_8);
        label_roiHeight_L_6->setObjectName(QStringLiteral("label_roiHeight_L_6"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_6->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_6->setSizePolicy(sizePolicy);
        label_roiHeight_L_6->setMinimumSize(QSize(140, 35));
        label_roiHeight_L_6->setMaximumSize(QSize(140, 35));
        label_roiHeight_L_6->setFont(font);
        label_roiHeight_L_6->setAlignment(Qt::AlignCenter);

        horizontalLayout_9->addWidget(label_roiHeight_L_6);

        horizontalSpacer_9 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_9);

        lineEdit_MaxPhotographingNum = new QLineEdit(layoutWidget_8);
        lineEdit_MaxPhotographingNum->setObjectName(QStringLiteral("lineEdit_MaxPhotographingNum"));
        sizePolicy.setHeightForWidth(lineEdit_MaxPhotographingNum->sizePolicy().hasHeightForWidth());
        lineEdit_MaxPhotographingNum->setSizePolicy(sizePolicy);
        lineEdit_MaxPhotographingNum->setMinimumSize(QSize(180, 35));
        lineEdit_MaxPhotographingNum->setMaximumSize(QSize(160, 35));
        lineEdit_MaxPhotographingNum->setFont(font);
        lineEdit_MaxPhotographingNum->setAlignment(Qt::AlignCenter);

        horizontalLayout_9->addWidget(lineEdit_MaxPhotographingNum);

        line = new QFrame(AdminSettingUI);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(0, 30, 1371, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

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
        label_roiHeight_L_5->setText(QApplication::translate("AdminSettingUI", "\347\233\270\346\234\272\344\270\252\346\225\260", nullptr));
        label_roiHeight_L_6->setText(QApplication::translate("AdminSettingUI", "\346\234\200\345\244\247\346\213\215\347\205\247\346\254\241\346\225\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AdminSettingUI: public Ui_AdminSettingUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADMINSETTINGUI_H
