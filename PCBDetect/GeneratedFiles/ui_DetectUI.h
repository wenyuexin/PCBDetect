/********************************************************************************
** Form generated from reading UI file 'DetectUI.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DETECTUI_H
#define UI_DETECTUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DetectUI
{
public:
    QFrame *line;
    QLabel *label_logo;
    QLabel *label_3;
    QLabel *label_title;
    QGraphicsView *graphicsView;
    QPushButton *pushButton_start;
    QPushButton *pushButton_return;
    QLabel *label_status;
    QLabel *label;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *label_indicator;
    QSpacerItem *horizontalSpacer;
    QLabel *label_result;

    void setupUi(QWidget *DetectUI)
    {
        if (DetectUI->objectName().isEmpty())
            DetectUI->setObjectName(QStringLiteral("DetectUI"));
        DetectUI->resize(1440, 900);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DetectUI->sizePolicy().hasHeightForWidth());
        DetectUI->setSizePolicy(sizePolicy);
        line = new QFrame(DetectUI);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(0, 30, 1441, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        label_logo = new QLabel(DetectUI);
        label_logo->setObjectName(QStringLiteral("label_logo"));
        label_logo->setGeometry(QRect(1239, 89, 151, 111));
        sizePolicy.setHeightForWidth(label_logo->sizePolicy().hasHeightForWidth());
        label_logo->setSizePolicy(sizePolicy);
        label_logo->setMaximumSize(QSize(16777215, 16000000));
        QFont font;
        font.setPointSize(16);
        font.setBold(false);
        font.setWeight(50);
        label_logo->setFont(font);
        label_logo->setStyleSheet(QStringLiteral("border-image: url(:/PCBDetect/Icons/logo.png);"));
        label_logo->setAlignment(Qt::AlignCenter);
        label_3 = new QLabel(DetectUI);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(0, -1, 1441, 41));
        label_3->setStyleSheet(QStringLiteral("background-color: rgb(230, 240, 252);"));
        label_title = new QLabel(DetectUI);
        label_title->setObjectName(QStringLiteral("label_title"));
        label_title->setGeometry(QRect(570, 0, 301, 41));
        sizePolicy.setHeightForWidth(label_title->sizePolicy().hasHeightForWidth());
        label_title->setSizePolicy(sizePolicy);
        label_title->setMinimumSize(QSize(60, 30));
        QFont font1;
        font1.setFamily(QStringLiteral("SimSun-ExtB FB"));
        font1.setPointSize(22);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(25);
        label_title->setFont(font1);
        label_title->setStyleSheet(QLatin1String("font: 200 22pt \"SimSun-ExtB FB\";\n"
""));
        label_title->setAlignment(Qt::AlignCenter);
        graphicsView = new QGraphicsView(DetectUI);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setGeometry(QRect(0, 40, 1201, 861));
        graphicsView->setStyleSheet(QStringLiteral("background-color: rgb(250, 250, 250);"));
        graphicsView->setFrameShape(QFrame::NoFrame);
        pushButton_start = new QPushButton(DetectUI);
        pushButton_start->setObjectName(QStringLiteral("pushButton_start"));
        pushButton_start->setGeometry(QRect(1260, 420, 112, 40));
        sizePolicy.setHeightForWidth(pushButton_start->sizePolicy().hasHeightForWidth());
        pushButton_start->setSizePolicy(sizePolicy);
        QFont font2;
        font2.setPointSize(16);
        pushButton_start->setFont(font2);
        pushButton_start->setStyleSheet(QStringLiteral(""));
        pushButton_return = new QPushButton(DetectUI);
        pushButton_return->setObjectName(QStringLiteral("pushButton_return"));
        pushButton_return->setGeometry(QRect(1260, 490, 112, 40));
        sizePolicy.setHeightForWidth(pushButton_return->sizePolicy().hasHeightForWidth());
        pushButton_return->setSizePolicy(sizePolicy);
        pushButton_return->setFont(font2);
        label_status = new QLabel(DetectUI);
        label_status->setObjectName(QStringLiteral("label_status"));
        label_status->setGeometry(QRect(1220, 800, 201, 51));
        QFont font3;
        font3.setPointSize(15);
        label_status->setFont(font3);
        label_status->setFrameShape(QFrame::StyledPanel);
        label_status->setFrameShadow(QFrame::Raised);
        label_status->setAlignment(Qt::AlignCenter);
        label = new QLabel(DetectUI);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(1260, 220, 111, 31));
        QFont font4;
        font4.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font4.setPointSize(19);
        font4.setBold(false);
        font4.setItalic(false);
        font4.setWeight(50);
        label->setFont(font4);
        label->setStyleSheet(QString::fromUtf8("font: 19pt \"\346\245\267\344\275\223\";"));
        label->setAlignment(Qt::AlignCenter);
        layoutWidget = new QWidget(DetectUI);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(1250, 700, 141, 61));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label_indicator = new QLabel(layoutWidget);
        label_indicator->setObjectName(QStringLiteral("label_indicator"));
        sizePolicy.setHeightForWidth(label_indicator->sizePolicy().hasHeightForWidth());
        label_indicator->setSizePolicy(sizePolicy);
        label_indicator->setMinimumSize(QSize(50, 50));
        QFont font5;
        font5.setPointSize(14);
        label_indicator->setFont(font5);
        label_indicator->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label_indicator);

        horizontalSpacer = new QSpacerItem(3, 5, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label_result = new QLabel(layoutWidget);
        label_result->setObjectName(QStringLiteral("label_result"));
        QFont font6;
        font6.setPointSize(18);
        font6.setBold(false);
        font6.setWeight(50);
        label_result->setFont(font6);
        label_result->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label_result);

        layoutWidget->raise();
        label_3->raise();
        label_title->raise();
        label_logo->raise();
        graphicsView->raise();
        pushButton_start->raise();
        pushButton_return->raise();
        label_status->raise();
        line->raise();
        label->raise();

        retranslateUi(DetectUI);

        QMetaObject::connectSlotsByName(DetectUI);
    } // setupUi

    void retranslateUi(QWidget *DetectUI)
    {
        DetectUI->setWindowTitle(QApplication::translate("DetectUI", "DetectUI", nullptr));
        label_logo->setText(QString());
        label_3->setText(QString());
        label_title->setText(QApplication::translate("DetectUI", "PCB\346\243\200\346\265\213\347\263\273\347\273\237 - \346\243\200\346\265\213", nullptr));
        pushButton_start->setText(QApplication::translate("DetectUI", "\345\274\200\345\247\213", nullptr));
        pushButton_return->setText(QApplication::translate("DetectUI", "\350\277\224\345\233\236", nullptr));
        label_status->setText(QApplication::translate("DetectUI", "\347\212\266\346\200\201\346\240\217", nullptr));
        label->setText(QApplication::translate("DetectUI", "\347\272\242\347\202\271\347\262\276\345\267\245", nullptr));
        label_indicator->setText(QApplication::translate("DetectUI", "LED", nullptr));
        label_result->setText(QApplication::translate("DetectUI", "--", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DetectUI: public Ui_DetectUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DETECTUI_H
