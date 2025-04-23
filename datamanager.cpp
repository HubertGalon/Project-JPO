#include "datamanager.h"
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

DataManager::DataManager(QObject* parent) : QObject(parent),
    offlineMode(false),
    autoSwitchToOffline(true),
    currentStationId(0),
    currentSensorId(0)
{
    apiClient = new ApiClient(this);
    databaseHandler = new DatabaseHandler(this);
    dataAnalyzer = new DataAnalyzer(this);
    networkChecker = new NetworkChecker(this);
    threadPool = new QThreadPool(this);

    // Ustawiamy liczbę wątków
    threadPool->setMaxThreadCount(QThread::idealThreadCount());
    qDebug() << "DataManager: Inicjalizacja puli wątków, maksymalna liczba wątków:" << threadPool->maxThreadCount();

    // Połączenia sygnałów
    connect(apiClient, &ApiClient::stationsReceived, this, &DataManager::handleStationsReceived);
    connect(apiClient, &ApiClient::sensorsReceived, this, &DataManager::handleSensorsReceived);
    connect(apiClient, &ApiClient::measurementsReceived, this, &DataManager::handleMeasurementsReceived);
    connect(apiClient, &ApiClient::airQualityIndexReceived, this, &DataManager::handleAirQualityIndexReceived);
    connect(apiClient, &ApiClient::errorOccurred, this, &DataManager::handleApiError);

    // Dodajemy połączenia dla NetworkChecker
    connect(networkChecker, &NetworkChecker::networkStatusChanged, this, &DataManager::onNetworkStatusChanged);

    // Uruchamiamy monitoring sieci
    networkChecker->startMonitoring(20000); // Sprawdzaj co 20 sekund
}

DataManager::~DataManager()
{
    threadPool->waitForDone(); // Czekamy na zakończenie wszystkich wątków
}

bool DataManager::isNetworkAvailable() const
{
    return networkChecker->isNetworkAvailable();
}

bool DataManager::isOfflineMode() const
{
    return offlineMode;
}

void DataManager::setOfflineMode(bool offline)
{
    if (offline != offlineMode) {
        offlineMode = offline;
        emit offlineModeChanged(offline);

        if (offline) {
            emit dataSourceChanged("offline");
        } else {
            emit dataSourceChanged("online");
        }
    }
}

bool DataManager::hasCachedData(int sensorId) const
{
    QVector<Measurement> measurements = databaseHandler->loadMeasurements(sensorId);
    return !measurements.isEmpty();
}

void DataManager::onNetworkStatusChanged(bool available)
{
    qDebug() << "Status sieci zmieniony na:" << (available ? "dostępna" : "niedostępna");
    emit networkStatusChanged(available);

    // Jeśli ustawiono automatyczne przełączanie i nie ma połączenia - przełącz na tryb offline
    if (!available && autoSwitchToOffline) {
        setOfflineMode(true);
    }

    // Jeśli połączenie wróciło, a jesteśmy w trybie offline - można zaproponować przełączenie
    // Nie przełączamy automatycznie, bo to może być denerwujące dla użytkownika
    if (available && offlineMode) {
        qDebug() << "Połączenie internetowe jest dostępne, ale aplikacja jest w trybie offline";
    }
}

void DataManager::checkAndSwitchDataSource()
{
    // Sprawdzamy czy mamy dostęp do sieci
    bool networkAvailable = isNetworkAvailable();

    // Jeśli nie ma sieci, a nie jesteśmy w trybie offline - przełącz na offline
    if (!networkAvailable && !offlineMode) {
        setOfflineMode(true);
    }
}

