/********************************************************************************
** Form generated from reading UI file 'stationlistwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STATIONLISTWIDGET_H
#define UI_STATIONLISTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StationListWidget
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QHBoxLayout *horizontalLayout;
    QComboBox *filterComboBox;
    QLineEdit *searchLineEdit;
    QPushButton *searchButton;
    QPushButton *geolocateButton;
    QListWidget *stationListWidget;
    QLabel *totalStationsLabel;

    void setupUi(QWidget *StationListWidget)
    {
        if (StationListWidget->objectName().isEmpty())
            StationListWidget->setObjectName("StationListWidget");
        StationListWidget->resize(600, 400);
        verticalLayout = new QVBoxLayout(StationListWidget);
        verticalLayout->setObjectName("verticalLayout");
        titleLabel = new QLabel(StationListWidget);
        titleLabel->setObjectName("titleLabel");
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        filterComboBox = new QComboBox(StationListWidget);
        filterComboBox->setObjectName("filterComboBox");

        horizontalLayout->addWidget(filterComboBox);

        searchLineEdit = new QLineEdit(StationListWidget);
        searchLineEdit->setObjectName("searchLineEdit");

        horizontalLayout->addWidget(searchLineEdit);

        searchButton = new QPushButton(StationListWidget);
        searchButton->setObjectName("searchButton");

        horizontalLayout->addWidget(searchButton);

        geolocateButton = new QPushButton(StationListWidget);
        geolocateButton->setObjectName("geolocateButton");

        horizontalLayout->addWidget(geolocateButton);


        verticalLayout->addLayout(horizontalLayout);

        stationListWidget = new QListWidget(StationListWidget);
        stationListWidget->setObjectName("stationListWidget");

        verticalLayout->addWidget(stationListWidget);

        totalStationsLabel = new QLabel(StationListWidget);
        totalStationsLabel->setObjectName("totalStationsLabel");

        verticalLayout->addWidget(totalStationsLabel);


        retranslateUi(StationListWidget);

        QMetaObject::connectSlotsByName(StationListWidget);
    } // setupUi

    void retranslateUi(QWidget *StationListWidget)
    {
        StationListWidget->setWindowTitle(QCoreApplication::translate("StationListWidget", "Stacje pomiarowe", nullptr));
        titleLabel->setText(QCoreApplication::translate("StationListWidget", "Lista stacji pomiarowych", nullptr));
        searchLineEdit->setPlaceholderText(QCoreApplication::translate("StationListWidget", "Wprowad\305\272 kryteria wyszukiwania...", nullptr));
        searchButton->setText(QCoreApplication::translate("StationListWidget", "Szukaj", nullptr));
        geolocateButton->setText(QCoreApplication::translate("StationListWidget", "Geolokalizacja", nullptr));
        totalStationsLabel->setText(QCoreApplication::translate("StationListWidget", "\305\201\304\205czna liczba stacji: 0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class StationListWidget: public Ui_StationListWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STATIONLISTWIDGET_H
