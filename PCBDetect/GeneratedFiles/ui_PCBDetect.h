/********************************************************************************
** Form generated from reading UI file 'PCBDetect.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PCBDETECT_H
#define UI_PCBDETECT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PCBDetectClass
{
public:
    QWidget *centralWidget;
    QLabel *label_logo;
    QLabel *label_title;
    QFrame *line;
    QLabel *label_3;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton_set;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *pushButton_set2;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButton_getTempl;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_6;
    QPushButton *pushButton_getTempl2;
    QSpacerItem *horizontalSpacer_7;
    QSpacerItem *horizontalSpacer_2;
    QVBoxLayout *verticalLayout_3;
    QPushButton *pushButton_detect;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_8;
    QPushButton *pushButton_detect2;
    QSpacerItem *horizontalSpacer_9;
    QSpacerItem *horizontalSpacer_3;
    QVBoxLayout *verticalLayout_4;
    QPushButton *pushButton_exit;
    QSpacerItem *verticalSpacer_4;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_10;
    QPushButton *pushButton_exit2;
    QSpacerItem *horizontalSpacer_11;

    void setupUi(QMainWindow *PCBDetectClass)
    {
        if (PCBDetectClass->objectName().isEmpty())
            PCBDetectClass->setObjectName(QStringLiteral("PCBDetectClass"));
        PCBDetectClass->resize(1440, 900);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PCBDetectClass->sizePolicy().hasHeightForWidth());
        PCBDetectClass->setSizePolicy(sizePolicy);
        QFont font;
        font.setPointSize(16);
        PCBDetectClass->setFont(font);
        centralWidget = new QWidget(PCBDetectClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label_logo = new QLabel(centralWidget);
        label_logo->setObjectName(QStringLiteral("label_logo"));
        label_logo->setGeometry(QRect(610, 0, 51, 35));
        sizePolicy.setHeightForWidth(label_logo->sizePolicy().hasHeightForWidth());
        label_logo->setSizePolicy(sizePolicy);
        label_logo->setMaximumSize(QSize(16777215, 16000000));
        QFont font1;
        font1.setPointSize(12);
        font1.setBold(true);
        font1.setWeight(75);
        label_logo->setFont(font1);
        label_logo->setStyleSheet(QStringLiteral("border-image: url(:/PCBDetect/icons/logo.png);"));
        label_logo->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        label_title = new QLabel(centralWidget);
        label_title->setObjectName(QStringLiteral("label_title"));
        label_title->setGeometry(QRect(670, 0, 171, 41));
        sizePolicy.setHeightForWidth(label_title->sizePolicy().hasHeightForWidth());
        label_title->setSizePolicy(sizePolicy);
        label_title->setMinimumSize(QSize(60, 30));
        QFont font2;
        font2.setFamily(QStringLiteral("SimSun-ExtB FB"));
        font2.setPointSize(22);
        font2.setBold(false);
        font2.setItalic(false);
        font2.setWeight(25);
        label_title->setFont(font2);
        label_title->setStyleSheet(QStringLiteral("font: 200 22pt \"SimSun-ExtB FB\";"));
        label_title->setAlignment(Qt::AlignCenter);
        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(0, 30, 1441, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(0, -1, 1441, 41));
        label_3->setStyleSheet(QStringLiteral("background-color: rgb(240, 240, 250);"));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(150, 310, 1141, 268));
        horizontalLayout_5 = new QHBoxLayout(layoutWidget);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        pushButton_set = new QPushButton(layoutWidget);
        pushButton_set->setObjectName(QStringLiteral("pushButton_set"));
        sizePolicy.setHeightForWidth(pushButton_set->sizePolicy().hasHeightForWidth());
        pushButton_set->setSizePolicy(sizePolicy);
        pushButton_set->setMinimumSize(QSize(200, 200));
        pushButton_set->setMaximumSize(QSize(200, 200));
        pushButton_set->setStyleSheet(QStringLiteral("border-image: url(:/PCBDetect/icons/set.png);"));

        verticalLayout->addWidget(pushButton_set);

        verticalSpacer = new QSpacerItem(20, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);

        pushButton_set2 = new QPushButton(layoutWidget);
        pushButton_set2->setObjectName(QStringLiteral("pushButton_set2"));
        sizePolicy.setHeightForWidth(pushButton_set2->sizePolicy().hasHeightForWidth());
        pushButton_set2->setSizePolicy(sizePolicy);
        pushButton_set2->setMinimumSize(QSize(120, 40));
        QFont font3;
        font3.setPointSize(17);
        pushButton_set2->setFont(font3);
        pushButton_set2->setAutoDefault(false);
        pushButton_set2->setFlat(false);

        horizontalLayout->addWidget(pushButton_set2);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_5);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_5->addLayout(verticalLayout);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        pushButton_getTempl = new QPushButton(layoutWidget);
        pushButton_getTempl->setObjectName(QStringLiteral("pushButton_getTempl"));
        sizePolicy.setHeightForWidth(pushButton_getTempl->sizePolicy().hasHeightForWidth());
        pushButton_getTempl->setSizePolicy(sizePolicy);
        pushButton_getTempl->setMinimumSize(QSize(200, 200));
        pushButton_getTempl->setMaximumSize(QSize(200, 200));
        pushButton_getTempl->setStyleSheet(QStringLiteral("border-image: url(:/PCBDetect/icons/getTempl.png);"));

        verticalLayout_2->addWidget(pushButton_getTempl);

        verticalSpacer_2 = new QSpacerItem(20, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_2->addItem(verticalSpacer_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_6);

        pushButton_getTempl2 = new QPushButton(layoutWidget);
        pushButton_getTempl2->setObjectName(QStringLiteral("pushButton_getTempl2"));
        sizePolicy.setHeightForWidth(pushButton_getTempl2->sizePolicy().hasHeightForWidth());
        pushButton_getTempl2->setSizePolicy(sizePolicy);
        pushButton_getTempl2->setMinimumSize(QSize(120, 40));
        pushButton_getTempl2->setFont(font3);
        pushButton_getTempl2->setFlat(false);

        horizontalLayout_2->addWidget(pushButton_getTempl2);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_7);


        verticalLayout_2->addLayout(horizontalLayout_2);


        horizontalLayout_5->addLayout(verticalLayout_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        pushButton_detect = new QPushButton(layoutWidget);
        pushButton_detect->setObjectName(QStringLiteral("pushButton_detect"));
        sizePolicy.setHeightForWidth(pushButton_detect->sizePolicy().hasHeightForWidth());
        pushButton_detect->setSizePolicy(sizePolicy);
        pushButton_detect->setMinimumSize(QSize(200, 200));
        pushButton_detect->setMaximumSize(QSize(200, 200));
        pushButton_detect->setStyleSheet(QStringLiteral("border-image: url(:/PCBDetect/icons/detect.png);"));

        verticalLayout_3->addWidget(pushButton_detect);

        verticalSpacer_3 = new QSpacerItem(20, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_3->addItem(verticalSpacer_3);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_8);

        pushButton_detect2 = new QPushButton(layoutWidget);
        pushButton_detect2->setObjectName(QStringLiteral("pushButton_detect2"));
        sizePolicy.setHeightForWidth(pushButton_detect2->sizePolicy().hasHeightForWidth());
        pushButton_detect2->setSizePolicy(sizePolicy);
        pushButton_detect2->setMinimumSize(QSize(120, 40));
        pushButton_detect2->setFont(font3);
        pushButton_detect2->setFlat(false);

        horizontalLayout_3->addWidget(pushButton_detect2);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_9);


        verticalLayout_3->addLayout(horizontalLayout_3);


        horizontalLayout_5->addLayout(verticalLayout_3);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        pushButton_exit = new QPushButton(layoutWidget);
        pushButton_exit->setObjectName(QStringLiteral("pushButton_exit"));
        sizePolicy.setHeightForWidth(pushButton_exit->sizePolicy().hasHeightForWidth());
        pushButton_exit->setSizePolicy(sizePolicy);
        pushButton_exit->setMinimumSize(QSize(200, 200));
        pushButton_exit->setMaximumSize(QSize(200, 200));
        pushButton_exit->setStyleSheet(QStringLiteral("border-image: url(:/PCBDetect/icons/exit.png);"));

        verticalLayout_4->addWidget(pushButton_exit);

        verticalSpacer_4 = new QSpacerItem(20, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_4->addItem(verticalSpacer_4);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_10);

        pushButton_exit2 = new QPushButton(layoutWidget);
        pushButton_exit2->setObjectName(QStringLiteral("pushButton_exit2"));
        sizePolicy.setHeightForWidth(pushButton_exit2->sizePolicy().hasHeightForWidth());
        pushButton_exit2->setSizePolicy(sizePolicy);
        pushButton_exit2->setMinimumSize(QSize(120, 40));
        pushButton_exit2->setFont(font3);
        pushButton_exit2->setFlat(false);

        horizontalLayout_4->addWidget(pushButton_exit2);

        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_11);


        verticalLayout_4->addLayout(horizontalLayout_4);


        horizontalLayout_5->addLayout(verticalLayout_4);

        PCBDetectClass->setCentralWidget(centralWidget);
        layoutWidget->raise();
        label_3->raise();
        label_logo->raise();
        label_title->raise();
        line->raise();

        retranslateUi(PCBDetectClass);

        pushButton_set2->setDefault(false);


        QMetaObject::connectSlotsByName(PCBDetectClass);
    } // setupUi

    void retranslateUi(QMainWindow *PCBDetectClass)
    {
        PCBDetectClass->setWindowTitle(QApplication::translate("PCBDetectClass", "PCBDetect", nullptr));
        label_logo->setText(QString());
        label_title->setText(QApplication::translate("PCBDetectClass", "PCB\346\243\200\346\265\213\347\263\273\347\273\237", nullptr));
        label_3->setText(QString());
        pushButton_set->setText(QString());
        pushButton_set2->setText(QApplication::translate("PCBDetectClass", "\350\256\276\347\275\256", nullptr));
        pushButton_getTempl->setText(QString());
        pushButton_getTempl2->setText(QApplication::translate("PCBDetectClass", "\346\250\241\346\235\277\346\217\220\345\217\226", nullptr));
        pushButton_detect->setText(QString());
        pushButton_detect2->setText(QApplication::translate("PCBDetectClass", "\346\243\200\346\265\213", nullptr));
        pushButton_exit->setText(QString());
        pushButton_exit2->setText(QApplication::translate("PCBDetectClass", "\351\200\200\345\207\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PCBDetectClass: public Ui_PCBDetectClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PCBDETECT_H
