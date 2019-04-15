/********************************************************************************
** Form generated from reading UI file 'sctp_graph_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCTP_GRAPH_DIALOG_H
#define UI_SCTP_GRAPH_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include "widgets/qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_SCTPGraphDialog
{
public:
    QAction *actionGoToPacket;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QCustomPlot *sctpPlot;
    QLabel *hintLabel;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *saveButton;
    QSpacerItem *horizontalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SCTPGraphDialog)
    {
        if (SCTPGraphDialog->objectName().isEmpty())
            SCTPGraphDialog->setObjectName(QString::fromUtf8("SCTPGraphDialog"));
        SCTPGraphDialog->resize(800, 546);
        actionGoToPacket = new QAction(SCTPGraphDialog);
        actionGoToPacket->setObjectName(QString::fromUtf8("actionGoToPacket"));
        verticalLayout_2 = new QVBoxLayout(SCTPGraphDialog);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        sctpPlot = new QCustomPlot(SCTPGraphDialog);
        sctpPlot->setObjectName(QString::fromUtf8("sctpPlot"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(sctpPlot->sizePolicy().hasHeightForWidth());
        sctpPlot->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(sctpPlot);

        hintLabel = new QLabel(SCTPGraphDialog);
        hintLabel->setObjectName(QString::fromUtf8("hintLabel"));
        hintLabel->setWordWrap(true);

        verticalLayout->addWidget(hintLabel);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton = new QPushButton(SCTPGraphDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(SCTPGraphDialog);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(SCTPGraphDialog);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(pushButton_3);

        pushButton_4 = new QPushButton(SCTPGraphDialog);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(pushButton_4);

        saveButton = new QPushButton(SCTPGraphDialog);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));

        horizontalLayout->addWidget(saveButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        buttonBox = new QDialogButtonBox(SCTPGraphDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setFocusPolicy(Qt::NoFocus);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close);

        horizontalLayout->addWidget(buttonBox);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(SCTPGraphDialog);
        QObject::connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), SCTPGraphDialog, SLOT(close()));

        QMetaObject::connectSlotsByName(SCTPGraphDialog);
    } // setupUi

    void retranslateUi(QDialog *SCTPGraphDialog)
    {
        SCTPGraphDialog->setWindowTitle(QApplication::translate("SCTPGraphDialog", "SCTP Graph", nullptr));
        actionGoToPacket->setText(QApplication::translate("SCTPGraphDialog", "goToPacket", nullptr));
#ifndef QT_NO_TOOLTIP
        actionGoToPacket->setToolTip(QApplication::translate("SCTPGraphDialog", "Go to Packet", nullptr));
#endif // QT_NO_TOOLTIP
        hintLabel->setText(QApplication::translate("SCTPGraphDialog", "<html><head/><body><p><br/></p></body></html>", nullptr));
        pushButton->setText(QApplication::translate("SCTPGraphDialog", "Only SACKs", nullptr));
        pushButton_2->setText(QApplication::translate("SCTPGraphDialog", "Only TSNs", nullptr));
        pushButton_3->setText(QApplication::translate("SCTPGraphDialog", "Show both", nullptr));
        pushButton_4->setText(QApplication::translate("SCTPGraphDialog", "Reset to full size", nullptr));
        saveButton->setText(QApplication::translate("SCTPGraphDialog", "Save Graph", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SCTPGraphDialog: public Ui_SCTPGraphDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCTP_GRAPH_DIALOG_H
