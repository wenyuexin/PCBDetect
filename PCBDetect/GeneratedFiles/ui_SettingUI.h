/********************************************************************************
** Form generated from reading UI file 'SettingUI.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGUI_H
#define UI_SETTINGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingUI
{
public:
    QLabel *label_title;
    QLabel *label_3;
    QFrame *line;
    QPushButton *pushButton_return;
    QPushButton *pushButton_confirm;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *label_path_L;
    QSpacerItem *horizontalSpacer;
    QLineEdit *lineEdit_SampleDirPath;
    QSpacerItem *horizontalSpacer_15;
    QPushButton *pushButton_SampleDirPath;
    QWidget *layoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_path_L_2;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *lineEdit_TemplDirPath;
    QSpacerItem *horizontalSpacer_16;
    QPushButton *pushButton_TemplDirPath;
    QWidget *layoutWidget_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_path_L_3;
    QSpacerItem *horizontalSpacer_3;
    QLineEdit *lineEdit_OutputDirPath;
    QSpacerItem *horizontalSpacer_17;
    QPushButton *pushButton_OutputDirPath;
    QWidget *layoutWidget_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_roiHeight_L;
    QSpacerItem *horizontalSpacer_4;
    QLineEdit *lineEdit_nCamera;
    QWidget *layoutWidget_5;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_roiHeight_L_2;
    QSpacerItem *horizontalSpacer_5;
    QLineEdit *lineEdit_nPhotographing;
    QWidget *layoutWidget_6;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_roiHeight_L_3;
    QSpacerItem *horizontalSpacer_6;
    QLabel *label_roiHeight_L_7;
    QLineEdit *lineEdit_nBasicUnitInRow;
    QSpacerItem *horizontalSpacer_7;
    QLabel *label_roiHeight_L_8;
    QLineEdit *lineEdit_nBasicUnitInCol;
    QWidget *layoutWidget_7;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_roiHeight_L_4;
    QSpacerItem *horizontalSpacer_8;
    QComboBox *comboBox_ImageFormat;
    QWidget *layoutWidget_8;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_roiHeight_L_5;
    QSpacerItem *horizontalSpacer_9;
    QLineEdit *lineEdit_ImageAspectRatio_W;
    QSpacerItem *horizontalSpacer_11;
    QLabel *label_roiHeight_L_6;
    QSpacerItem *horizontalSpacer_12;
    QLineEdit *lineEdit_ImageAspectRatio_H;
    QPushButton *pushButton_admin;

    void setupUi(QWidget *SettingUI)
    {
        if (SettingUI->objectName().isEmpty())
            SettingUI->setObjectName(QStringLiteral("SettingUI"));
        SettingUI->resize(1366, 768);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SettingUI->sizePolicy().hasHeightForWidth());
        SettingUI->setSizePolicy(sizePolicy);
        label_title = new QLabel(SettingUI);
        label_title->setObjectName(QStringLiteral("label_title"));
        label_title->setGeometry(QRect(560, 0, 271, 41));
        sizePolicy.setHeightForWidth(label_title->sizePolicy().hasHeightForWidth());
        label_title->setSizePolicy(sizePolicy);
        label_title->setMinimumSize(QSize(60, 30));
        QFont font;
        font.setFamily(QStringLiteral("SimSun-ExtB"));
        font.setPointSize(22);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(12);
        label_title->setFont(font);
        label_title->setStyleSheet(QStringLiteral("font: 100 22pt \"SimSun-ExtB\";"));
        label_title->setAlignment(Qt::AlignCenter);
        label_3 = new QLabel(SettingUI);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(0, -1, 1371, 41));
        label_3->setStyleSheet(QStringLiteral("background-color: rgb(240, 240, 250);"));
        line = new QFrame(SettingUI);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(0, 30, 1371, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        pushButton_return = new QPushButton(SettingUI);
        pushButton_return->setObjectName(QStringLiteral("pushButton_return"));
        pushButton_return->setGeometry(QRect(640, 710, 120, 40));
        sizePolicy.setHeightForWidth(pushButton_return->sizePolicy().hasHeightForWidth());
        pushButton_return->setSizePolicy(sizePolicy);
        QFont font1;
        font1.setPointSize(17);
        pushButton_return->setFont(font1);
        pushButton_confirm = new QPushButton(SettingUI);
        pushButton_confirm->setObjectName(QStringLiteral("pushButton_confirm"));
        pushButton_confirm->setGeometry(QRect(430, 710, 120, 40));
        sizePolicy.setHeightForWidth(pushButton_confirm->sizePolicy().hasHeightForWidth());
        pushButton_confirm->setSizePolicy(sizePolicy);
        pushButton_confirm->setFont(font1);
        layoutWidget = new QWidget(SettingUI);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(70, 90, 482, 37));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label_path_L = new QLabel(layoutWidget);
        label_path_L->setObjectName(QStringLiteral("label_path_L"));
        sizePolicy.setHeightForWidth(label_path_L->sizePolicy().hasHeightForWidth());
        label_path_L->setSizePolicy(sizePolicy);
        label_path_L->setMinimumSize(QSize(90, 35));
        label_path_L->setMaximumSize(QSize(90, 35));
        QFont font2;
        font2.setPointSize(16);
        label_path_L->setFont(font2);
        label_path_L->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label_path_L);

        horizontalSpacer = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        lineEdit_SampleDirPath = new QLineEdit(layoutWidget);
        lineEdit_SampleDirPath->setObjectName(QStringLiteral("lineEdit_SampleDirPath"));
        sizePolicy.setHeightForWidth(lineEdit_SampleDirPath->sizePolicy().hasHeightForWidth());
        lineEdit_SampleDirPath->setSizePolicy(sizePolicy);
        lineEdit_SampleDirPath->setMinimumSize(QSize(300, 35));
        lineEdit_SampleDirPath->setMaximumSize(QSize(160, 35));
        lineEdit_SampleDirPath->setFont(font2);
        lineEdit_SampleDirPath->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(lineEdit_SampleDirPath);

        horizontalSpacer_15 = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_15);

        pushButton_SampleDirPath = new QPushButton(layoutWidget);
        pushButton_SampleDirPath->setObjectName(QStringLiteral("pushButton_SampleDirPath"));
        sizePolicy.setHeightForWidth(pushButton_SampleDirPath->sizePolicy().hasHeightForWidth());
        pushButton_SampleDirPath->setSizePolicy(sizePolicy);
        pushButton_SampleDirPath->setMinimumSize(QSize(60, 35));
        pushButton_SampleDirPath->setMaximumSize(QSize(60, 35));
        pushButton_SampleDirPath->setFont(font2);

        horizontalLayout->addWidget(pushButton_SampleDirPath);

        layoutWidget_2 = new QWidget(SettingUI);
        layoutWidget_2->setObjectName(QStringLiteral("layoutWidget_2"));
        layoutWidget_2->setGeometry(QRect(70, 150, 482, 37));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_path_L_2 = new QLabel(layoutWidget_2);
        label_path_L_2->setObjectName(QStringLiteral("label_path_L_2"));
        sizePolicy.setHeightForWidth(label_path_L_2->sizePolicy().hasHeightForWidth());
        label_path_L_2->setSizePolicy(sizePolicy);
        label_path_L_2->setMinimumSize(QSize(90, 35));
        label_path_L_2->setMaximumSize(QSize(90, 35));
        label_path_L_2->setFont(font2);
        label_path_L_2->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(label_path_L_2);

        horizontalSpacer_2 = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        lineEdit_TemplDirPath = new QLineEdit(layoutWidget_2);
        lineEdit_TemplDirPath->setObjectName(QStringLiteral("lineEdit_TemplDirPath"));
        sizePolicy.setHeightForWidth(lineEdit_TemplDirPath->sizePolicy().hasHeightForWidth());
        lineEdit_TemplDirPath->setSizePolicy(sizePolicy);
        lineEdit_TemplDirPath->setMinimumSize(QSize(300, 35));
        lineEdit_TemplDirPath->setMaximumSize(QSize(160, 35));
        lineEdit_TemplDirPath->setFont(font2);
        lineEdit_TemplDirPath->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(lineEdit_TemplDirPath);

        horizontalSpacer_16 = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_16);

        pushButton_TemplDirPath = new QPushButton(layoutWidget_2);
        pushButton_TemplDirPath->setObjectName(QStringLiteral("pushButton_TemplDirPath"));
        sizePolicy.setHeightForWidth(pushButton_TemplDirPath->sizePolicy().hasHeightForWidth());
        pushButton_TemplDirPath->setSizePolicy(sizePolicy);
        pushButton_TemplDirPath->setMinimumSize(QSize(60, 35));
        pushButton_TemplDirPath->setMaximumSize(QSize(60, 35));
        pushButton_TemplDirPath->setFont(font2);

        horizontalLayout_2->addWidget(pushButton_TemplDirPath);

        layoutWidget_3 = new QWidget(SettingUI);
        layoutWidget_3->setObjectName(QStringLiteral("layoutWidget_3"));
        layoutWidget_3->setGeometry(QRect(70, 210, 482, 37));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget_3);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_path_L_3 = new QLabel(layoutWidget_3);
        label_path_L_3->setObjectName(QStringLiteral("label_path_L_3"));
        sizePolicy.setHeightForWidth(label_path_L_3->sizePolicy().hasHeightForWidth());
        label_path_L_3->setSizePolicy(sizePolicy);
        label_path_L_3->setMinimumSize(QSize(90, 35));
        label_path_L_3->setMaximumSize(QSize(90, 35));
        label_path_L_3->setFont(font2);
        label_path_L_3->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(label_path_L_3);

        horizontalSpacer_3 = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        lineEdit_OutputDirPath = new QLineEdit(layoutWidget_3);
        lineEdit_OutputDirPath->setObjectName(QStringLiteral("lineEdit_OutputDirPath"));
        sizePolicy.setHeightForWidth(lineEdit_OutputDirPath->sizePolicy().hasHeightForWidth());
        lineEdit_OutputDirPath->setSizePolicy(sizePolicy);
        lineEdit_OutputDirPath->setMinimumSize(QSize(300, 35));
        lineEdit_OutputDirPath->setMaximumSize(QSize(160, 35));
        lineEdit_OutputDirPath->setFont(font2);
        lineEdit_OutputDirPath->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(lineEdit_OutputDirPath);

        horizontalSpacer_17 = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_17);

        pushButton_OutputDirPath = new QPushButton(layoutWidget_3);
        pushButton_OutputDirPath->setObjectName(QStringLiteral("pushButton_OutputDirPath"));
        sizePolicy.setHeightForWidth(pushButton_OutputDirPath->sizePolicy().hasHeightForWidth());
        pushButton_OutputDirPath->setSizePolicy(sizePolicy);
        pushButton_OutputDirPath->setMinimumSize(QSize(60, 35));
        pushButton_OutputDirPath->setMaximumSize(QSize(60, 35));
        pushButton_OutputDirPath->setFont(font2);

        horizontalLayout_3->addWidget(pushButton_OutputDirPath);

        layoutWidget_4 = new QWidget(SettingUI);
        layoutWidget_4->setObjectName(QStringLiteral("layoutWidget_4"));
        layoutWidget_4->setGeometry(QRect(60, 280, 311, 37));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget_4);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L = new QLabel(layoutWidget_4);
        label_roiHeight_L->setObjectName(QStringLiteral("label_roiHeight_L"));
        sizePolicy.setHeightForWidth(label_roiHeight_L->sizePolicy().hasHeightForWidth());
        label_roiHeight_L->setSizePolicy(sizePolicy);
        label_roiHeight_L->setMinimumSize(QSize(90, 35));
        label_roiHeight_L->setMaximumSize(QSize(90, 35));
        label_roiHeight_L->setFont(font2);
        label_roiHeight_L->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(label_roiHeight_L);

        horizontalSpacer_4 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);

        lineEdit_nCamera = new QLineEdit(layoutWidget_4);
        lineEdit_nCamera->setObjectName(QStringLiteral("lineEdit_nCamera"));
        sizePolicy.setHeightForWidth(lineEdit_nCamera->sizePolicy().hasHeightForWidth());
        lineEdit_nCamera->setSizePolicy(sizePolicy);
        lineEdit_nCamera->setMinimumSize(QSize(180, 35));
        lineEdit_nCamera->setMaximumSize(QSize(160, 35));
        lineEdit_nCamera->setFont(font2);
        lineEdit_nCamera->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(lineEdit_nCamera);

        layoutWidget_5 = new QWidget(SettingUI);
        layoutWidget_5->setObjectName(QStringLiteral("layoutWidget_5"));
        layoutWidget_5->setGeometry(QRect(60, 340, 311, 37));
        horizontalLayout_5 = new QHBoxLayout(layoutWidget_5);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_2 = new QLabel(layoutWidget_5);
        label_roiHeight_L_2->setObjectName(QStringLiteral("label_roiHeight_L_2"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_2->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_2->setSizePolicy(sizePolicy);
        label_roiHeight_L_2->setMinimumSize(QSize(90, 35));
        label_roiHeight_L_2->setMaximumSize(QSize(90, 35));
        label_roiHeight_L_2->setFont(font2);
        label_roiHeight_L_2->setAlignment(Qt::AlignCenter);

        horizontalLayout_5->addWidget(label_roiHeight_L_2);

        horizontalSpacer_5 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);

        lineEdit_nPhotographing = new QLineEdit(layoutWidget_5);
        lineEdit_nPhotographing->setObjectName(QStringLiteral("lineEdit_nPhotographing"));
        sizePolicy.setHeightForWidth(lineEdit_nPhotographing->sizePolicy().hasHeightForWidth());
        lineEdit_nPhotographing->setSizePolicy(sizePolicy);
        lineEdit_nPhotographing->setMinimumSize(QSize(180, 35));
        lineEdit_nPhotographing->setMaximumSize(QSize(150, 35));
        lineEdit_nPhotographing->setFont(font2);
        lineEdit_nPhotographing->setAlignment(Qt::AlignCenter);

        horizontalLayout_5->addWidget(lineEdit_nPhotographing);

        layoutWidget_6 = new QWidget(SettingUI);
        layoutWidget_6->setObjectName(QStringLiteral("layoutWidget_6"));
        layoutWidget_6->setGeometry(QRect(60, 400, 348, 37));
        horizontalLayout_6 = new QHBoxLayout(layoutWidget_6);
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_3 = new QLabel(layoutWidget_6);
        label_roiHeight_L_3->setObjectName(QStringLiteral("label_roiHeight_L_3"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_3->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_3->setSizePolicy(sizePolicy);
        label_roiHeight_L_3->setMinimumSize(QSize(120, 35));
        label_roiHeight_L_3->setMaximumSize(QSize(120, 35));
        label_roiHeight_L_3->setFont(font2);
        label_roiHeight_L_3->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(label_roiHeight_L_3);

        horizontalSpacer_6 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_6);

        label_roiHeight_L_7 = new QLabel(layoutWidget_6);
        label_roiHeight_L_7->setObjectName(QStringLiteral("label_roiHeight_L_7"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_7->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_7->setSizePolicy(sizePolicy);
        label_roiHeight_L_7->setMinimumSize(QSize(30, 35));
        label_roiHeight_L_7->setMaximumSize(QSize(30, 35));
        QFont font3;
        font3.setPointSize(15);
        label_roiHeight_L_7->setFont(font3);
        label_roiHeight_L_7->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(label_roiHeight_L_7);

        lineEdit_nBasicUnitInRow = new QLineEdit(layoutWidget_6);
        lineEdit_nBasicUnitInRow->setObjectName(QStringLiteral("lineEdit_nBasicUnitInRow"));
        sizePolicy.setHeightForWidth(lineEdit_nBasicUnitInRow->sizePolicy().hasHeightForWidth());
        lineEdit_nBasicUnitInRow->setSizePolicy(sizePolicy);
        lineEdit_nBasicUnitInRow->setMinimumSize(QSize(60, 35));
        lineEdit_nBasicUnitInRow->setMaximumSize(QSize(60, 35));
        lineEdit_nBasicUnitInRow->setFont(font2);
        lineEdit_nBasicUnitInRow->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(lineEdit_nBasicUnitInRow);

        horizontalSpacer_7 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_7);

        label_roiHeight_L_8 = new QLabel(layoutWidget_6);
        label_roiHeight_L_8->setObjectName(QStringLiteral("label_roiHeight_L_8"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_8->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_8->setSizePolicy(sizePolicy);
        label_roiHeight_L_8->setMinimumSize(QSize(30, 35));
        label_roiHeight_L_8->setMaximumSize(QSize(30, 35));
        label_roiHeight_L_8->setFont(font3);
        label_roiHeight_L_8->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(label_roiHeight_L_8);

        lineEdit_nBasicUnitInCol = new QLineEdit(layoutWidget_6);
        lineEdit_nBasicUnitInCol->setObjectName(QStringLiteral("lineEdit_nBasicUnitInCol"));
        sizePolicy.setHeightForWidth(lineEdit_nBasicUnitInCol->sizePolicy().hasHeightForWidth());
        lineEdit_nBasicUnitInCol->setSizePolicy(sizePolicy);
        lineEdit_nBasicUnitInCol->setMinimumSize(QSize(60, 35));
        lineEdit_nBasicUnitInCol->setMaximumSize(QSize(60, 35));
        lineEdit_nBasicUnitInCol->setFont(font2);
        lineEdit_nBasicUnitInCol->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(lineEdit_nBasicUnitInCol);

        layoutWidget_7 = new QWidget(SettingUI);
        layoutWidget_7->setObjectName(QStringLiteral("layoutWidget_7"));
        layoutWidget_7->setGeometry(QRect(60, 470, 311, 37));
        horizontalLayout_7 = new QHBoxLayout(layoutWidget_7);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_4 = new QLabel(layoutWidget_7);
        label_roiHeight_L_4->setObjectName(QStringLiteral("label_roiHeight_L_4"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_4->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_4->setSizePolicy(sizePolicy);
        label_roiHeight_L_4->setMinimumSize(QSize(90, 35));
        label_roiHeight_L_4->setMaximumSize(QSize(90, 35));
        label_roiHeight_L_4->setFont(font2);
        label_roiHeight_L_4->setAlignment(Qt::AlignCenter);

        horizontalLayout_7->addWidget(label_roiHeight_L_4);

        horizontalSpacer_8 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_8);

        comboBox_ImageFormat = new QComboBox(layoutWidget_7);
        comboBox_ImageFormat->addItem(QString());
        comboBox_ImageFormat->addItem(QString());
        comboBox_ImageFormat->addItem(QString());
        comboBox_ImageFormat->addItem(QString());
        comboBox_ImageFormat->addItem(QString());
        comboBox_ImageFormat->setObjectName(QStringLiteral("comboBox_ImageFormat"));
        sizePolicy.setHeightForWidth(comboBox_ImageFormat->sizePolicy().hasHeightForWidth());
        comboBox_ImageFormat->setSizePolicy(sizePolicy);
        comboBox_ImageFormat->setMinimumSize(QSize(150, 35));
        comboBox_ImageFormat->setMaximumSize(QSize(150, 35));
        comboBox_ImageFormat->setFont(font3);
        comboBox_ImageFormat->setLayoutDirection(Qt::LeftToRight);

        horizontalLayout_7->addWidget(comboBox_ImageFormat);

        layoutWidget_8 = new QWidget(SettingUI);
        layoutWidget_8->setObjectName(QStringLiteral("layoutWidget_8"));
        layoutWidget_8->setGeometry(QRect(60, 530, 321, 37));
        horizontalLayout_8 = new QHBoxLayout(layoutWidget_8);
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_5 = new QLabel(layoutWidget_8);
        label_roiHeight_L_5->setObjectName(QStringLiteral("label_roiHeight_L_5"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_5->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_5->setSizePolicy(sizePolicy);
        label_roiHeight_L_5->setMinimumSize(QSize(120, 35));
        label_roiHeight_L_5->setMaximumSize(QSize(120, 35));
        label_roiHeight_L_5->setFont(font2);
        label_roiHeight_L_5->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(label_roiHeight_L_5);

        horizontalSpacer_9 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_9);

        lineEdit_ImageAspectRatio_W = new QLineEdit(layoutWidget_8);
        lineEdit_ImageAspectRatio_W->setObjectName(QStringLiteral("lineEdit_ImageAspectRatio_W"));
        sizePolicy.setHeightForWidth(lineEdit_ImageAspectRatio_W->sizePolicy().hasHeightForWidth());
        lineEdit_ImageAspectRatio_W->setSizePolicy(sizePolicy);
        lineEdit_ImageAspectRatio_W->setMinimumSize(QSize(60, 35));
        lineEdit_ImageAspectRatio_W->setMaximumSize(QSize(60, 35));
        lineEdit_ImageAspectRatio_W->setFont(font2);
        lineEdit_ImageAspectRatio_W->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(lineEdit_ImageAspectRatio_W);

        horizontalSpacer_11 = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_11);

        label_roiHeight_L_6 = new QLabel(layoutWidget_8);
        label_roiHeight_L_6->setObjectName(QStringLiteral("label_roiHeight_L_6"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_6->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_6->setSizePolicy(sizePolicy);
        label_roiHeight_L_6->setMinimumSize(QSize(8, 35));
        label_roiHeight_L_6->setMaximumSize(QSize(8, 35));
        QFont font4;
        font4.setPointSize(16);
        font4.setBold(true);
        font4.setWeight(75);
        label_roiHeight_L_6->setFont(font4);
        label_roiHeight_L_6->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(label_roiHeight_L_6);

        horizontalSpacer_12 = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_12);

        lineEdit_ImageAspectRatio_H = new QLineEdit(layoutWidget_8);
        lineEdit_ImageAspectRatio_H->setObjectName(QStringLiteral("lineEdit_ImageAspectRatio_H"));
        sizePolicy.setHeightForWidth(lineEdit_ImageAspectRatio_H->sizePolicy().hasHeightForWidth());
        lineEdit_ImageAspectRatio_H->setSizePolicy(sizePolicy);
        lineEdit_ImageAspectRatio_H->setMinimumSize(QSize(60, 35));
        lineEdit_ImageAspectRatio_H->setMaximumSize(QSize(60, 35));
        lineEdit_ImageAspectRatio_H->setFont(font2);
        lineEdit_ImageAspectRatio_H->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(lineEdit_ImageAspectRatio_H);

        pushButton_admin = new QPushButton(SettingUI);
        pushButton_admin->setObjectName(QStringLiteral("pushButton_admin"));
        pushButton_admin->setGeometry(QRect(850, 710, 120, 40));
        sizePolicy.setHeightForWidth(pushButton_admin->sizePolicy().hasHeightForWidth());
        pushButton_admin->setSizePolicy(sizePolicy);
        pushButton_admin->setFont(font1);
        label_3->raise();
        label_title->raise();
        line->raise();
        pushButton_return->raise();
        pushButton_confirm->raise();
        layoutWidget->raise();
        layoutWidget_2->raise();
        layoutWidget_3->raise();
        layoutWidget_4->raise();
        layoutWidget_5->raise();
        layoutWidget_6->raise();
        layoutWidget_7->raise();
        layoutWidget_8->raise();
        pushButton_admin->raise();

        retranslateUi(SettingUI);

        QMetaObject::connectSlotsByName(SettingUI);
    } // setupUi

    void retranslateUi(QWidget *SettingUI)
    {
        SettingUI->setWindowTitle(QApplication::translate("SettingUI", "SettingUI", nullptr));
        label_title->setText(QApplication::translate("SettingUI", "PCB\346\243\200\346\265\213\347\263\273\347\273\237 - \350\256\276\347\275\256", nullptr));
        label_3->setText(QString());
        pushButton_return->setText(QApplication::translate("SettingUI", "\350\277\224\345\233\236", nullptr));
        pushButton_confirm->setText(QApplication::translate("SettingUI", "\347\241\256\345\256\232", nullptr));
        label_path_L->setText(QApplication::translate("SettingUI", "\346\240\267\346\234\254\350\267\257\345\276\204", nullptr));
        pushButton_SampleDirPath->setText(QApplication::translate("SettingUI", "\351\200\211\346\213\251", nullptr));
        label_path_L_2->setText(QApplication::translate("SettingUI", "\346\250\241\346\235\277\350\267\257\345\276\204", nullptr));
        pushButton_TemplDirPath->setText(QApplication::translate("SettingUI", "\351\200\211\346\213\251", nullptr));
        label_path_L_3->setText(QApplication::translate("SettingUI", "\350\276\223\345\207\272\350\267\257\345\276\204", nullptr));
        pushButton_OutputDirPath->setText(QApplication::translate("SettingUI", "\351\200\211\346\213\251", nullptr));
        label_roiHeight_L->setText(QApplication::translate("SettingUI", "\347\233\270\346\234\272\344\270\252\346\225\260", nullptr));
        label_roiHeight_L_2->setText(QApplication::translate("SettingUI", "\346\213\215\346\221\204\346\254\241\346\225\260", nullptr));
        label_roiHeight_L_3->setText(QApplication::translate("SettingUI", "\345\237\272\346\234\254\345\215\225\345\205\203\346\225\260", nullptr));
        label_roiHeight_L_7->setText(QApplication::translate("SettingUI", "\350\241\214", nullptr));
        label_roiHeight_L_8->setText(QApplication::translate("SettingUI", "\345\210\227", nullptr));
        label_roiHeight_L_4->setText(QApplication::translate("SettingUI", "\345\233\276\345\203\217\346\240\274\345\274\217", nullptr));
        comboBox_ImageFormat->setItemText(0, QApplication::translate("SettingUI", "      -", nullptr));
        comboBox_ImageFormat->setItemText(1, QApplication::translate("SettingUI", "    *.bmp", nullptr));
        comboBox_ImageFormat->setItemText(2, QApplication::translate("SettingUI", "    *.jpg", nullptr));
        comboBox_ImageFormat->setItemText(3, QApplication::translate("SettingUI", "    *.png", nullptr));
        comboBox_ImageFormat->setItemText(4, QApplication::translate("SettingUI", "    *.tif", nullptr));

        label_roiHeight_L_5->setText(QApplication::translate("SettingUI", "\345\233\276\345\203\217\345\256\275\351\253\230\346\257\224", nullptr));
        label_roiHeight_L_6->setText(QApplication::translate("SettingUI", ":", nullptr));
        lineEdit_ImageAspectRatio_H->setText(QString());
        pushButton_admin->setText(QApplication::translate("SettingUI", "\347\263\273\347\273\237\350\256\276\347\275\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingUI: public Ui_SettingUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGUI_H
