/********************************************************************************
** Form generated from reading UI file 'mainpd4224.ui'
**
** Created: Mon Feb 15 20:15:40 2010
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MAINPD4224_H
#define MAINPD4224_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTableView>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginAdminForm
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *tabServer;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBoxAddress;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEditIP;
    QLabel *label;
    QLineEdit *lineEditPort;
    QGroupBox *groupBoxPubKey;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *lineEditPublicKey;
    QPushButton *pushButtonBrowsePubKey;
    QGroupBox *groupBoxPassword;
    QGridLayout *gridLayout_5;
    QLabel *label_3;
    QLineEdit *lineEditUserName;
    QLabel *label_2;
    QLineEdit *lineEditPassword;
    QLabel *label_4;
    QLineEdit *lineEditAlias;
    QGroupBox *groupBoxKeyFile;
    QGridLayout *gridLayout_6;
    QLineEdit *lineEditKeyFilePath;
    QPushButton *pushButtonBrowseKeyFile;
    QCheckBox *checkBoxUseKeyFile;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButtonDisconnect;
    QPushButton *pushButtonConnect;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QTextEdit *textEditStatus;
    QWidget *tabChat;
    QGridLayout *gridLayout_2;
    QTextEdit *textEditChat;
    QLineEdit *lineEditChat;
    QListView *listViewChatUsers;
    QWidget *tabActive;
    QVBoxLayout *verticalLayout_2;
    QListView *listViewActiveUsers;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_7;
    QPushButton *pushButtonKickActiveUser;
    QPushButton *pushButtonBanActiveUser;
    QTableView *tableViewSelectedUser;
    QWidget *tabUsers;
    QVBoxLayout *verticalLayout_3;
    QListView *listViewAccounts;
    QGroupBox *groupBox_7;
    QGridLayout *gridLayout_8;
    QPushButton *pushButtonBanAccount;
    QTableView *tableViewSelectedAccount;
    QWidget *tabBans;
    QVBoxLayout *verticalLayout_4;
    QListView *listViewBans;
    QGroupBox *groupBox_8;
    QGridLayout *gridLayout_9;
    QPushButton *pushButtonUnban;
    QTableView *tableViewSelectedBan;
    QWidget *tabZones;
    QVBoxLayout *verticalLayout_5;
    QListView *listViewZones;
    QGroupBox *groupBox_9;
    QGridLayout *gridLayout_10;
    QPushButton *pushButtonDeleteServer;
    QTableView *tableViewSelectedZone;
    QWidget *tabLog;
    QGridLayout *gridLayout_3;
    QTextEdit *textMonitorLog;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *LoginAdminForm)
    {
        if (LoginAdminForm->objectName().isEmpty())
            LoginAdminForm->setObjectName(QString::fromUtf8("LoginAdminForm"));
        LoginAdminForm->resize(927, 871);
        QFont font;
        font.setFamily(QString::fromUtf8("Terminal"));
        font.setPointSize(12);
        LoginAdminForm->setFont(font);
        centralwidget = new QWidget(LoginAdminForm);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setAutoFillBackground(true);
        tabServer = new QWidget();
        tabServer->setObjectName(QString::fromUtf8("tabServer"));
        tabServer->setAutoFillBackground(true);
        verticalLayout = new QVBoxLayout(tabServer);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBoxAddress = new QGroupBox(tabServer);
        groupBoxAddress->setObjectName(QString::fromUtf8("groupBoxAddress"));
        groupBoxAddress->setMaximumSize(QSize(16777215, 65));
        groupBoxAddress->setAutoFillBackground(true);
        horizontalLayout = new QHBoxLayout(groupBoxAddress);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineEditIP = new QLineEdit(groupBoxAddress);
        lineEditIP->setObjectName(QString::fromUtf8("lineEditIP"));

        horizontalLayout->addWidget(lineEditIP);

        label = new QLabel(groupBoxAddress);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        lineEditPort = new QLineEdit(groupBoxAddress);
        lineEditPort->setObjectName(QString::fromUtf8("lineEditPort"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lineEditPort->sizePolicy().hasHeightForWidth());
        lineEditPort->setSizePolicy(sizePolicy);
        lineEditPort->setMinimumSize(QSize(0, 0));
        lineEditPort->setMaximumSize(QSize(100, 16777215));
        lineEditPort->setMaxLength(32);
        lineEditPort->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(lineEditPort);


        verticalLayout->addWidget(groupBoxAddress);

        groupBoxPubKey = new QGroupBox(tabServer);
        groupBoxPubKey->setObjectName(QString::fromUtf8("groupBoxPubKey"));
        groupBoxPubKey->setAutoFillBackground(true);
        horizontalLayout_4 = new QHBoxLayout(groupBoxPubKey);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        lineEditPublicKey = new QLineEdit(groupBoxPubKey);
        lineEditPublicKey->setObjectName(QString::fromUtf8("lineEditPublicKey"));

        horizontalLayout_4->addWidget(lineEditPublicKey);

        pushButtonBrowsePubKey = new QPushButton(groupBoxPubKey);
        pushButtonBrowsePubKey->setObjectName(QString::fromUtf8("pushButtonBrowsePubKey"));

        horizontalLayout_4->addWidget(pushButtonBrowsePubKey);


        verticalLayout->addWidget(groupBoxPubKey);

        groupBoxPassword = new QGroupBox(tabServer);
        groupBoxPassword->setObjectName(QString::fromUtf8("groupBoxPassword"));
        groupBoxPassword->setAutoFillBackground(true);
        gridLayout_5 = new QGridLayout(groupBoxPassword);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        label_3 = new QLabel(groupBoxPassword);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_3, 0, 0, 1, 1);

        lineEditUserName = new QLineEdit(groupBoxPassword);
        lineEditUserName->setObjectName(QString::fromUtf8("lineEditUserName"));

        gridLayout_5->addWidget(lineEditUserName, 0, 2, 1, 1);

        label_2 = new QLabel(groupBoxPassword);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_2, 2, 0, 1, 1);

        lineEditPassword = new QLineEdit(groupBoxPassword);
        lineEditPassword->setObjectName(QString::fromUtf8("lineEditPassword"));
        lineEditPassword->setEchoMode(QLineEdit::Password);

        gridLayout_5->addWidget(lineEditPassword, 2, 2, 1, 1);

        label_4 = new QLabel(groupBoxPassword);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_4, 1, 0, 1, 1);

        lineEditAlias = new QLineEdit(groupBoxPassword);
        lineEditAlias->setObjectName(QString::fromUtf8("lineEditAlias"));

        gridLayout_5->addWidget(lineEditAlias, 1, 2, 1, 1);


        verticalLayout->addWidget(groupBoxPassword);

        groupBoxKeyFile = new QGroupBox(tabServer);
        groupBoxKeyFile->setObjectName(QString::fromUtf8("groupBoxKeyFile"));
        groupBoxKeyFile->setAutoFillBackground(true);
        gridLayout_6 = new QGridLayout(groupBoxKeyFile);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        lineEditKeyFilePath = new QLineEdit(groupBoxKeyFile);
        lineEditKeyFilePath->setObjectName(QString::fromUtf8("lineEditKeyFilePath"));
        lineEditKeyFilePath->setEnabled(false);

        gridLayout_6->addWidget(lineEditKeyFilePath, 1, 1, 1, 1);

        pushButtonBrowseKeyFile = new QPushButton(groupBoxKeyFile);
        pushButtonBrowseKeyFile->setObjectName(QString::fromUtf8("pushButtonBrowseKeyFile"));
        pushButtonBrowseKeyFile->setEnabled(false);

        gridLayout_6->addWidget(pushButtonBrowseKeyFile, 1, 2, 1, 1);

        checkBoxUseKeyFile = new QCheckBox(groupBoxKeyFile);
        checkBoxUseKeyFile->setObjectName(QString::fromUtf8("checkBoxUseKeyFile"));
        checkBoxUseKeyFile->setChecked(false);

        gridLayout_6->addWidget(checkBoxUseKeyFile, 0, 1, 1, 1);


        verticalLayout->addWidget(groupBoxKeyFile);

        widget = new QWidget(tabServer);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setAutoFillBackground(true);
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pushButtonDisconnect = new QPushButton(widget);
        pushButtonDisconnect->setObjectName(QString::fromUtf8("pushButtonDisconnect"));
        pushButtonDisconnect->setEnabled(false);

        horizontalLayout_2->addWidget(pushButtonDisconnect);

        pushButtonConnect = new QPushButton(widget);
        pushButtonConnect->setObjectName(QString::fromUtf8("pushButtonConnect"));
        pushButtonConnect->setAutoDefault(false);
        pushButtonConnect->setDefault(true);

        horizontalLayout_2->addWidget(pushButtonConnect);


        verticalLayout->addWidget(widget);

        groupBox_2 = new QGroupBox(tabServer);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        textEditStatus = new QTextEdit(groupBox_2);
        textEditStatus->setObjectName(QString::fromUtf8("textEditStatus"));
        QFont font1;
        font1.setPointSize(8);
        textEditStatus->setFont(font1);
        textEditStatus->setReadOnly(true);

        gridLayout_4->addWidget(textEditStatus, 0, 0, 1, 1);


        verticalLayout->addWidget(groupBox_2);

        tabWidget->addTab(tabServer, QString());
        tabChat = new QWidget();
        tabChat->setObjectName(QString::fromUtf8("tabChat"));
        tabChat->setAutoFillBackground(true);
        gridLayout_2 = new QGridLayout(tabChat);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        textEditChat = new QTextEdit(tabChat);
        textEditChat->setObjectName(QString::fromUtf8("textEditChat"));
        textEditChat->setReadOnly(true);

        gridLayout_2->addWidget(textEditChat, 0, 0, 1, 1);

        lineEditChat = new QLineEdit(tabChat);
        lineEditChat->setObjectName(QString::fromUtf8("lineEditChat"));

        gridLayout_2->addWidget(lineEditChat, 1, 0, 1, 2);

        listViewChatUsers = new QListView(tabChat);
        listViewChatUsers->setObjectName(QString::fromUtf8("listViewChatUsers"));
        listViewChatUsers->setMaximumSize(QSize(160, 16777215));
        listViewChatUsers->setAlternatingRowColors(true);

        gridLayout_2->addWidget(listViewChatUsers, 0, 1, 1, 1);

        tabWidget->addTab(tabChat, QString());
        tabActive = new QWidget();
        tabActive->setObjectName(QString::fromUtf8("tabActive"));
        tabActive->setAutoFillBackground(true);
        verticalLayout_2 = new QVBoxLayout(tabActive);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        listViewActiveUsers = new QListView(tabActive);
        listViewActiveUsers->setObjectName(QString::fromUtf8("listViewActiveUsers"));

        verticalLayout_2->addWidget(listViewActiveUsers);

        groupBox_6 = new QGroupBox(tabActive);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setMaximumSize(QSize(16777215, 200));
        gridLayout_7 = new QGridLayout(groupBox_6);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        pushButtonKickActiveUser = new QPushButton(groupBox_6);
        pushButtonKickActiveUser->setObjectName(QString::fromUtf8("pushButtonKickActiveUser"));

        gridLayout_7->addWidget(pushButtonKickActiveUser, 1, 0, 1, 1);

        pushButtonBanActiveUser = new QPushButton(groupBox_6);
        pushButtonBanActiveUser->setObjectName(QString::fromUtf8("pushButtonBanActiveUser"));

        gridLayout_7->addWidget(pushButtonBanActiveUser, 1, 1, 1, 1);

        tableViewSelectedUser = new QTableView(groupBox_6);
        tableViewSelectedUser->setObjectName(QString::fromUtf8("tableViewSelectedUser"));
        tableViewSelectedUser->setMaximumSize(QSize(16777215, 16777215));
        tableViewSelectedUser->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableViewSelectedUser->setProperty("showDropIndicator", QVariant(false));
        tableViewSelectedUser->setDragDropOverwriteMode(false);
        tableViewSelectedUser->setDragDropMode(QAbstractItemView::NoDragDrop);
        tableViewSelectedUser->setAlternatingRowColors(true);
        tableViewSelectedUser->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableViewSelectedUser->setSortingEnabled(true);
        tableViewSelectedUser->horizontalHeader()->setVisible(false);
        tableViewSelectedUser->horizontalHeader()->setStretchLastSection(true);

        gridLayout_7->addWidget(tableViewSelectedUser, 0, 0, 1, 2);


        verticalLayout_2->addWidget(groupBox_6);

        tabWidget->addTab(tabActive, QString());
        tabUsers = new QWidget();
        tabUsers->setObjectName(QString::fromUtf8("tabUsers"));
        tabUsers->setAutoFillBackground(true);
        verticalLayout_3 = new QVBoxLayout(tabUsers);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        listViewAccounts = new QListView(tabUsers);
        listViewAccounts->setObjectName(QString::fromUtf8("listViewAccounts"));

        verticalLayout_3->addWidget(listViewAccounts);

        groupBox_7 = new QGroupBox(tabUsers);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setMaximumSize(QSize(16777215, 200));
        gridLayout_8 = new QGridLayout(groupBox_7);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        pushButtonBanAccount = new QPushButton(groupBox_7);
        pushButtonBanAccount->setObjectName(QString::fromUtf8("pushButtonBanAccount"));

        gridLayout_8->addWidget(pushButtonBanAccount, 1, 0, 1, 2);

        tableViewSelectedAccount = new QTableView(groupBox_7);
        tableViewSelectedAccount->setObjectName(QString::fromUtf8("tableViewSelectedAccount"));
        tableViewSelectedAccount->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableViewSelectedAccount->setProperty("showDropIndicator", QVariant(false));
        tableViewSelectedAccount->setDragDropOverwriteMode(false);
        tableViewSelectedAccount->setDragDropMode(QAbstractItemView::NoDragDrop);
        tableViewSelectedAccount->setAlternatingRowColors(true);
        tableViewSelectedAccount->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableViewSelectedAccount->setSortingEnabled(true);
        tableViewSelectedAccount->horizontalHeader()->setVisible(false);
        tableViewSelectedAccount->horizontalHeader()->setStretchLastSection(true);

        gridLayout_8->addWidget(tableViewSelectedAccount, 0, 0, 1, 2);


        verticalLayout_3->addWidget(groupBox_7);

        tabWidget->addTab(tabUsers, QString());
        tabBans = new QWidget();
        tabBans->setObjectName(QString::fromUtf8("tabBans"));
        tabBans->setAutoFillBackground(true);
        verticalLayout_4 = new QVBoxLayout(tabBans);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        listViewBans = new QListView(tabBans);
        listViewBans->setObjectName(QString::fromUtf8("listViewBans"));

        verticalLayout_4->addWidget(listViewBans);

        groupBox_8 = new QGroupBox(tabBans);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        groupBox_8->setMaximumSize(QSize(16777215, 200));
        gridLayout_9 = new QGridLayout(groupBox_8);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        pushButtonUnban = new QPushButton(groupBox_8);
        pushButtonUnban->setObjectName(QString::fromUtf8("pushButtonUnban"));

        gridLayout_9->addWidget(pushButtonUnban, 1, 0, 1, 2);

        tableViewSelectedBan = new QTableView(groupBox_8);
        tableViewSelectedBan->setObjectName(QString::fromUtf8("tableViewSelectedBan"));
        tableViewSelectedBan->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableViewSelectedBan->setProperty("showDropIndicator", QVariant(false));
        tableViewSelectedBan->setDragDropOverwriteMode(false);
        tableViewSelectedBan->setDragDropMode(QAbstractItemView::NoDragDrop);
        tableViewSelectedBan->setAlternatingRowColors(true);
        tableViewSelectedBan->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableViewSelectedBan->setSortingEnabled(true);
        tableViewSelectedBan->horizontalHeader()->setVisible(false);
        tableViewSelectedBan->horizontalHeader()->setStretchLastSection(true);

        gridLayout_9->addWidget(tableViewSelectedBan, 0, 0, 1, 2);


        verticalLayout_4->addWidget(groupBox_8);

        tabWidget->addTab(tabBans, QString());
        tabZones = new QWidget();
        tabZones->setObjectName(QString::fromUtf8("tabZones"));
        tabZones->setAutoFillBackground(true);
        verticalLayout_5 = new QVBoxLayout(tabZones);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        listViewZones = new QListView(tabZones);
        listViewZones->setObjectName(QString::fromUtf8("listViewZones"));

        verticalLayout_5->addWidget(listViewZones);

        groupBox_9 = new QGroupBox(tabZones);
        groupBox_9->setObjectName(QString::fromUtf8("groupBox_9"));
        groupBox_9->setMaximumSize(QSize(16777215, 200));
        gridLayout_10 = new QGridLayout(groupBox_9);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        pushButtonDeleteServer = new QPushButton(groupBox_9);
        pushButtonDeleteServer->setObjectName(QString::fromUtf8("pushButtonDeleteServer"));

        gridLayout_10->addWidget(pushButtonDeleteServer, 1, 0, 1, 2);

        tableViewSelectedZone = new QTableView(groupBox_9);
        tableViewSelectedZone->setObjectName(QString::fromUtf8("tableViewSelectedZone"));
        tableViewSelectedZone->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableViewSelectedZone->setProperty("showDropIndicator", QVariant(false));
        tableViewSelectedZone->setDragDropOverwriteMode(false);
        tableViewSelectedZone->setDragDropMode(QAbstractItemView::NoDragDrop);
        tableViewSelectedZone->setAlternatingRowColors(true);
        tableViewSelectedZone->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableViewSelectedZone->setSortingEnabled(true);
        tableViewSelectedZone->horizontalHeader()->setVisible(false);
        tableViewSelectedZone->horizontalHeader()->setStretchLastSection(true);

        gridLayout_10->addWidget(tableViewSelectedZone, 0, 0, 1, 2);


        verticalLayout_5->addWidget(groupBox_9);

        tabWidget->addTab(tabZones, QString());
        tabLog = new QWidget();
        tabLog->setObjectName(QString::fromUtf8("tabLog"));
        tabLog->setAutoFillBackground(true);
        gridLayout_3 = new QGridLayout(tabLog);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        textMonitorLog = new QTextEdit(tabLog);
        textMonitorLog->setObjectName(QString::fromUtf8("textMonitorLog"));
        textMonitorLog->setReadOnly(true);

        gridLayout_3->addWidget(textMonitorLog, 0, 0, 1, 1);

        tabWidget->addTab(tabLog, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);

        LoginAdminForm->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(LoginAdminForm);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        LoginAdminForm->setStatusBar(statusbar);
        QWidget::setTabOrder(lineEditPassword, pushButtonConnect);
        QWidget::setTabOrder(pushButtonConnect, lineEditPublicKey);
        QWidget::setTabOrder(lineEditPublicKey, pushButtonBrowsePubKey);
        QWidget::setTabOrder(pushButtonBrowsePubKey, lineEditUserName);
        QWidget::setTabOrder(lineEditUserName, lineEditAlias);
        QWidget::setTabOrder(lineEditAlias, lineEditIP);
        QWidget::setTabOrder(lineEditIP, checkBoxUseKeyFile);
        QWidget::setTabOrder(checkBoxUseKeyFile, lineEditKeyFilePath);
        QWidget::setTabOrder(lineEditKeyFilePath, pushButtonBrowseKeyFile);
        QWidget::setTabOrder(pushButtonBrowseKeyFile, lineEditPort);
        QWidget::setTabOrder(lineEditPort, tabWidget);
        QWidget::setTabOrder(tabWidget, textEditStatus);
        QWidget::setTabOrder(textEditStatus, textEditChat);
        QWidget::setTabOrder(textEditChat, lineEditChat);
        QWidget::setTabOrder(lineEditChat, listViewChatUsers);
        QWidget::setTabOrder(listViewChatUsers, listViewActiveUsers);
        QWidget::setTabOrder(listViewActiveUsers, pushButtonKickActiveUser);
        QWidget::setTabOrder(pushButtonKickActiveUser, pushButtonBanActiveUser);
        QWidget::setTabOrder(pushButtonBanActiveUser, tableViewSelectedUser);
        QWidget::setTabOrder(tableViewSelectedUser, listViewAccounts);
        QWidget::setTabOrder(listViewAccounts, pushButtonBanAccount);
        QWidget::setTabOrder(pushButtonBanAccount, tableViewSelectedAccount);
        QWidget::setTabOrder(tableViewSelectedAccount, listViewBans);
        QWidget::setTabOrder(listViewBans, pushButtonUnban);
        QWidget::setTabOrder(pushButtonUnban, tableViewSelectedBan);
        QWidget::setTabOrder(tableViewSelectedBan, listViewZones);
        QWidget::setTabOrder(listViewZones, pushButtonDeleteServer);
        QWidget::setTabOrder(pushButtonDeleteServer, tableViewSelectedZone);
        QWidget::setTabOrder(tableViewSelectedZone, textMonitorLog);
        QWidget::setTabOrder(textMonitorLog, pushButtonDisconnect);

        retranslateUi(LoginAdminForm);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(LoginAdminForm);
    } // setupUi

    void retranslateUi(QMainWindow *LoginAdminForm)
    {
        LoginAdminForm->setWindowTitle(QApplication::translate("LoginAdminForm", "Splane - Login Server Administratration Tool", 0, QApplication::UnicodeUTF8));
        groupBoxAddress->setTitle(QApplication::translate("LoginAdminForm", "Server Address", 0, QApplication::UnicodeUTF8));
        lineEditIP->setText(QApplication::translate("LoginAdminForm", "localhost", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("LoginAdminForm", ":", 0, QApplication::UnicodeUTF8));
        lineEditPort->setText(QApplication::translate("LoginAdminForm", "22000", 0, QApplication::UnicodeUTF8));
        groupBoxPubKey->setTitle(QApplication::translate("LoginAdminForm", "Server Public Key", 0, QApplication::UnicodeUTF8));
        lineEditPublicKey->setText(QString());
        pushButtonBrowsePubKey->setText(QApplication::translate("LoginAdminForm", "Load", 0, QApplication::UnicodeUTF8));
        groupBoxPassword->setTitle(QApplication::translate("LoginAdminForm", "Authentication: Password", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("LoginAdminForm", "Account Name:", 0, QApplication::UnicodeUTF8));
        lineEditUserName->setText(QString());
        label_2->setText(QApplication::translate("LoginAdminForm", "Password:", 0, QApplication::UnicodeUTF8));
        lineEditPassword->setText(QString());
        label_4->setText(QApplication::translate("LoginAdminForm", "Alias:", 0, QApplication::UnicodeUTF8));
        lineEditAlias->setText(QString());
        groupBoxKeyFile->setTitle(QApplication::translate("LoginAdminForm", "Authentication: Key File Location (Optional)", 0, QApplication::UnicodeUTF8));
        lineEditKeyFilePath->setText(QApplication::translate("LoginAdminForm", "user_key_file.private", 0, QApplication::UnicodeUTF8));
        pushButtonBrowseKeyFile->setText(QApplication::translate("LoginAdminForm", "...", 0, QApplication::UnicodeUTF8));
        checkBoxUseKeyFile->setText(QApplication::translate("LoginAdminForm", "Use Key File", 0, QApplication::UnicodeUTF8));
        pushButtonDisconnect->setText(QApplication::translate("LoginAdminForm", "Disconnect", 0, QApplication::UnicodeUTF8));
        pushButtonConnect->setText(QApplication::translate("LoginAdminForm", "Connect", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("LoginAdminForm", "Connection Status", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabServer), QApplication::translate("LoginAdminForm", "Server Connection", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabChat), QApplication::translate("LoginAdminForm", "Chat", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("LoginAdminForm", "Selected Active User", 0, QApplication::UnicodeUTF8));
        pushButtonKickActiveUser->setText(QApplication::translate("LoginAdminForm", "Kick", 0, QApplication::UnicodeUTF8));
        pushButtonBanActiveUser->setText(QApplication::translate("LoginAdminForm", "Ban", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabActive), QApplication::translate("LoginAdminForm", "Active Users", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("LoginAdminForm", "Selected Account", 0, QApplication::UnicodeUTF8));
        pushButtonBanAccount->setText(QApplication::translate("LoginAdminForm", "Ban", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabUsers), QApplication::translate("LoginAdminForm", "User Accounts", 0, QApplication::UnicodeUTF8));
        groupBox_8->setTitle(QApplication::translate("LoginAdminForm", "Selected Ban", 0, QApplication::UnicodeUTF8));
        pushButtonUnban->setText(QApplication::translate("LoginAdminForm", "Unban", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabBans), QApplication::translate("LoginAdminForm", "Bans", 0, QApplication::UnicodeUTF8));
        groupBox_9->setTitle(QApplication::translate("LoginAdminForm", "Selected Zone Server", 0, QApplication::UnicodeUTF8));
        pushButtonDeleteServer->setText(QApplication::translate("LoginAdminForm", "Delete", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabZones), QApplication::translate("LoginAdminForm", "Zone Servers", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabLog), QApplication::translate("LoginAdminForm", "Monitor Log", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LoginAdminForm: public Ui_LoginAdminForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAINPD4224_H
