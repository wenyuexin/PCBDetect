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
#include <QtWidgets/QCheckBox>
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
    QPushButton *pushButton_admin;
    QWidget *layoutWidget_8;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_roiHeight_L_5;
    QSpacerItem *horizontalSpacer_9;
    QLabel *label_roiHeight_L_9;
    QLineEdit *lineEdit_ActualProductSize_W;
    QSpacerItem *horizontalSpacer_10;
    QLabel *label_roiHeight_L_10;
    QLineEdit *lineEdit_ActualProductSize_H;
    QWidget *layoutWidget_9;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_roiHeight_L_6;
    QSpacerItem *horizontalSpacer_11;
    QComboBox *comboBox_clusterComPort;
    QWidget *layoutWidget_10;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_roiHeight_L_11;
    QSpacerItem *horizontalSpacer_20;
    QCheckBox *checkBox_matchingAccuracy_high;
    QSpacerItem *horizontalSpacer_13;
    QCheckBox *checkBox_matchingAccuracy_low;
    QWidget *layoutWidget_11;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_roiHeight_L_12;
    QSpacerItem *horizontalSpacer_18;
    QLineEdit *lineEdit_convexRateThresh;
    QLabel *label_roiHeight_L_13;
    QWidget *layoutWidget_12;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_roiHeight_L_14;
    QSpacerItem *horizontalSpacer_19;
    QLineEdit *lineEdit_concaveRateThresh;
    QLabel *label_roiHeight_L_15;

    void setupUi(QWidget *SettingUI)
    {
        if (SettingUI->objectName().isEmpty())
            SettingUI->setObjectName(QStringLiteral("SettingUI"));
        SettingUI->resize(1440, 900);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SettingUI->sizePolicy().hasHeightForWidth());
        SettingUI->setSizePolicy(sizePolicy);
        label_title = new QLabel(SettingUI);
        label_title->setObjectName(QStringLiteral("label_title"));
        label_title->setGeometry(QRect(580, 0, 271, 41));
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
        label_3->setGeometry(QRect(0, -1, 1441, 41));
        label_3->setStyleSheet(QStringLiteral("background-color: rgb(240, 240, 250);"));
        line = new QFrame(SettingUI);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(0, 30, 1441, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        pushButton_return = new QPushButton(SettingUI);
        pushButton_return->setObjectName(QStringLiteral("pushButton_return"));
        pushButton_return->setGeometry(QRect(660, 830, 120, 40));
        sizePolicy.setHeightForWidth(pushButton_return->sizePolicy().hasHeightForWidth());
        pushButton_return->setSizePolicy(sizePolicy);
        QFont font1;
        font1.setPointSize(17);
        pushButton_return->setFont(font1);
        pushButton_confirm = new QPushButton(SettingUI);
        pushButton_confirm->setObjectName(QStringLiteral("pushButton_confirm"));
        pushButton_confirm->setGeometry(QRect(450, 830, 120, 40));
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

        layoutWidget_6 = new QWidget(SettingUI);
        layoutWidget_6->setObjectName(QStringLiteral("layoutWidget_6"));
        layoutWidget_6->setGeometry(QRect(70, 450, 341, 37));
        horizontalLayout_6 = new QHBoxLayout(layoutWidget_6);
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_3 = new QLabel(layoutWidget_6);
        label_roiHeight_L_3->setObjectName(QStringLiteral("label_roiHeight_L_3"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_3->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_3->setSizePolicy(sizePolicy);
        label_roiHeight_L_3->setMinimumSize(QSize(130, 35));
        label_roiHeight_L_3->setMaximumSize(QSize(130, 35));
        label_roiHeight_L_3->setFont(font2);
        label_roiHeight_L_3->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(label_roiHeight_L_3);

        horizontalSpacer_6 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_6);

        label_roiHeight_L_7 = new QLabel(layoutWidget_6);
        label_roiHeight_L_7->setObjectName(QStringLiteral("label_roiHeight_L_7"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_7->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_7->setSizePolicy(sizePolicy);
        label_roiHeight_L_7->setMinimumSize(QSize(25, 35));
        label_roiHeight_L_7->setMaximumSize(QSize(25, 35));
        QFont font3;
        font3.setPointSize(15);
        label_roiHeight_L_7->setFont(font3);
        label_roiHeight_L_7->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(label_roiHeight_L_7);

        lineEdit_nBasicUnitInRow = new QLineEdit(layoutWidget_6);
        lineEdit_nBasicUnitInRow->setObjectName(QStringLiteral("lineEdit_nBasicUnitInRow"));
        sizePolicy.setHeightForWidth(lineEdit_nBasicUnitInRow->sizePolicy().hasHeightForWidth());
        lineEdit_nBasicUnitInRow->setSizePolicy(sizePolicy);
        lineEdit_nBasicUnitInRow->setMinimumSize(QSize(62, 35));
        lineEdit_nBasicUnitInRow->setMaximumSize(QSize(62, 35));
        lineEdit_nBasicUnitInRow->setFont(font2);
        lineEdit_nBasicUnitInRow->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(lineEdit_nBasicUnitInRow);

        horizontalSpacer_7 = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_7);

        label_roiHeight_L_8 = new QLabel(layoutWidget_6);
        label_roiHeight_L_8->setObjectName(QStringLiteral("label_roiHeight_L_8"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_8->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_8->setSizePolicy(sizePolicy);
        label_roiHeight_L_8->setMinimumSize(QSize(25, 35));
        label_roiHeight_L_8->setMaximumSize(QSize(25, 35));
        label_roiHeight_L_8->setFont(font3);
        label_roiHeight_L_8->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(label_roiHeight_L_8);

        lineEdit_nBasicUnitInCol = new QLineEdit(layoutWidget_6);
        lineEdit_nBasicUnitInCol->setObjectName(QStringLiteral("lineEdit_nBasicUnitInCol"));
        sizePolicy.setHeightForWidth(lineEdit_nBasicUnitInCol->sizePolicy().hasHeightForWidth());
        lineEdit_nBasicUnitInCol->setSizePolicy(sizePolicy);
        lineEdit_nBasicUnitInCol->setMinimumSize(QSize(62, 35));
        lineEdit_nBasicUnitInCol->setMaximumSize(QSize(62, 35));
        lineEdit_nBasicUnitInCol->setFont(font2);
        lineEdit_nBasicUnitInCol->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(lineEdit_nBasicUnitInCol);

        layoutWidget_7 = new QWidget(SettingUI);
        layoutWidget_7->setObjectName(QStringLiteral("layoutWidget_7"));
        layoutWidget_7->setGeometry(QRect(70, 270, 311, 37));
        horizontalLayout_7 = new QHBoxLayout(layoutWidget_7);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_4 = new QLabel(layoutWidget_7);
        label_roiHeight_L_4->setObjectName(QStringLiteral("label_roiHeight_L_4"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_4->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_4->setSizePolicy(sizePolicy);
        label_roiHeight_L_4->setMinimumSize(QSize(125, 35));
        label_roiHeight_L_4->setMaximumSize(QSize(125, 35));
        label_roiHeight_L_4->setFont(font2);
        label_roiHeight_L_4->setAlignment(Qt::AlignCenter);

        horizontalLayout_7->addWidget(label_roiHeight_L_4);

        horizontalSpacer_8 = new QSpacerItem(7, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

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

        pushButton_admin = new QPushButton(SettingUI);
        pushButton_admin->setObjectName(QStringLiteral("pushButton_admin"));
        pushButton_admin->setGeometry(QRect(870, 830, 120, 40));
        sizePolicy.setHeightForWidth(pushButton_admin->sizePolicy().hasHeightForWidth());
        pushButton_admin->setSizePolicy(sizePolicy);
        pushButton_admin->setFont(font1);
        layoutWidget_8 = new QWidget(SettingUI);
        layoutWidget_8->setObjectName(QStringLiteral("layoutWidget_8"));
        layoutWidget_8->setGeometry(QRect(70, 390, 341, 37));
        horizontalLayout_8 = new QHBoxLayout(layoutWidget_8);
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_5 = new QLabel(layoutWidget_8);
        label_roiHeight_L_5->setObjectName(QStringLiteral("label_roiHeight_L_5"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_5->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_5->setSizePolicy(sizePolicy);
        label_roiHeight_L_5->setMinimumSize(QSize(130, 35));
        label_roiHeight_L_5->setMaximumSize(QSize(130, 35));
        label_roiHeight_L_5->setFont(font2);
        label_roiHeight_L_5->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(label_roiHeight_L_5);

        horizontalSpacer_9 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_9);

        label_roiHeight_L_9 = new QLabel(layoutWidget_8);
        label_roiHeight_L_9->setObjectName(QStringLiteral("label_roiHeight_L_9"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_9->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_9->setSizePolicy(sizePolicy);
        label_roiHeight_L_9->setMinimumSize(QSize(25, 35));
        label_roiHeight_L_9->setMaximumSize(QSize(25, 35));
        label_roiHeight_L_9->setFont(font3);
        label_roiHeight_L_9->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(label_roiHeight_L_9);

        lineEdit_ActualProductSize_W = new QLineEdit(layoutWidget_8);
        lineEdit_ActualProductSize_W->setObjectName(QStringLiteral("lineEdit_ActualProductSize_W"));
        sizePolicy.setHeightForWidth(lineEdit_ActualProductSize_W->sizePolicy().hasHeightForWidth());
        lineEdit_ActualProductSize_W->setSizePolicy(sizePolicy);
        lineEdit_ActualProductSize_W->setMinimumSize(QSize(62, 35));
        lineEdit_ActualProductSize_W->setMaximumSize(QSize(62, 35));
        lineEdit_ActualProductSize_W->setFont(font2);
        lineEdit_ActualProductSize_W->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(lineEdit_ActualProductSize_W);

        horizontalSpacer_10 = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_10);

        label_roiHeight_L_10 = new QLabel(layoutWidget_8);
        label_roiHeight_L_10->setObjectName(QStringLiteral("label_roiHeight_L_10"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_10->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_10->setSizePolicy(sizePolicy);
        label_roiHeight_L_10->setMinimumSize(QSize(25, 35));
        label_roiHeight_L_10->setMaximumSize(QSize(25, 35));
        label_roiHeight_L_10->setFont(font3);
        label_roiHeight_L_10->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(label_roiHeight_L_10);

        lineEdit_ActualProductSize_H = new QLineEdit(layoutWidget_8);
        lineEdit_ActualProductSize_H->setObjectName(QStringLiteral("lineEdit_ActualProductSize_H"));
        sizePolicy.setHeightForWidth(lineEdit_ActualProductSize_H->sizePolicy().hasHeightForWidth());
        lineEdit_ActualProductSize_H->setSizePolicy(sizePolicy);
        lineEdit_ActualProductSize_H->setMinimumSize(QSize(62, 35));
        lineEdit_ActualProductSize_H->setMaximumSize(QSize(62, 35));
        lineEdit_ActualProductSize_H->setFont(font2);
        lineEdit_ActualProductSize_H->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(lineEdit_ActualProductSize_H);

        layoutWidget_9 = new QWidget(SettingUI);
        layoutWidget_9->setObjectName(QStringLiteral("layoutWidget_9"));
        layoutWidget_9->setGeometry(QRect(70, 330, 311, 37));
        horizontalLayout_9 = new QHBoxLayout(layoutWidget_9);
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        horizontalLayout_9->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_6 = new QLabel(layoutWidget_9);
        label_roiHeight_L_6->setObjectName(QStringLiteral("label_roiHeight_L_6"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_6->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_6->setSizePolicy(sizePolicy);
        label_roiHeight_L_6->setMinimumSize(QSize(125, 35));
        label_roiHeight_L_6->setMaximumSize(QSize(125, 35));
        label_roiHeight_L_6->setFont(font2);
        label_roiHeight_L_6->setAlignment(Qt::AlignCenter);

        horizontalLayout_9->addWidget(label_roiHeight_L_6);

        horizontalSpacer_11 = new QSpacerItem(7, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_11);

        comboBox_clusterComPort = new QComboBox(layoutWidget_9);
        comboBox_clusterComPort->addItem(QString());
        comboBox_clusterComPort->addItem(QString());
        comboBox_clusterComPort->addItem(QString());
        comboBox_clusterComPort->addItem(QString());
        comboBox_clusterComPort->addItem(QString());
        comboBox_clusterComPort->addItem(QString());
        comboBox_clusterComPort->addItem(QString());
        comboBox_clusterComPort->addItem(QString());
        comboBox_clusterComPort->addItem(QString());
        comboBox_clusterComPort->addItem(QString());
        comboBox_clusterComPort->setObjectName(QStringLiteral("comboBox_clusterComPort"));
        sizePolicy.setHeightForWidth(comboBox_clusterComPort->sizePolicy().hasHeightForWidth());
        comboBox_clusterComPort->setSizePolicy(sizePolicy);
        comboBox_clusterComPort->setMinimumSize(QSize(150, 35));
        comboBox_clusterComPort->setMaximumSize(QSize(150, 35));
        comboBox_clusterComPort->setFont(font3);
        comboBox_clusterComPort->setLayoutDirection(Qt::LeftToRight);

        horizontalLayout_9->addWidget(comboBox_clusterComPort);

        layoutWidget_10 = new QWidget(SettingUI);
        layoutWidget_10->setObjectName(QStringLiteral("layoutWidget_10"));
        layoutWidget_10->setGeometry(QRect(70, 510, 281, 37));
        horizontalLayout_10 = new QHBoxLayout(layoutWidget_10);
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalLayout_10->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_11 = new QLabel(layoutWidget_10);
        label_roiHeight_L_11->setObjectName(QStringLiteral("label_roiHeight_L_11"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_11->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_11->setSizePolicy(sizePolicy);
        label_roiHeight_L_11->setMinimumSize(QSize(130, 35));
        label_roiHeight_L_11->setMaximumSize(QSize(120, 35));
        label_roiHeight_L_11->setFont(font2);
        label_roiHeight_L_11->setAlignment(Qt::AlignCenter);

        horizontalLayout_10->addWidget(label_roiHeight_L_11);

        horizontalSpacer_20 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_20);

        checkBox_matchingAccuracy_high = new QCheckBox(layoutWidget_10);
        checkBox_matchingAccuracy_high->setObjectName(QStringLiteral("checkBox_matchingAccuracy_high"));
        sizePolicy.setHeightForWidth(checkBox_matchingAccuracy_high->sizePolicy().hasHeightForWidth());
        checkBox_matchingAccuracy_high->setSizePolicy(sizePolicy);
        checkBox_matchingAccuracy_high->setMinimumSize(QSize(30, 35));
        checkBox_matchingAccuracy_high->setMaximumSize(QSize(16777215, 35));
        checkBox_matchingAccuracy_high->setFont(font3);

        horizontalLayout_10->addWidget(checkBox_matchingAccuracy_high);

        horizontalSpacer_13 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_13);

        checkBox_matchingAccuracy_low = new QCheckBox(layoutWidget_10);
        checkBox_matchingAccuracy_low->setObjectName(QStringLiteral("checkBox_matchingAccuracy_low"));
        sizePolicy.setHeightForWidth(checkBox_matchingAccuracy_low->sizePolicy().hasHeightForWidth());
        checkBox_matchingAccuracy_low->setSizePolicy(sizePolicy);
        checkBox_matchingAccuracy_low->setMinimumSize(QSize(30, 35));
        checkBox_matchingAccuracy_low->setMaximumSize(QSize(16777215, 35));
        checkBox_matchingAccuracy_low->setFont(font3);

        horizontalLayout_10->addWidget(checkBox_matchingAccuracy_low);

        layoutWidget_11 = new QWidget(SettingUI);
        layoutWidget_11->setObjectName(QStringLiteral("layoutWidget_11"));
        layoutWidget_11->setGeometry(QRect(70, 630, 297, 37));
        horizontalLayout_11 = new QHBoxLayout(layoutWidget_11);
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        horizontalLayout_11->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_12 = new QLabel(layoutWidget_11);
        label_roiHeight_L_12->setObjectName(QStringLiteral("label_roiHeight_L_12"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_12->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_12->setSizePolicy(sizePolicy);
        label_roiHeight_L_12->setMinimumSize(QSize(130, 35));
        label_roiHeight_L_12->setMaximumSize(QSize(130, 35));
        label_roiHeight_L_12->setFont(font2);
        label_roiHeight_L_12->setAlignment(Qt::AlignCenter);

        horizontalLayout_11->addWidget(label_roiHeight_L_12);

        horizontalSpacer_18 = new QSpacerItem(7, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_18);

        lineEdit_convexRateThresh = new QLineEdit(layoutWidget_11);
        lineEdit_convexRateThresh->setObjectName(QStringLiteral("lineEdit_convexRateThresh"));
        sizePolicy.setHeightForWidth(lineEdit_convexRateThresh->sizePolicy().hasHeightForWidth());
        lineEdit_convexRateThresh->setSizePolicy(sizePolicy);
        lineEdit_convexRateThresh->setMinimumSize(QSize(120, 35));
        lineEdit_convexRateThresh->setMaximumSize(QSize(110, 35));
        lineEdit_convexRateThresh->setFont(font2);
        lineEdit_convexRateThresh->setAlignment(Qt::AlignCenter);

        horizontalLayout_11->addWidget(lineEdit_convexRateThresh);

        label_roiHeight_L_13 = new QLabel(layoutWidget_11);
        label_roiHeight_L_13->setObjectName(QStringLiteral("label_roiHeight_L_13"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_13->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_13->setSizePolicy(sizePolicy);
        label_roiHeight_L_13->setMinimumSize(QSize(20, 35));
        label_roiHeight_L_13->setMaximumSize(QSize(20, 35));
        label_roiHeight_L_13->setFont(font2);
        label_roiHeight_L_13->setAlignment(Qt::AlignCenter);

        horizontalLayout_11->addWidget(label_roiHeight_L_13);

        layoutWidget_12 = new QWidget(SettingUI);
        layoutWidget_12->setObjectName(QStringLiteral("layoutWidget_12"));
        layoutWidget_12->setGeometry(QRect(70, 570, 297, 37));
        horizontalLayout_12 = new QHBoxLayout(layoutWidget_12);
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        horizontalLayout_12->setContentsMargins(0, 0, 0, 0);
        label_roiHeight_L_14 = new QLabel(layoutWidget_12);
        label_roiHeight_L_14->setObjectName(QStringLiteral("label_roiHeight_L_14"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_14->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_14->setSizePolicy(sizePolicy);
        label_roiHeight_L_14->setMinimumSize(QSize(130, 35));
        label_roiHeight_L_14->setMaximumSize(QSize(130, 35));
        label_roiHeight_L_14->setFont(font2);
        label_roiHeight_L_14->setAlignment(Qt::AlignCenter);

        horizontalLayout_12->addWidget(label_roiHeight_L_14);

        horizontalSpacer_19 = new QSpacerItem(7, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_19);

        lineEdit_concaveRateThresh = new QLineEdit(layoutWidget_12);
        lineEdit_concaveRateThresh->setObjectName(QStringLiteral("lineEdit_concaveRateThresh"));
        sizePolicy.setHeightForWidth(lineEdit_concaveRateThresh->sizePolicy().hasHeightForWidth());
        lineEdit_concaveRateThresh->setSizePolicy(sizePolicy);
        lineEdit_concaveRateThresh->setMinimumSize(QSize(120, 35));
        lineEdit_concaveRateThresh->setMaximumSize(QSize(110, 35));
        lineEdit_concaveRateThresh->setFont(font2);
        lineEdit_concaveRateThresh->setAlignment(Qt::AlignCenter);

        horizontalLayout_12->addWidget(lineEdit_concaveRateThresh);

        label_roiHeight_L_15 = new QLabel(layoutWidget_12);
        label_roiHeight_L_15->setObjectName(QStringLiteral("label_roiHeight_L_15"));
        sizePolicy.setHeightForWidth(label_roiHeight_L_15->sizePolicy().hasHeightForWidth());
        label_roiHeight_L_15->setSizePolicy(sizePolicy);
        label_roiHeight_L_15->setMinimumSize(QSize(20, 35));
        label_roiHeight_L_15->setMaximumSize(QSize(20, 35));
        label_roiHeight_L_15->setFont(font2);
        label_roiHeight_L_15->setAlignment(Qt::AlignCenter);

        horizontalLayout_12->addWidget(label_roiHeight_L_15);

        label_3->raise();
        label_title->raise();
        line->raise();
        pushButton_return->raise();
        pushButton_confirm->raise();
        layoutWidget->raise();
        layoutWidget_2->raise();
        layoutWidget_3->raise();
        layoutWidget_6->raise();
        layoutWidget_7->raise();
        pushButton_admin->raise();
        layoutWidget_8->raise();
        layoutWidget_9->raise();
        layoutWidget_10->raise();
        layoutWidget_11->raise();
        layoutWidget_12->raise();

        retranslateUi(SettingUI);

        QMetaObject::connectSlotsByName(SettingUI);
    } // setupUi

    void retranslateUi(QWidget *SettingUI)
    {
        SettingUI->setWindowTitle(QApplication::translate("SettingUI", "SettingUI", nullptr));
#ifndef QT_NO_TOOLTIP
        SettingUI->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        SettingUI->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
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
        label_roiHeight_L_3->setText(QApplication::translate("SettingUI", "\345\237\272\346\234\254\345\215\225\345\205\203\346\225\260", nullptr));
        label_roiHeight_L_7->setText(QApplication::translate("SettingUI", "\350\241\214", nullptr));
        label_roiHeight_L_8->setText(QApplication::translate("SettingUI", "\345\210\227", nullptr));
        label_roiHeight_L_4->setText(QApplication::translate("SettingUI", "\345\233\276\345\203\217\346\240\274\345\274\217", nullptr));
        comboBox_ImageFormat->setItemText(0, QApplication::translate("SettingUI", "      -", nullptr));
        comboBox_ImageFormat->setItemText(1, QApplication::translate("SettingUI", "    *.bmp", nullptr));
        comboBox_ImageFormat->setItemText(2, QApplication::translate("SettingUI", "    *.jpg", nullptr));
        comboBox_ImageFormat->setItemText(3, QApplication::translate("SettingUI", "    *.png", nullptr));
        comboBox_ImageFormat->setItemText(4, QApplication::translate("SettingUI", "    *.tif", nullptr));

        pushButton_admin->setText(QApplication::translate("SettingUI", "\347\263\273\347\273\237\350\256\276\347\275\256", nullptr));
        label_roiHeight_L_5->setText(QApplication::translate("SettingUI", "\344\272\247\345\223\201\345\260\272\345\257\270(mm)", nullptr));
        label_roiHeight_L_9->setText(QApplication::translate("SettingUI", "\345\256\275", nullptr));
        label_roiHeight_L_10->setText(QApplication::translate("SettingUI", "\351\253\230", nullptr));
        label_roiHeight_L_6->setText(QApplication::translate("SettingUI", "\350\277\220\345\212\250\346\216\247\345\210\266\344\270\262\345\217\243", nullptr));
        comboBox_clusterComPort->setItemText(0, QApplication::translate("SettingUI", "      -", nullptr));
        comboBox_clusterComPort->setItemText(1, QApplication::translate("SettingUI", "     COM1", nullptr));
        comboBox_clusterComPort->setItemText(2, QApplication::translate("SettingUI", "     COM2", nullptr));
        comboBox_clusterComPort->setItemText(3, QApplication::translate("SettingUI", "     COM3", nullptr));
        comboBox_clusterComPort->setItemText(4, QApplication::translate("SettingUI", "     COM4", nullptr));
        comboBox_clusterComPort->setItemText(5, QApplication::translate("SettingUI", "     COM5", nullptr));
        comboBox_clusterComPort->setItemText(6, QApplication::translate("SettingUI", "     COM6", nullptr));
        comboBox_clusterComPort->setItemText(7, QApplication::translate("SettingUI", "     COM7", nullptr));
        comboBox_clusterComPort->setItemText(8, QApplication::translate("SettingUI", "     COM8", nullptr));
        comboBox_clusterComPort->setItemText(9, QApplication::translate("SettingUI", "     COM9", nullptr));

#ifndef QT_NO_TOOLTIP
        label_roiHeight_L_11->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        label_roiHeight_L_11->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        label_roiHeight_L_11->setText(QApplication::translate("SettingUI", "\345\214\271\351\205\215\347\262\276\345\272\246", nullptr));
        checkBox_matchingAccuracy_high->setText(QApplication::translate("SettingUI", "\351\253\230", nullptr));
        checkBox_matchingAccuracy_low->setText(QApplication::translate("SettingUI", "\344\275\216", nullptr));
        label_roiHeight_L_12->setText(QApplication::translate("SettingUI", "\345\207\270\350\265\267\347\216\207\351\230\210\345\200\274", nullptr));
        label_roiHeight_L_13->setText(QApplication::translate("SettingUI", "%", nullptr));
        label_roiHeight_L_14->setText(QApplication::translate("SettingUI", "\347\274\272\345\244\261\347\216\207\351\230\210\345\200\274", nullptr));
        label_roiHeight_L_15->setText(QApplication::translate("SettingUI", "%", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingUI: public Ui_SettingUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGUI_H