void DataManager::getAllStations()
{
    // Sprawdzamy dostępność sieci i tryb działania
    checkAndSwitchDataSource();

    // Najpierw próbujemy załadować z bazy danych
    QVector<Station> cachedStations = loadStationsFromCache();

    if (!cachedStations.isEmpty()) {
        // Używamy lokalnych danych
        stations = cachedStations;
        emit stationsLoaded(stations);

        // Jeśli mamy połączenie i nie jesteśmy w trybie offline, pobieramy świeże dane
        if (!offlineMode && isNetworkAvailable()) {
            apiClient->getAllStations();
        }
    } else {
        // Jeśli nie mamy w cache
        if (!offlineMode && isNetworkAvailable()) {
            apiClient->getAllStations();
        } else {
            // Jeśli nie ma danych w cache i nie ma internetu
            emit apiError("Brak połączenia internetowego i brak zapisanych stacji");
        }
    }
}

void DataManager::getStationsForCity(const QString& cityName)
{
    // Najpierw pobieramy wszystkie stacje, a potem filtrujemy
    connect(this, &DataManager::stationsLoaded, this, [this, cityName](const QVector<Station>& allStations) {
        QVector<Station> filteredStations = findStationsByCity(cityName);
        emit stationsLoaded(filteredStations);
    }, Qt::SingleShotConnection);

    getAllStations();
}

void DataManager::getStationsNearLocation(double latitude, double longitude, double radiusInKm)
{
    // Najpierw pobieramy wszystkie stacje, a potem filtrujemy po odległości
    connect(this, &DataManager::stationsLoaded, this, [this, latitude, longitude, radiusInKm](const QVector<Station>& allStations) {
        QVector<Station> nearbyStations = findStationsNearLocation(latitude, longitude, radiusInKm);
        emit stationsLoaded(nearbyStations);
    }, Qt::SingleShotConnection);

    getAllStations();
}

void DataManager::getSensors(int stationId)
{
    // Zapamiętujemy ID stacji dla automatycznego zapisu
    currentStationId = stationId;

    // Sprawdzamy dostępność sieci i tryb działania
    checkAndSwitchDataSource();

    // Próbujemy najpierw załadować z bazy
    QVector<Sensor> cachedSensors = databaseHandler->loadSensors(stationId);

    if (!cachedSensors.isEmpty()) {
        // Używamy lokalnych danych
        sensorsForCurrentStation = cachedSensors;
        emit sensorsLoaded(cachedSensors);
        emit dataSourceChanged("offline");

        // Jeśli mamy połączenie i nie jesteśmy w trybie offline, pobieramy świeże dane
        if (!offlineMode && isNetworkAvailable()) {
            apiClient->getSensors(stationId);
        }
    } else {
        // Jeśli nie mamy w cache
        if (!offlineMode && isNetworkAvailable()) {
            apiClient->getSensors(stationId);
        } else {
            // Jeśli nie ma danych w cache i nie ma internetu
            emit apiError("Brak połączenia internetowego i brak zapisanych danych o sensorach");
        }
    }
}

void DataManager::getMeasurements(int sensorId)
{
    try {
        // Zapamiętujemy ID sensora dla automatycznego zapisu
        currentSensorId = sensorId;

        // Sprawdzamy dostępność sieci i tryb działania
        checkAndSwitchDataSource();

        // Próbujemy najpierw pobrać dane z bazy
        QVector<Measurement> cachedMeasurements = loadMeasurementsFromDatabase(sensorId);

        if (!cachedMeasurements.isEmpty()) {
            // Używamy lokalnych danych
            emit measurementsLoaded(cachedMeasurements);
            emit dataSourceChanged("offline");

            // Jeśli mamy połączenie i nie jesteśmy w trybie offline, pobieramy świeże dane
            if (!offlineMode && isNetworkAvailable()) {
                apiClient->getMeasurements(sensorId);
            }
        } else {
            // Jeśli nie mamy w cache
            if (!offlineMode && isNetworkAvailable()) {
                apiClient->getMeasurements(sensorId);
            } else {
                // Jeśli nie ma danych w cache i nie ma internetu
                emit apiError("Brak połączenia internetowego i brak zapisanych pomiarów");
            }
        }
    } catch (const std::exception& e) {
        emit apiError(QString("Błąd podczas pobierania pomiarów: %1").arg(e.what()));
    } catch (...) {
        emit apiError("Nieznany błąd podczas pobierania pomiarów");
    }
}

