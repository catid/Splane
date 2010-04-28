/********************************************************************************
** Form generated from reading UI file 'imageWh5508.ui'
**
** Created: Sat Apr 17 08:25:07 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef IMAGEWH5508_H
#define IMAGEWH5508_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGraphicsView>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Image
{
public:
    QVBoxLayout *verticalLayout;
    QGraphicsView *graphicsView;
    QLabel *label;

    void setupUi(QDialog *Image)
    {
        if (Image->objectName().isEmpty())
            Image->setObjectName(QString::fromUtf8("Image"));
        Image->resize(738, 688);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/kennel.png"), QSize(), QIcon::Normal, QIcon::Off);
        Image->setWindowIcon(icon);
        Image->setSizeGripEnabled(true);
        verticalLayout = new QVBoxLayout(Image);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        graphicsView = new QGraphicsView(Image);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));

        verticalLayout->addWidget(graphicsView);

        label = new QLabel(Image);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);


        retranslateUi(Image);

        QMetaObject::connectSlotsByName(Image);
    } // setupUi

    void retranslateUi(QDialog *Image)
    {
        Image->setWindowTitle(QApplication::translate("Image", "Kennel - Texture Preview", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        Image->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
        label->setText(QApplication::translate("Image", "Loading...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Image: public Ui_Image {};
} // namespace Ui

QT_END_NAMESPACE

#endif // IMAGEWH5508_H
