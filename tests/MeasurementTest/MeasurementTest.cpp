#include <QtTest>
#include "../../measurement.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class MeasurementTest : public QObject
{
    Q_OBJECT

private slots:
    void testFromJsonObject();
    void testEmptyJson();
    void testNullValues();
};

void MeasurementTest::testFromJsonObject()
{
    // Przygotowanie danych testowych
    QString jsonStr = R"({
        "key": "PM10",
        "values": [
            {
                "date": "2024-04-21 10:00:00",
                "value": 23.5
            },
            {
                "date": "2024-04-21 11:00:00",
                "value": 25.8
            }
        ]
    })";
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject jsonObj = doc.object();
    
    // Wykonanie metody do przetestowania
    QVector<Measurement> measurements = Measurement::fromJsonObject(jsonObj);
    
    // Weryfikacja wyników
    QCOMPARE(measurements.size(), 2);
    QCOMPARE(measurements[0].parameterCode, QString("PM10"));
    QCOMPARE(measurements[0].date, QDateTime::fromString("2024-04-21 10:00:00", Qt::ISODate));
    QCOMPARE(measurements[0].value, 23.5);
    QCOMPARE(measurements[0].isValid, true);
    
    QCOMPARE(measurements[1].parameterCode, QString("PM10"));
    QCOMPARE(measurements[1].date, QDateTime::fromString("2024-04-21 11:00:00", Qt::ISODate));
    QCOMPARE(measurements[1].value, 25.8);
    QCOMPARE(measurements[1].isValid, true);
}

void MeasurementTest::testEmptyJson()
{
    // Test z pustym obiektem JSON
    QJsonObject emptyObj;
    QVector<Measurement> measurements = Measurement::fromJsonObject(emptyObj);
    
    // Powinien zwrócić pustą listę
    QVERIFY(measurements.isEmpty());
}

void MeasurementTest::testNullValues()
{
    // Przygotowanie danych testowych z wartością NULL
    QString jsonStr = R"({
        "key": "PM10",
        "values": [
            {
                "date": "2024-04-21 10:00:00",
                "value": null
            },
            {
                "date": "2024-04-21 11:00:00",
                "value": 25.8
            }
        ]
    })";
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject jsonObj = doc.object();
    
    // Wykonanie metody do przetestowania
    QVector<Measurement> measurements = Measurement::fromJsonObject(jsonObj);
    
    // Weryfikacja wyników - powinien pominąć null i zwrócić tylko jeden pomiar
    QCOMPARE(measurements.size(), 1);
    QCOMPARE(measurements[0].parameterCode, QString("PM10"));
    QCOMPARE(measurements[0].date, QDateTime::fromString("2024-04-21 11:00:00", Qt::ISODate));
    QCOMPARE(measurements[0].value, 25.8);
}

QTEST_APPLESS_MAIN(MeasurementTest)
#include "MeasurementTest.moc"
