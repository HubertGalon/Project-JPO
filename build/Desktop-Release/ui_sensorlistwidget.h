/********************************************************************************
** Form generated from reading UI file 'sensorlistwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SENSORLISTWIDGET_H
#define UI_SENSORLISTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SensorListWidget
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QLabel *stationNameLabel;
    QListWidget *sensorListWidget;
    QLabel *sensorCountLabel;
    QPushButton *backButton;

    void setupUi(QWidget *SensorListWidget)
    {
        if (SensorListWidget->objectName().isEmpty())
            SensorListWidget->setObjectName("SensorListWidget");
        SensorListWidget->resize(600, 400);
        verticalLayout = new QVBoxLayout(SensorListWidget);
        verticalLayout->setObjectName("verticalLayout");
        titleLabel = new QLabel(SensorListWidget);
        titleLabel->setObjectName("titleLabel");
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        stationNameLabel = new QLabel(SensorListWidget);
        stationNameLabel->setObjectName("stationNameLabel");
        QFont font1;
        font1.setPointSize(12);
        stationNameLabel->setFont(font1);
        stationNameLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(stationNameLabel);

        sensorListWidget = new QListWidget(SensorListWidget);
        sensorListWidget->setObjectName("sensorListWidget");

        verticalLayout->addWidget(sensorListWidget);

        sensorCountLabel = new QLabel(SensorListWidget);
        sensorCountLabel->setObjectName("sensorCountLabel");

        verticalLayout->addWidget(sensorCountLabel);

        backButton = new QPushButton(SensorListWidget);
        backButton->setObjectName("backButton");

        verticalLayout->addWidget(backButton);


        retranslateUi(SensorListWidget);

        QMetaObject::connectSlotsByName(SensorListWidget);
    } // setupUi

    void retranslateUi(QWidget *SensorListWidget)
    {
        SensorListWidget->setWindowTitle(QCoreApplication::translate("SensorListWidget", "Czujniki pomiarowe", nullptr));
        titleLabel->setText(QCoreApplication::translate("SensorListWidget", "Dostepne czujniki na stacji", nullptr));
        stationNameLabel->setText(QCoreApplication::translate("SensorListWidget", "Nazwa stacji", nullptr));
        sensorCountLabel->setText(QCoreApplication::translate("SensorListWidget", "Dostepne czujniki: 0", nullptr));
        backButton->setText(QCoreApplication::translate("SensorListWidget", "Powrot do listy stacji", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SensorListWidget: public Ui_SensorListWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SENSORLISTWIDGET_H
