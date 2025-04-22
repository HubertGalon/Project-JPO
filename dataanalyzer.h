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
    double medianValue;
    double stdDeviation;  // Odchylenie standardowe
    double trend;         // Współczynnik trendu (dodatni oznacza wzrost, ujemny spadek)
    QString trendDescription;
    int totalMeasurements;
    QMap<QString, double> dailyAverages;  // Średnie dzienne
    QMap<QString, double> hourlyAverages; // Średnie godzinowe

    // Inicjalizacja domyślnych wartości
    AnalysisResult() :
        minValue(0.0), maxValue(0.0), avgValue(0.0), medianValue(0.0),
        stdDeviation(0.0), trend(0.0), totalMeasurements(0) {}
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

    // Wykrywanie anomalii
    QVector<Measurement> findAnomalies(const QVector<Measurement>& measurements, double zThreshold = 2.0);

    // Prognozowanie (proste)
    QVector<Forecast> forecastValues(const QVector<Measurement>& measurements, int daysAhead = 3);

    // Statystyki dzienne i godzinowe
    QMap<QString, double> calculateDailyAverages(const QVector<Measurement>& measurements);
    QMap<QString, double> calculateHourlyAverages(const QVector<Measurement>& measurements);

    // Korelacja z innym zestawem danych (np. korelacja pomiędzy różnymi parametrami)
    double calculateCorrelation(const QVector<Measurement>& measurements1,
                                const QVector<Measurement>& measurements2);

private:
    // Metody pomocnicze
    double calculateStandardDeviation(const QVector<double>& values, double mean);
    double calculateMedian(QVector<double> values);
    QPair<QVector<double>, QVector<double>> alignMeasurementSeries(
        const QVector<Measurement>& measurements1,
        const QVector<Measurement>& measurements2);
};

#endif // DATAANALYZER_H
