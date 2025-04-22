#include <QtTest>
#include "../../dataanalyzer.h"
#include "../../measurement.h"

class DataAnalyzerTest : public QObject
{
    Q_OBJECT

private slots:
    void testAnalyzeMeasurements();
    void testFilterMeasurementsByDateRange();
    void testCalculateTrend();
};

void DataAnalyzerTest::testAnalyzeMeasurements()
{
    // Przygotowanie danych testowych
    QVector<Measurement> measurements;
    
    // Dodajemy kilka pomiarów testowych
    QDateTime baseTime = QDateTime::currentDateTime();
    measurements.append(Measurement("PM10", baseTime.addSecs(-3600), 10.0)); // 1h temu
    measurements.append(Measurement("PM10", baseTime.addSecs(-7200), 20.0)); // 2h temu
    measurements.append(Measurement("PM10", baseTime.addSecs(-10800), 15.0)); // 3h temu
    
    // Inicjalizacja analizatora danych
    DataAnalyzer analyzer;
    
    // Wywołanie testowanej metody
    AnalysisResult result = analyzer.analyzeMeasurements(measurements);
    
    // Weryfikacja wyników
    QCOMPARE(result.minValue, 10.0);
    QCOMPARE(result.maxValue, 20.0);
    QCOMPARE(result.avgValue, 15.0);
    QVERIFY(result.trend != 0.0); // Sprawdzamy, czy trend został obliczony (dokładna wartość może się różnić)
    QVERIFY(!result.trendDescription.isEmpty()); // Opis trendu nie powinien być pusty
}

void DataAnalyzerTest::testFilterMeasurementsByDateRange()
{
    // Przygotowanie danych testowych
    QVector<Measurement> measurements;
    
    QDateTime now = QDateTime::currentDateTime();
    QDateTime yesterday = now.addDays(-1);
    QDateTime twoDaysAgo = now.addDays(-2);
    QDateTime threeDaysAgo = now.addDays(-3);
    
    measurements.append(Measurement("PM10", now, 10.0));
    measurements.append(Measurement("PM10", yesterday, 20.0));
    measurements.append(Measurement("PM10", twoDaysAgo, 15.0));
    measurements.append(Measurement("PM10", threeDaysAgo, 25.0));
    
    // Inicjalizacja analizatora danych
    DataAnalyzer analyzer;
    
    // Filtrujemy dane z ostatnich dwóch dni
    QVector<Measurement> filtered = analyzer.filterMeasurementsByDateRange(
        measurements, yesterday.addSecs(-3600), now); // Od wczoraj (z 1h zapasem) do teraz
    
    // Weryfikacja wyników
    QCOMPARE(filtered.size(), 2); // Powinny zostać tylko dwa najnowsze pomiary
    QCOMPARE(filtered[0].date, now);
    QCOMPARE(filtered[1].date, yesterday);
}

void DataAnalyzerTest::testCalculateTrend()
{
    // Przygotowanie danych testowych dla trendu rosnącego
    QVector<Measurement> risingMeasurements;
    QDateTime baseTime = QDateTime::currentDateTime();
    
    // Dodajemy sekwencję rosnących wartości
    for (int i = 0; i < 10; i++) {
        risingMeasurements.append(Measurement("PM10", baseTime.addSecs(i * 3600), 10.0 + i * 2.0));
    }
    
    // Przygotowanie danych testowych dla trendu malejącego
    QVector<Measurement> fallingMeasurements;
    
    // Dodajemy sekwencję malejących wartości
    for (int i = 0; i < 10; i++) {
        fallingMeasurements.append(Measurement("PM10", baseTime.addSecs(i * 3600), 30.0 - i * 2.0));
    }
    
    // Inicjalizacja analizatora danych
    DataAnalyzer analyzer;
    
    // Obliczanie trendów
    double risingTrend = analyzer.calculateTrend(risingMeasurements);
    double fallingTrend = analyzer.calculateTrend(fallingMeasurements);
    
    // Weryfikacja wyników
    QVERIFY(risingTrend > 0.0); // Trend powinien być dodatni (rosnący)
    QVERIFY(fallingTrend < 0.0); // Trend powinien być ujemny (malejący)
    
    // Sprawdzenie, czy opisy trendów są odpowiednie
    QString risingDescription = analyzer.getTrendDescription(risingTrend);
    QString fallingDescription = analyzer.getTrendDescription(fallingTrend);
    
    QVERIFY(risingDescription.contains("wzrost", Qt::CaseInsensitive));
    QVERIFY(fallingDescription.contains("spadek", Qt::CaseInsensitive));
}

QTEST_APPLESS_MAIN(DataAnalyzerTest)
#include "DataAnalyzerTest.moc"
