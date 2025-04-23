#include "dataanalyzer.h"
#include <algorithm>
#include <numeric>

DataAnalyzer::DataAnalyzer(QObject* parent) : QObject(parent)
{
}

AnalysisResult DataAnalyzer::analyzeMeasurements(const QVector<Measurement>& measurements)
{
    AnalysisResult result;

    if (measurements.isEmpty()) {
        // Wartości domyślne dla pustych pomiarów
        result.minValue = 0;
        result.maxValue = 0;
        result.avgValue = 0;
        result.trend = 0;
        result.trendDescription = "Brak danych do analizy";
        return result;
    }

    // Szukanie minimum i maksimum
    auto minMaxPair = std::minmax_element(
        measurements.begin(), measurements.end(),
        [](const Measurement& a, const Measurement& b) { return a.value < b.value; }
        );

    result.minValue = minMaxPair.first->value;
    result.minDate = minMaxPair.first->date;

    result.maxValue = minMaxPair.second->value;
    result.maxDate = minMaxPair.second->date;

    // Obliczanie średniej
    double sum = std::accumulate(
        measurements.begin(), measurements.end(), 0.0,
        [](double sum, const Measurement& m) { return sum + m.value; }
        );

    result.avgValue = sum / measurements.size();

    // Obliczanie trendu
    result.trend = calculateTrend(measurements);
    result.trendDescription = getTrendDescription(result.trend);

    return result;
}

QVector<Measurement> DataAnalyzer::filterMeasurementsByDateRange(const QVector<Measurement>& measurements,
                                                                 const QDateTime& startDate,
                                                                 const QDateTime& endDate)
{
    QVector<Measurement> filteredMeasurements;

    std::copy_if(
        measurements.begin(), measurements.end(),
        std::back_inserter(filteredMeasurements),
        [&startDate, &endDate](const Measurement& m) {
            return m.date >= startDate && m.date <= endDate;
        }
        );

    return filteredMeasurements;
}

double DataAnalyzer::calculateTrend(const QVector<Measurement>& measurements)
{
    if (measurements.size() < 2) {
        return 0.0;
    }

    // Sortujemy pomiary według czasu
    QVector<Measurement> sortedMeasurements = measurements;
    std::sort(sortedMeasurements.begin(), sortedMeasurements.end(),
              [](const Measurement& a, const Measurement& b) { return a.date < b.date; });

    // Implementacja prostej regresji liniowej
    int n = sortedMeasurements.size();

    // Zamieniamy daty na sekundy
    QVector<double> x; // czasTrwania w sekundach
    QVector<double> y; // wartości pomiarów

    qint64 firstTimestamp = sortedMeasurements.first().date.toSecsSinceEpoch();

    for (const Measurement& m : sortedMeasurements) {
        // Normalizacja czasu - sekundy od pierwszego pomiaru
        double timeValue = m.date.toSecsSinceEpoch() - firstTimestamp;
        x.append(timeValue);
        y.append(m.value);
    }

    // Obliczanie średnich
    double sumX = std::accumulate(x.begin(), x.end(), 0.0);
    double sumY = std::accumulate(y.begin(), y.end(), 0.0);
    double meanX = sumX / n;
    double meanY = sumY / n;

    // Obliczanie współczynnika nachylenia (slope)
    double numerator = 0.0;
    double denominator = 0.0;

    for (int i = 0; i < n; i++) {
        numerator += (x[i] - meanX) * (y[i] - meanY);
        denominator += (x[i] - meanX) * (x[i] - meanX);
    }

    if (denominator == 0.0) {
        return 0.0;
    }

    // Współczynnik nachylenia (slope) reprezentuje trend
    double slope = numerator / denominator;

    // Normalizacja trendu - chcemy wartość na godzinę, więc mnożymy przez 3600 (liczba sekund w godzinie)
    return slope * 3600.0;
}

QString DataAnalyzer::getTrendDescription(double trend)
{
    const double THRESHOLD = 0.1; // Próg dla uznania trendu za istotny

    if (std::abs(trend) < THRESHOLD) {
        return "Brak wyraźnego trendu (stabilny)";
    }
    else if (trend > 0) {
        if (trend > 5.0) {
            return "Szybki wzrost";
        }
        else if (trend > 1.0) {
            return "Umiarkowany wzrost";
        }
        else {
            return "Niewielki wzrost";
        }
    }
    else {
        if (trend < -5.0) {
            return "Szybki spadek";
        }
        else if (trend < -1.0) {
            return "Umiarkowany spadek";
        }
        else {
            return "Niewielki spadek";
        }
    }
}
