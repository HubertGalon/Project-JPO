/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOdswiezDane;
    QAction *actionZamknij;
    QAction *actionOProgramie;
    QWidget *centralwidget;
    QMenuBar *menubar;
    QMenu *menuPlik;
    QMenu *menuPomoc;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        actionOdswiezDane = new QAction(MainWindow);
        actionOdswiezDane->setObjectName("actionOdswiezDane");
        actionZamknij = new QAction(MainWindow);
        actionZamknij->setObjectName("actionZamknij");
        actionOProgramie = new QAction(MainWindow);
        actionOProgramie->setObjectName("actionOProgramie");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        menuPlik = new QMenu(menubar);
        menuPlik->setObjectName("menuPlik");
        menuPomoc = new QMenu(menubar);
        menuPomoc->setObjectName("menuPomoc");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuPlik->menuAction());
        menubar->addAction(menuPomoc->menuAction());
        menuPlik->addAction(actionOdswiezDane);
        menuPlik->addSeparator();
        menuPlik->addAction(actionZamknij);
        menuPomoc->addAction(actionOProgramie);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Monitor Jako\305\233ci Powietrza", nullptr));
        actionOdswiezDane->setText(QCoreApplication::translate("MainWindow", "Odswiez dane", nullptr));
#if QT_CONFIG(shortcut)
        actionOdswiezDane->setShortcut(QCoreApplication::translate("MainWindow", "F5", nullptr));
#endif // QT_CONFIG(shortcut)
        actionZamknij->setText(QCoreApplication::translate("MainWindow", "Zamknij", nullptr));
#if QT_CONFIG(shortcut)
        actionZamknij->setShortcut(QCoreApplication::translate("MainWindow", "Alt+F4", nullptr));
#endif // QT_CONFIG(shortcut)
        actionOProgramie->setText(QCoreApplication::translate("MainWindow", "O programie", nullptr));
        menuPlik->setTitle(QCoreApplication::translate("MainWindow", "Plik", nullptr));
        menuPomoc->setTitle(QCoreApplication::translate("MainWindow", "Pomoc", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