void DataManager::getAirQualityIndex(int stationId)
{
    // Sprawdzamy dostępność sieci i tryb działania
    checkAndSwitchDataSource();

    // Indeks jakości powietrza jest mocno zależny od czasu, więc używamy go tylko online
    if (!offlineMode && isNetworkAvailable()) {
        apiClient->getAirQualityIndex(stationId);
    } else {
        emit apiError("Indeks jakości powietrza jest dostępny tylko w trybie online");
    }
}

QVector<Station> DataManager::findStationsByCity(const QString& cityName) const
{
    QMutexLocker locker(&mutex); // Blokujemy dostęp do stations
    QVector<Station> result;

    for (const Station& station : stations) {
        if (station.cityName.toLower() == cityName.toLower()) {
            result.append(station);
        }
    }

    return result;
}

QVector<Station> DataManager::findStationsNearLocation(double latitude, double longitude, double radiusInKm) const
{
    QMutexLocker locker(&mutex); // Blokujemy dostęp do stations
    QVector<Station> result;

    for (const Station& station : stations) {
        double distance = station.calculateDistance(latitude, longitude);
        if (distance <= radiusInKm) {
            result.append(station);
        }
    }

    // Sortowanie stacji według odległości
    std::sort(result.begin(), result.end(), [latitude, longitude](const Station& a, const Station& b) {
        return a.calculateDistance(latitude, longitude) < b.calculateDistance(latitude, longitude);
    });

    return result;
}

void DataManager::saveMeasurementsToDatabase(int sensorId, const QVector<Measurement>& measurements)
{
    // Zapisujemy wskaźnik this
    DataManager* thisPtr = this;

    // Zapisujemy dane do bazy w osobnym wątku
    QtConcurrent::run(threadPool, [thisPtr, sensorId, measurements]() {
        thisPtr->databaseHandler->saveMeasurements(sensorId, measurements);
    });
}

QVector<Measurement> DataManager::loadMeasurementsFromDatabase(int sensorId)
{
    return databaseHandler->loadMeasurements(sensorId);
}

void DataManager::handleStationsReceived(const QJsonArray& stationsData)
{
    // Przetwarzamy dane stacji w osobnym wątku
    processStationsDataAsync(stationsData);
}

void DataManager::handleSensorsReceived(const QJsonArray& sensorsData)
{
    // Przetwarzamy dane sensorów w osobnym wątku
    processSensorsDataAsync(sensorsData);
}

void DataManager::handleMeasurementsReceived(const QJsonObject& measurementsData)
{
    QVector<Measurement> measurements = Measurement::fromJsonObject(measurementsData);

    // Zapisujemy pobrane dane do bazy
    if (!measurements.isEmpty()) {
        // Zakładamy, że wszystkie pomiary dotyczą tego samego sensora
        int sensorId = currentSensorId;
        processMeasurementsDataAsync(measurementsData, sensorId);
    }

    emit measurementsLoaded(measurements);
    emit dataSourceChanged("online");
}

void DataManager::handleAirQualityIndexReceived(const QJsonObject& indexData)
{
    AirQualityIndex index = AirQualityIndex::fromJsonObject(indexData);
    emit airQualityIndexLoaded(index);
}

void DataManager::handleApiError(const QString& errorMessage)
{
    // Jeśli jest błąd API i mamy dane w cache, przełączamy się na tryb offline
    if (offlineMode || !isNetworkAvailable()) {
        qDebug() << "Błąd API, przełączam na tryb offline: " << errorMessage;
        setOfflineMode(true);

        // Próbujemy załadować dane z cache, jeśli są dostępne
        if (currentSensorId != 0 && hasCachedData(currentSensorId)) {
            QVector<Measurement> cachedMeasurements = loadMeasurementsFromDatabase(currentSensorId);
            emit measurementsLoaded(cachedMeasurements);
            emit dataSourceChanged("offline");
            return;
        }
    }

    // Propagujemy błąd
    emit apiError(errorMessage);
}

