#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QVector>
#include <QDateEdit>

#include "measurement.h"
#include "dataanalyzer.h"

    namespace Ui {
    class ChartWidget;
}

class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = nullptr);
    ~ChartWidget();

    void setMeasurements(const QVector<Measurement>& measurements);

signals:
    void backRequested();

private slots:
    void onBackButtonClicked();
    void onTimeRangeChanged(int index);
    void onDateRangeChanged();
    void onAnalyzeButtonClicked();
    void onSaveChartButtonClicked();
    void onExportDataButtonClicked();

private:
    Ui::ChartWidget *ui;
    QVector<Measurement> measurements;
    QChart *chart;
    QChartView *chartView;
    DataAnalyzer *dataAnalyzer;

    // Metody do obsługi wykresu
    void createChart();
    void updateChart(const QVector<Measurement>& measurementsToShow);

    // Metody do filtrowania danych
    QVector<Measurement> filterMeasurementsByTimeRange(int timeRangeIndex);

    // Metody do analizy danych
    void displayAnalysisResults(const AnalysisResult& result);
};

#endif // CHARTWIDGET_H
