#ifndef DATAANALYZER_H
#define DATAANALYZER_H

#include <QObject>
#include <QVector>
#include <QPair>
#include <QDateTime>
#include <QMap>

#include "measurement.h"

// Rozszerzona struktura wyników analizy
struct AnalysisResult {
    double minValue;
    QDateTime minDate;
    double maxValue;
    QDateTime maxDate;
    double avgValue;
    double trend;         // Współczynnik trendu (dodatni oznacza wzrost, ujemny spadek)
    QString trendDescription;
    int totalMeasurements;
    // Inicjalizacja domyślnych wartości
    AnalysisResult() :
         maxValue(0.0), avgValue(0.0),
         trend(0.0), totalMeasurements(0) {}
};

// Struktura dla prognoz
struct Forecast {
    QDateTime date;
    double predictedValue;
    double confidenceLow;  // Dolna granica przedziału ufności
    double confidenceHigh; // Górna granica przedziału ufności
};

class DataAnalyzer : public QObject
{
    Q_OBJECT

public:
    explicit DataAnalyzer(QObject *parent = nullptr);

    // Podstawowa analiza danych
    AnalysisResult analyzeMeasurements(const QVector<Measurement>& measurements);

    // Filtrowanie danych
    QVector<Measurement> filterMeasurementsByDateRange(const QVector<Measurement>& measurements,
                                                       const QDateTime& startDate,
                                                       const QDateTime& endDate);

    // Analiza trendu
    double calculateTrend(const QVector<Measurement>& measurements);
    QString getTrendDescription(double trend);


};

#endif // DATAANALYZER_H