void DataManager::saveStationsToCache(const QVector<Station>& stations)
{
    // Zapisujemy stacje do lokalnego pliku cache w osobnym wątku
    QtConcurrent::run(threadPool, [this, stations]() {
        databaseHandler->saveStations(stations);
    });
}

QVector<Station> DataManager::loadStationsFromCache() const
{
    return databaseHandler->loadStations();
}

void DataManager::processStationsDataAsync(const QJsonArray& stationsData)
{
    // Zapisujemy wskaźnik this
    DataManager* thisPtr = this;

    // Przetwarzamy dane stacji w osobnym wątku
    auto future = QtConcurrent::run(threadPool, [thisPtr, stationsData]() {
        try {
            QVector<Station> parsedStations = Station::fromJsonArray(stationsData);

            // Aktualizujemy dane stacji i emitujemy sygnał w wątku głównym
            QMetaObject::invokeMethod(thisPtr, [thisPtr, parsedStations]() {
                try {
                    {
                        QMutexLocker locker(&thisPtr->mutex);
                        thisPtr->stations = parsedStations;
                    }
                    emit thisPtr->stationsLoaded(parsedStations);
                    emit thisPtr->dataSourceChanged("online");

                    // Zapisujemy stacje do cache
                    thisPtr->saveStationsToCache(parsedStations);
                } catch (const std::exception& e) {
                    emit thisPtr->apiError(QString("Błąd aktualizacji danych stacji: %1").arg(e.what()));
                } catch (...) {
                    emit thisPtr->apiError("Nieznany błąd aktualizacji danych stacji");
                }
            }, Qt::QueuedConnection);
        } catch (const std::exception& e) {
            QMetaObject::invokeMethod(thisPtr, "apiError", Qt::QueuedConnection,
                                      Q_ARG(QString, QString("Błąd przetwarzania danych stacji: %1").arg(e.what())));
        } catch (...) {
            QMetaObject::invokeMethod(thisPtr, "apiError", Qt::QueuedConnection,
                                      Q_ARG(QString, "Nieznany błąd przetwarzania danych stacji"));
        }
    });
}


void DataManager::processSensorsDataAsync(const QJsonArray& sensorsData)
{
    // Zapisujemy wskaźnik this
    DataManager* thisPtr = this;

    // Przetwarzamy dane sensorów w osobnym wątku
    auto future = QtConcurrent::run(threadPool, [thisPtr, sensorsData]() {
        QVector<Sensor> parsedSensors = Sensor::fromJsonArray(sensorsData);

        // Aktualizujemy dane sensorów i emitujemy sygnał w wątku głównym
        QMetaObject::invokeMethod(thisPtr, [thisPtr, parsedSensors]() {
            {
                QMutexLocker locker(&thisPtr->mutex);
                thisPtr->sensorsForCurrentStation = parsedSensors;
            }
            emit thisPtr->sensorsLoaded(parsedSensors);
            emit thisPtr->dataSourceChanged("online");

            // Zapisujemy sensory do cache jeśli nie są puste
            if (!parsedSensors.isEmpty()) {
                int stationId = thisPtr->currentStationId;
                QtConcurrent::run(thisPtr->threadPool, [thisPtr, stationId, parsedSensors]() {
                    thisPtr->databaseHandler->saveSensors(stationId, parsedSensors);
                });
            }
        }, Qt::QueuedConnection);
    });
}

void DataManager::processMeasurementsDataAsync(const QJsonObject& measurementsData, int sensorId)
{
    // Zapisujemy wskaźnik this
    DataManager* thisPtr = this;

    // Przetwarzamy dane pomiarów w osobnym wątku
    auto future = QtConcurrent::run(threadPool, [thisPtr, measurementsData, sensorId]() {
        QVector<Measurement> parsedMeasurements = Measurement::fromJsonObject(measurementsData);

        // Zapisujemy pomiary do bazy danych
        if (!parsedMeasurements.isEmpty()) {
            thisPtr->saveMeasurementsToDatabase(sensorId, parsedMeasurements);
        }
    });
}
