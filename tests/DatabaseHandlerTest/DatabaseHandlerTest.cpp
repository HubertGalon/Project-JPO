#include <QtTest>
#include "../../databasehandler.h"
#include <QTemporaryDir>
#include <QStandardPaths>

class MockDatabaseHandler : public DatabaseHandler
{
public:
    MockDatabaseHandler() : DatabaseHandler(nullptr) {}
    
    // Metoda dostępu do ścieżek plików do testów
    QString getTestStationsFilePath() const { return getStationsFilePath(); }
    QString getTestSensorsFilePath(int stationId) const { return getSensorsFilePath(stationId); }
    QString getTestMeasurementsFilePath(int sensorId) const { return getMeasurementsFilePath(sensorId); }
};

class DatabaseHandlerTest : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir tempDir;
    QString originalDataLocation;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testSaveAndLoadStations();
    void testSaveAndLoadSensors();
    void testSaveAndLoadMeasurements();
};

void DatabaseHandlerTest::initTestCase()
{
    // Zapisujemy oryginalną lokalizację danych
    originalDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    
    // Tworzymy tymczasowy katalog na dane testowe
    QVERIFY(tempDir.isValid());
    
    // Przekierowujemy AppLocalDataLocation na tymczasowy katalog na czas testów
    QStandardPaths::setTestModeEnabled(true);
    QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)).mkpath(".");
}

void DatabaseHandlerTest::cleanupTestCase()
{
    // Przywracamy oryginalne ustawienia
    QStandardPaths::setTestModeEnabled(false);
}

void DatabaseHandlerTest::testSaveAndLoadStations()
{
    // Tworzenie testowych danych
    QVector<Station> testStations;
    
    Station station1;
    station1.id = 1;
    station1.stationName = "Testowa Stacja 1";
    station1.gegrLat = 52.123;
    station1.gegrLon = 21.456;
    station1.cityName = "Miasto Testowe";
    
    Station station2;
    station2.id = 2;
    station2.stationName = "Testowa Stacja 2";
    station2.gegrLat = 53.789;
    station2.gegrLon = 22.123;
    station2.cityName = "Inne Miasto";
    
    testStations.append(station1);
    testStations.append(station2);
    
    // Inicjalizacja handlerów bazy danych
    DatabaseHandler dbHandler;
    
    // Zapis danych
    dbHandler.saveStations(testStations);
    
    // Odczyt danych
    QVector<Station> loadedStations = dbHandler.loadStations();
    
    // Weryfikacja wyników
    QCOMPARE(loadedStations.size(), 2);
    QCOMPARE(loadedStations[0].id, 1);
    QCOMPARE(loadedStations[0].stationName, QString("Testowa Stacja 1"));
    QCOMPARE(loadedStations[0].gegrLat, 52.123);
    QCOMPARE(loadedStations[0].cityName, QString("Miasto Testowe"));
    
    QCOMPARE(loadedStations[1].id, 2);
    QCOMPARE(loadedStations[1].stationName, QString("Testowa Stacja 2"));
}

void DatabaseHandlerTest::testSaveAndLoadSensors()
{
    // Tworzenie testowych danych
    QVector<Sensor> testSensors;
    
    Sensor sensor1;
    sensor1.id = 101;
    sensor1.stationId = 1;
    sensor1.paramName = "PM10";
    sensor1.paramFormula = "PM10";
    sensor1.paramCode = "PM10";
    
    Sensor sensor2;
    sensor2.id = 102;
    sensor2.stationId = 1;
    sensor2.paramName = "PM2.5";
    sensor2.paramFormula = "PM2.5";
    sensor2.paramCode = "PM25";
    
    testSensors.append(sensor1);
    testSensors.append(sensor2);
    
    // Inicjalizacja handlera bazy danych
    DatabaseHandler dbHandler;
    
    // Zapis danych
    dbHandler.saveSensors(1, testSensors);
    
    // Odczyt danych
    QVector<Sensor> loadedSensors = dbHandler.loadSensors(1);
    
    // Weryfikacja wyników
    QCOMPARE(loadedSensors.size(), 2);
    QCOMPARE(loadedSensors[0].id, 101);
    QCOMPARE(loadedSensors[0].stationId, 1);
    QCOMPARE(loadedSensors[0].paramName, QString("PM10"));
    
    QCOMPARE(loadedSensors[1].id, 102);
    QCOMPARE(loadedSensors[1].paramName, QString("PM2.5"));
}

void DatabaseHandlerTest::testSaveAndLoadMeasurements()
{
    // Tworzenie testowych danych
    QVector<Measurement> testMeasurements;
    
    QDateTime now = QDateTime::currentDateTime();
    testMeasurements.append(Measurement("PM10", now, 25.5));
    testMeasurements.append(Measurement("PM10", now.addSecs(-3600), 30.2)); // Godzinę wcześniej
    testMeasurements.append(Measurement("PM10", now.addSecs(-7200), 28.7)); // Dwie godziny wcześniej
    
    // Inicjalizacja handlera bazy danych
    DatabaseHandler dbHandler;
    
    // Zapis danych
    dbHandler.saveMeasurements(101, testMeasurements);
    
    // Odczyt danych
    QVector<Measurement> loadedMeasurements = dbHandler.loadMeasurements(101);
    
    // Weryfikacja wyników
    QCOMPARE(loadedMeasurements.size(), 3);
    
    // Sprawdzamy czy wszystkie wartości zostały prawidłowo zapisane i odczytane
    QSet<double> expectedValues = {25.5, 30.2, 28.7};
    QSet<double> actualValues;
    
    for (const Measurement& m : loadedMeasurements) {
        QCOMPARE(m.parameterCode, QString("PM10"));
        QVERIFY(m.isValid);
        actualValues.insert(m.value);
    }
    
    QCOMPARE(actualValues, expectedValues);
}

QTEST_APPLESS_MAIN(DatabaseHandlerTest)
#include "DatabaseHandlerTest.moc"
