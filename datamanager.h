#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QVector>
#include <QJsonArray>
#include <QJsonObject>
#include <QMutex>
#include <QThreadPool>
#include <QFuture>
#include <QFutureWatcher>

#include "apiclient.h"
#include "databasehandler.h"
#include "station.h"
#include "sensor.h"
#include "measurement.h"
#include "airqualityindex.h"
#include "dataanalyzer.h"
#include "networkchecker.h"

class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject *parent = nullptr);
    ~DataManager();

    // Metody pobierania danych
    void getAllStations();
    void getStationsForCity(const QString& cityName);
    void getStationsNearLocation(double latitude, double longitude, double radiusInKm);
    void getSensors(int stationId);
    void getMeasurements(int sensorId);
    void getAirQualityIndex(int stationId);

    // Metody wyszukiwania
    QVector<Station> findStationsByCity(const QString& cityName) const;
    QVector<Station> findStationsNearLocation(double latitude, double longitude, double radiusInKm) const;

    // Metody zapisu i odczytu
    void saveMeasurementsToDatabase(int sensorId, const QVector<Measurement>& measurements);
    QVector<Measurement> loadMeasurementsFromDatabase(int sensorId);

    // Metody dla obsługi trybu offline
    bool isNetworkAvailable() const;
    bool isOfflineMode() const;
    void setOfflineMode(bool offline);

    // Metoda sprawdzająca czy są dane zapisane dla danego sensora
    bool hasCachedData(int sensorId) const;

signals:
    void stationsLoaded(const QVector<Station>& stations);
    void sensorsLoaded(const QVector<Sensor>& sensors);
    void measurementsLoaded(const QVector<Measurement>& measurements);
    void airQualityIndexLoaded(const AirQualityIndex& index);
    void apiError(const QString& errorMessage);

    // Nowe sygnały
    void networkStatusChanged(bool available);
    void offlineModeChanged(bool offline);
    void dataSourceChanged(const QString& source); // "online" lub "offline"

private slots:
    void handleStationsReceived(const QJsonArray& stationsData);
    void handleSensorsReceived(const QJsonArray& sensorsData);
    void handleMeasurementsReceived(const QJsonObject& measurementsData);
    void handleAirQualityIndexReceived(const QJsonObject& indexData);
    void handleApiError(const QString& errorMessage);

    // Nowe sloty
    void onNetworkStatusChanged(bool available);
    void checkAndSwitchDataSource();

private:
    ApiClient* apiClient;
    DatabaseHandler* databaseHandler;
    DataAnalyzer* dataAnalyzer;
    NetworkChecker* networkChecker;
    QThreadPool* threadPool;
    mutable QMutex mutex; // Do synchronizacji dostępu do współdzielonych zasobów

    QVector<Station> stations;
    QVector<Sensor> sensorsForCurrentStation;

    bool offlineMode;
    bool autoSwitchToOffline;

    // Informacje o aktualnie wybranych elementach do automatycznego zapisu
    int currentStationId;
    int currentSensorId;

    // Metody pomocnicze
    void saveStationsToCache(const QVector<Station>& stations);
    QVector<Station> loadStationsFromCache() const;

    // Metody do przetwarzania danych w tle
    void processStationsDataAsync(const QJsonArray& stationsData);
    void processSensorsDataAsync(const QJsonArray& sensorsData);
    void processMeasurementsDataAsync(const QJsonObject& measurementsData, int sensorId);
};

#endif // DATAMANAGER_H
