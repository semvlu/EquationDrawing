/********************************************************************************
** Form generated from reading UI file 'viewer.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWER_H
#define UI_VIEWER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_Viewer
{
public:
    QWidget *centralwidget;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QPushButton *createFunction;
    QCustomPlot *qcp;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Viewer)
    {
        if (Viewer->objectName().isEmpty())
            Viewer->setObjectName(QString::fromUtf8("Viewer"));
        Viewer->resize(1070, 801);
        centralwidget = new QWidget(Viewer);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayoutWidget = new QWidget(centralwidget);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(790, 80, 271, 671));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        createFunction = new QPushButton(centralwidget);
        createFunction->setObjectName(QString::fromUtf8("createFunction"));
        createFunction->setGeometry(QRect(790, 10, 271, 61));
        qcp = new QCustomPlot(centralwidget);
        qcp->setObjectName(QString::fromUtf8("qcp"));
        qcp->setGeometry(QRect(0, 10, 781, 741));
        Viewer->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Viewer);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1070, 25));
        Viewer->setMenuBar(menubar);
        statusbar = new QStatusBar(Viewer);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        Viewer->setStatusBar(statusbar);

        retranslateUi(Viewer);

        QMetaObject::connectSlotsByName(Viewer);
    } // setupUi

    void retranslateUi(QMainWindow *Viewer)
    {
        Viewer->setWindowTitle(QCoreApplication::translate("Viewer", "Viewer", nullptr));
        createFunction->setText(QCoreApplication::translate("Viewer", "CREATE", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Viewer: public Ui_Viewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWER_H
