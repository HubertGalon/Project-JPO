/********************************************************************************
** Form generated from reading UI file 'chartwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHARTWIDGET_H
#define UI_CHARTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChartWidget
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QLabel *parameterLabel;
    QHBoxLayout *filterLayout;
    QLabel *timeRangeLabel;
    QComboBox *timeRangeComboBox;
    QLabel *startDateLabel;
    QDateEdit *startDateEdit;
    QLabel *endDateLabel;
    QDateEdit *endDateEdit;
    QVBoxLayout *chartLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *analyzeButton;
    QPushButton *saveChartButton;
    QPushButton *exportDataButton;
    QPushButton *backButton;

    void setupUi(QWidget *ChartWidget)
    {
        if (ChartWidget->objectName().isEmpty())
            ChartWidget->setObjectName("ChartWidget");
        ChartWidget->resize(800, 600);
        verticalLayout = new QVBoxLayout(ChartWidget);
        verticalLayout->setObjectName("verticalLayout");
        titleLabel = new QLabel(ChartWidget);
        titleLabel->setObjectName("titleLabel");
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        parameterLabel = new QLabel(ChartWidget);
        parameterLabel->setObjectName("parameterLabel");
        QFont font1;
        font1.setPointSize(12);
        parameterLabel->setFont(font1);
        parameterLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(parameterLabel);

        filterLayout = new QHBoxLayout();
        filterLayout->setObjectName("filterLayout");
        timeRangeLabel = new QLabel(ChartWidget);
        timeRangeLabel->setObjectName("timeRangeLabel");

        filterLayout->addWidget(timeRangeLabel);

        timeRangeComboBox = new QComboBox(ChartWidget);
        timeRangeComboBox->setObjectName("timeRangeComboBox");

        filterLayout->addWidget(timeRangeComboBox);

        startDateLabel = new QLabel(ChartWidget);
        startDateLabel->setObjectName("startDateLabel");

        filterLayout->addWidget(startDateLabel);

        startDateEdit = new QDateEdit(ChartWidget);
        startDateEdit->setObjectName("startDateEdit");
        startDateEdit->setCalendarPopup(true);

        filterLayout->addWidget(startDateEdit);

        endDateLabel = new QLabel(ChartWidget);
        endDateLabel->setObjectName("endDateLabel");

        filterLayout->addWidget(endDateLabel);

        endDateEdit = new QDateEdit(ChartWidget);
        endDateEdit->setObjectName("endDateEdit");
        endDateEdit->setCalendarPopup(true);

        filterLayout->addWidget(endDateEdit);


        verticalLayout->addLayout(filterLayout);

        chartLayout = new QVBoxLayout();
        chartLayout->setObjectName("chartLayout");

        verticalLayout->addLayout(chartLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        analyzeButton = new QPushButton(ChartWidget);
        analyzeButton->setObjectName("analyzeButton");

        horizontalLayout->addWidget(analyzeButton);

        saveChartButton = new QPushButton(ChartWidget);
        saveChartButton->setObjectName("saveChartButton");

        horizontalLayout->addWidget(saveChartButton);

        exportDataButton = new QPushButton(ChartWidget);
        exportDataButton->setObjectName("exportDataButton");

        horizontalLayout->addWidget(exportDataButton);

        backButton = new QPushButton(ChartWidget);
        backButton->setObjectName("backButton");

        horizontalLayout->addWidget(backButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ChartWidget);

        QMetaObject::connectSlotsByName(ChartWidget);
    } // setupUi

    void retranslateUi(QWidget *ChartWidget)
    {
        ChartWidget->setWindowTitle(QCoreApplication::translate("ChartWidget", "Wykres pomiarow", nullptr));
        titleLabel->setText(QCoreApplication::translate("ChartWidget", "Wykres danych pomiarowych", nullptr));
        parameterLabel->setText(QCoreApplication::translate("ChartWidget", "Parametr: ", nullptr));
        timeRangeLabel->setText(QCoreApplication::translate("ChartWidget", "Zakres czasu:", nullptr));
        startDateLabel->setText(QCoreApplication::translate("ChartWidget", "Data poczatkowa:", nullptr));
        endDateLabel->setText(QCoreApplication::translate("ChartWidget", "Data koncowa:", nullptr));
        analyzeButton->setText(QCoreApplication::translate("ChartWidget", "Analizuj dane", nullptr));
        saveChartButton->setText(QCoreApplication::translate("ChartWidget", "Zapisz wykres", nullptr));
        exportDataButton->setText(QCoreApplication::translate("ChartWidget", "Eksportuj dane", nullptr));
        backButton->setText(QCoreApplication::translate("ChartWidget", "Powrot do listy czujnikow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChartWidget: public Ui_ChartWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHARTWIDGET_H
