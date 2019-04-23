/********************************************************************************
** Form generated from reading UI file 'TemplateUI.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEMPLATEUI_H
#define UI_TEMPLATEUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TemplateUI
{
public:
    QFrame *line;
    QLabel *label_title;
    QLabel *label_3;
    QLabel *label_logo;
    QLabel *label;
    QPushButton *pushButton_start;
    QPushButton *pushButton_return;
    QLabel *label_status;
    QGraphicsView *graphicsView;

    void setupUi(QWidget *TemplateUI)
    {
        if (TemplateUI->objectName().isEmpty())
            TemplateUI->setObjectName(QStringLiteral("TemplateUI"));
        TemplateUI->resize(1440, 900);
        line = new QFrame(TemplateUI);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(0, 30, 1441, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        label_title = new QLabel(TemplateUI);
        label_title->setObjectName(QStringLiteral("label_title"));
        label_title->setGeometry(QRect(560, 1, 331, 41));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
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
        label_3 = new QLabel(TemplateUI);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(0, 0, 1441, 41));
        label_3->setStyleSheet(QStringLiteral("background-color: rgb(240, 242, 252);"));
        label_logo = new QLabel(TemplateUI);
        label_logo->setObjectName(QStringLiteral("label_logo"));
        label_logo->setGeometry(QRect(1209, 129, 171, 121));
        sizePolicy.setHeightForWidth(label_logo->sizePolicy().hasHeightForWidth());
        label_logo->setSizePolicy(sizePolicy);
        label_logo->setMaximumSize(QSize(16777215, 16000000));
        QFont font1;
        font1.setPointSize(16);
        font1.setBold(false);
        font1.setWeight(50);
        label_logo->setFont(font1);
        label_logo->setStyleSheet(QStringLiteral("border-image: url(:/PCBDetect/Icons/logo.png);"));
        label_logo->setAlignment(Qt::AlignCenter);
        label = new QLabel(TemplateUI);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(1240, 260, 111, 31));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font2.setPointSize(19);
        font2.setBold(false);
        font2.setItalic(false);
        font2.setWeight(50);
        label->setFont(font2);
        label->setStyleSheet(QString::fromUtf8("font: 19pt \"\346\245\267\344\275\223\";"));
        label->setAlignment(Qt::AlignCenter);
        pushButton_start = new QPushButton(TemplateUI);
        pushButton_start->setObjectName(QStringLiteral("pushButton_start"));
        pushButton_start->setGeometry(QRect(1230, 500, 131, 40));
        sizePolicy.setHeightForWidth(pushButton_start->sizePolicy().hasHeightForWidth());
        pushButton_start->setSizePolicy(sizePolicy);
        QFont font3;
        font3.setPointSize(16);
        pushButton_start->setFont(font3);
        pushButton_return = new QPushButton(TemplateUI);
        pushButton_return->setObjectName(QStringLiteral("pushButton_return"));
        pushButton_return->setGeometry(QRect(1230, 580, 131, 40));
        sizePolicy.setHeightForWidth(pushButton_return->sizePolicy().hasHeightForWidth());
        pushButton_return->setSizePolicy(sizePolicy);
        pushButton_return->setFont(font3);
        label_status = new QLabel(TemplateUI);
        label_status->setObjectName(QStringLiteral("label_status"));
        label_status->setGeometry(QRect(1190, 790, 221, 60));
        QFont font4;
        font4.setPointSize(15);
        label_status->setFont(font4);
        label_status->setFrameShape(QFrame::StyledPanel);
        label_status->setFrameShadow(QFrame::Raised);
        label_status->setAlignment(Qt::AlignCenter);
        graphicsView = new QGraphicsView(TemplateUI);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setGeometry(QRect(0, 40, 1161, 861));
        graphicsView->setStyleSheet(QStringLiteral("background-color: rgb(250, 250, 250);"));
        graphicsView->setFrameShape(QFrame::NoFrame);
        label_3->raise();
        label_title->raise();
        label_logo->raise();
        label->raise();
        pushButton_start->raise();
        pushButton_return->raise();
        label_status->raise();
        graphicsView->raise();
        line->raise();

        retranslateUi(TemplateUI);

        QMetaObject::connectSlotsByName(TemplateUI);
    } // setupUi

    void retranslateUi(QWidget *TemplateUI)
    {
        TemplateUI->setWindowTitle(QApplication::translate("TemplateUI", "TemplateUI", nullptr));
        label_title->setText(QApplication::translate("TemplateUI", "PCB\346\243\200\346\265\213\347\263\273\347\273\237 - \346\250\241\346\235\277\346\217\220\345\217\226", nullptr));
        label_3->setText(QString());
        label_logo->setText(QString());
        label->setText(QApplication::translate("TemplateUI", "\347\272\242\347\202\271\347\262\276\345\267\245", nullptr));
        pushButton_start->setText(QApplication::translate("TemplateUI", "\345\274\200\345\247\213", nullptr));
        pushButton_return->setText(QApplication::translate("TemplateUI", "\350\277\224\345\233\236", nullptr));
        label_status->setText(QApplication::translate("TemplateUI", "\347\212\266\346\200\201\346\240\217", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TemplateUI: public Ui_TemplateUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEMPLATEUI_H
