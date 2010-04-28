/********************************************************************************
** Form generated from reading UI file 'ban_idsem6204.ui'
**
** Created: Tue Jan 19 00:45:48 2010
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef BAN_IDSEM6204_H
#define BAN_IDSEM6204_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QMainWindow>
#include <QtGui/QScrollArea>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGroupBox *groupBox;
    QCheckBox *checkBoxCdRom;
    QLineEdit *lineEditProcessor;
    QCheckBox *checkBoxProcessor;
    QLineEdit *lineEditCdRom;
    QLabel *label_16;
    QLabel *label_15;
    QCheckBox *checkBoxMotherboardSerial;
    QLabel *label_14;
    QLineEdit *lineEditMotherboard;
    QLabel *label_13;
    QListView *listViewSerials;
    QLabel *label_17;
    QCheckBox *checkBoxSystemUUID;
    QLineEdit *lineEditSystemUUID;
    QGroupBox *groupBox_3;
    QCheckBox *checkBoxFile2;
    QLabel *label_10;
    QLineEdit *lineEditFileKey2;
    QCheckBox *checkBoxFile1;
    QLabel *label_9;
    QLineEdit *lineEditFileKey1;
    QLineEdit *lineEditRegKey1;
    QLineEdit *lineEditRegKey2;
    QCheckBox *checkBoxReg2;
    QLabel *label_7;
    QLabel *label_8;
    QCheckBox *checkBoxReg1;
    QGroupBox *groupBox_2;
    QCheckBox *checkBoxMachineGuid;
    QLabel *label_12;
    QLineEdit *lineEditProdId;
    QLineEdit *lineEditMachineGUID;
    QCheckBox *checkBoxProductId;
    QLabel *label_11;
    QCheckBox *checkBoxUserName;
    QLabel *label_3;
    QLineEdit *lineEditNETBIOS;
    QCheckBox *checkBoxNetBIOS;
    QLabel *label_5;
    QCheckBox *checkBoxMACAddress;
    QLabel *label_6;
    QLineEdit *lineEditMACAddress;
    QLineEdit *lineEditVolumeSerial;
    QCheckBox *checkBoxVSN;
    QLabel *label_2;
    QLineEdit *lineEditUserName;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(582, 741);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(0, 0));
        MainWindow->setMaximumSize(QSize(5790, 7920));
        QFont font;
        font.setPointSize(12);
        MainWindow->setFont(font);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        scrollArea = new QScrollArea(centralwidget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        sizePolicy.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy);
        scrollArea->setAutoFillBackground(false);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setWidgetResizable(true);
        scrollArea->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 562, 701));
        QSizePolicy sizePolicy2(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(scrollAreaWidgetContents->sizePolicy().hasHeightForWidth());
        scrollAreaWidgetContents->setSizePolicy(sizePolicy2);
        scrollAreaWidgetContents->setMinimumSize(QSize(562, 701));
        scrollAreaWidgetContents->setBaseSize(QSize(0, 0));
        groupBox = new QGroupBox(scrollAreaWidgetContents);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(0, 0, 561, 331));
        checkBoxCdRom = new QCheckBox(groupBox);
        checkBoxCdRom->setObjectName(QString::fromUtf8("checkBoxCdRom"));
        checkBoxCdRom->setEnabled(false);
        checkBoxCdRom->setGeometry(QRect(470, 120, 70, 21));
        checkBoxCdRom->setCheckable(true);
        lineEditProcessor = new QLineEdit(groupBox);
        lineEditProcessor->setObjectName(QString::fromUtf8("lineEditProcessor"));
        lineEditProcessor->setGeometry(QRect(220, 90, 241, 20));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Courier New"));
        lineEditProcessor->setFont(font1);
        checkBoxProcessor = new QCheckBox(groupBox);
        checkBoxProcessor->setObjectName(QString::fromUtf8("checkBoxProcessor"));
        checkBoxProcessor->setEnabled(false);
        checkBoxProcessor->setGeometry(QRect(470, 90, 70, 21));
        checkBoxProcessor->setCheckable(true);
        lineEditCdRom = new QLineEdit(groupBox);
        lineEditCdRom->setObjectName(QString::fromUtf8("lineEditCdRom"));
        lineEditCdRom->setGeometry(QRect(220, 120, 241, 20));
        lineEditCdRom->setFont(font1);
        label_16 = new QLabel(groupBox);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setGeometry(QRect(20, 90, 191, 21));
        label_16->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_15 = new QLabel(groupBox);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(20, 120, 191, 21));
        label_15->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        checkBoxMotherboardSerial = new QCheckBox(groupBox);
        checkBoxMotherboardSerial->setObjectName(QString::fromUtf8("checkBoxMotherboardSerial"));
        checkBoxMotherboardSerial->setEnabled(false);
        checkBoxMotherboardSerial->setGeometry(QRect(470, 60, 70, 21));
        checkBoxMotherboardSerial->setCheckable(true);
        label_14 = new QLabel(groupBox);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(20, 60, 191, 21));
        label_14->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditMotherboard = new QLineEdit(groupBox);
        lineEditMotherboard->setObjectName(QString::fromUtf8("lineEditMotherboard"));
        lineEditMotherboard->setGeometry(QRect(220, 60, 241, 20));
        lineEditMotherboard->setFont(font1);
        label_13 = new QLabel(groupBox);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(20, 150, 191, 21));
        label_13->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        listViewSerials = new QListView(groupBox);
        listViewSerials->setObjectName(QString::fromUtf8("listViewSerials"));
        listViewSerials->setGeometry(QRect(220, 150, 331, 171));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Courier New"));
        font2.setPointSize(12);
        listViewSerials->setFont(font2);
        label_17 = new QLabel(groupBox);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(20, 30, 191, 21));
        label_17->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        checkBoxSystemUUID = new QCheckBox(groupBox);
        checkBoxSystemUUID->setObjectName(QString::fromUtf8("checkBoxSystemUUID"));
        checkBoxSystemUUID->setEnabled(false);
        checkBoxSystemUUID->setGeometry(QRect(470, 30, 70, 21));
        checkBoxSystemUUID->setCheckable(true);
        lineEditSystemUUID = new QLineEdit(groupBox);
        lineEditSystemUUID->setObjectName(QString::fromUtf8("lineEditSystemUUID"));
        lineEditSystemUUID->setGeometry(QRect(220, 30, 241, 20));
        lineEditSystemUUID->setFont(font1);
        groupBox_3 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(0, 550, 561, 151));
        checkBoxFile2 = new QCheckBox(groupBox_3);
        checkBoxFile2->setObjectName(QString::fromUtf8("checkBoxFile2"));
        checkBoxFile2->setEnabled(false);
        checkBoxFile2->setGeometry(QRect(470, 120, 70, 21));
        checkBoxFile2->setCheckable(true);
        checkBoxFile2->setAutoExclusive(false);
        label_10 = new QLabel(groupBox_3);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(20, 120, 191, 21));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditFileKey2 = new QLineEdit(groupBox_3);
        lineEditFileKey2->setObjectName(QString::fromUtf8("lineEditFileKey2"));
        lineEditFileKey2->setGeometry(QRect(220, 120, 241, 20));
        lineEditFileKey2->setFont(font1);
        checkBoxFile1 = new QCheckBox(groupBox_3);
        checkBoxFile1->setObjectName(QString::fromUtf8("checkBoxFile1"));
        checkBoxFile1->setEnabled(false);
        checkBoxFile1->setGeometry(QRect(470, 90, 70, 21));
        checkBoxFile1->setCheckable(true);
        label_9 = new QLabel(groupBox_3);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(20, 90, 191, 21));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditFileKey1 = new QLineEdit(groupBox_3);
        lineEditFileKey1->setObjectName(QString::fromUtf8("lineEditFileKey1"));
        lineEditFileKey1->setGeometry(QRect(220, 90, 241, 20));
        lineEditFileKey1->setFont(font1);
        lineEditRegKey1 = new QLineEdit(groupBox_3);
        lineEditRegKey1->setObjectName(QString::fromUtf8("lineEditRegKey1"));
        lineEditRegKey1->setGeometry(QRect(220, 30, 241, 20));
        lineEditRegKey1->setFont(font1);
        lineEditRegKey2 = new QLineEdit(groupBox_3);
        lineEditRegKey2->setObjectName(QString::fromUtf8("lineEditRegKey2"));
        lineEditRegKey2->setGeometry(QRect(220, 60, 241, 20));
        lineEditRegKey2->setFont(font1);
        checkBoxReg2 = new QCheckBox(groupBox_3);
        checkBoxReg2->setObjectName(QString::fromUtf8("checkBoxReg2"));
        checkBoxReg2->setEnabled(false);
        checkBoxReg2->setGeometry(QRect(470, 60, 70, 21));
        checkBoxReg2->setCheckable(true);
        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(20, 30, 191, 21));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(20, 60, 191, 21));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        checkBoxReg1 = new QCheckBox(groupBox_3);
        checkBoxReg1->setObjectName(QString::fromUtf8("checkBoxReg1"));
        checkBoxReg1->setEnabled(false);
        checkBoxReg1->setGeometry(QRect(470, 30, 70, 21));
        checkBoxReg1->setCheckable(true);
        groupBox_2 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(0, 330, 561, 211));
        checkBoxMachineGuid = new QCheckBox(groupBox_2);
        checkBoxMachineGuid->setObjectName(QString::fromUtf8("checkBoxMachineGuid"));
        checkBoxMachineGuid->setEnabled(false);
        checkBoxMachineGuid->setGeometry(QRect(470, 60, 70, 21));
        checkBoxMachineGuid->setCheckable(true);
        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(20, 60, 191, 21));
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditProdId = new QLineEdit(groupBox_2);
        lineEditProdId->setObjectName(QString::fromUtf8("lineEditProdId"));
        lineEditProdId->setGeometry(QRect(220, 30, 241, 20));
        lineEditProdId->setFont(font1);
        lineEditMachineGUID = new QLineEdit(groupBox_2);
        lineEditMachineGUID->setObjectName(QString::fromUtf8("lineEditMachineGUID"));
        lineEditMachineGUID->setGeometry(QRect(220, 60, 241, 20));
        lineEditMachineGUID->setFont(font1);
        checkBoxProductId = new QCheckBox(groupBox_2);
        checkBoxProductId->setObjectName(QString::fromUtf8("checkBoxProductId"));
        checkBoxProductId->setEnabled(false);
        checkBoxProductId->setGeometry(QRect(470, 30, 70, 21));
        checkBoxProductId->setCheckable(true);
        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(20, 30, 191, 21));
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        checkBoxUserName = new QCheckBox(groupBox_2);
        checkBoxUserName->setObjectName(QString::fromUtf8("checkBoxUserName"));
        checkBoxUserName->setEnabled(false);
        checkBoxUserName->setGeometry(QRect(470, 180, 70, 21));
        checkBoxUserName->setCheckable(true);
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 120, 191, 21));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditNETBIOS = new QLineEdit(groupBox_2);
        lineEditNETBIOS->setObjectName(QString::fromUtf8("lineEditNETBIOS"));
        lineEditNETBIOS->setGeometry(QRect(220, 150, 241, 20));
        lineEditNETBIOS->setFont(font1);
        checkBoxNetBIOS = new QCheckBox(groupBox_2);
        checkBoxNetBIOS->setObjectName(QString::fromUtf8("checkBoxNetBIOS"));
        checkBoxNetBIOS->setEnabled(false);
        checkBoxNetBIOS->setGeometry(QRect(470, 150, 70, 21));
        checkBoxNetBIOS->setCheckable(true);
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(20, 150, 191, 21));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        checkBoxMACAddress = new QCheckBox(groupBox_2);
        checkBoxMACAddress->setObjectName(QString::fromUtf8("checkBoxMACAddress"));
        checkBoxMACAddress->setEnabled(false);
        checkBoxMACAddress->setGeometry(QRect(470, 120, 70, 21));
        checkBoxMACAddress->setCheckable(true);
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(20, 180, 191, 21));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditMACAddress = new QLineEdit(groupBox_2);
        lineEditMACAddress->setObjectName(QString::fromUtf8("lineEditMACAddress"));
        lineEditMACAddress->setGeometry(QRect(220, 120, 241, 20));
        lineEditMACAddress->setFont(font1);
        lineEditVolumeSerial = new QLineEdit(groupBox_2);
        lineEditVolumeSerial->setObjectName(QString::fromUtf8("lineEditVolumeSerial"));
        lineEditVolumeSerial->setGeometry(QRect(220, 90, 241, 20));
        lineEditVolumeSerial->setFont(font1);
        checkBoxVSN = new QCheckBox(groupBox_2);
        checkBoxVSN->setObjectName(QString::fromUtf8("checkBoxVSN"));
        checkBoxVSN->setEnabled(false);
        checkBoxVSN->setGeometry(QRect(470, 90, 70, 21));
        checkBoxVSN->setCheckable(true);
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 90, 191, 21));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditUserName = new QLineEdit(groupBox_2);
        lineEditUserName->setObjectName(QString::fromUtf8("lineEditUserName"));
        lineEditUserName->setGeometry(QRect(220, 180, 241, 20));
        lineEditUserName->setFont(font1);
        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout->addWidget(scrollArea, 0, 1, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Splane - Ban ID Viewer", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Hardware Info", 0, QApplication::UnicodeUTF8));
        checkBoxCdRom->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        checkBoxProcessor->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("MainWindow", "Processor Info Hash:", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("MainWindow", "CD-ROM Info Hash:", 0, QApplication::UnicodeUTF8));
        checkBoxMotherboardSerial->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("MainWindow", "Motherboard Info Hash:", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("MainWindow", "Harddrive Info Hash:", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("MainWindow", "System UUID Hash:", 0, QApplication::UnicodeUTF8));
        checkBoxSystemUUID->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Generated Identifiers", 0, QApplication::UnicodeUTF8));
        checkBoxFile2->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MainWindow", "Hidden File Key 2:", 0, QApplication::UnicodeUTF8));
        checkBoxFile1->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainWindow", "Hidden File Key 1:", 0, QApplication::UnicodeUTF8));
        checkBoxReg2->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "Hidden Registry Key 1:", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "Hidden Registry Key 2:", 0, QApplication::UnicodeUTF8));
        checkBoxReg1->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "OS Info", 0, QApplication::UnicodeUTF8));
        checkBoxMachineGuid->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("MainWindow", "Crypto MachineGuid Hash:", 0, QApplication::UnicodeUTF8));
        checkBoxProductId->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MainWindow", "Windows Product ID Hash:", 0, QApplication::UnicodeUTF8));
        checkBoxUserName->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "First MAC Address Hash:", 0, QApplication::UnicodeUTF8));
        checkBoxNetBIOS->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "NetBIOS Name Hash:", 0, QApplication::UnicodeUTF8));
        checkBoxMACAddress->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "User Name Hash:", 0, QApplication::UnicodeUTF8));
        checkBoxVSN->setText(QApplication::translate("MainWindow", "Strong", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "System Volume Serial:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // BAN_IDSEM6204_H
