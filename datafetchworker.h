#ifndef DATAFETCHWORKER_H
#define DATAFETCHWORKER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QJsonArray>
#include <QJsonObject>

// Typ zadania do wykonania
enum class FetchTaskType {
    FetchAllStations,
    FetchSensors,
    FetchMeasurements,
    FetchAirQualityIndex
};

// Struktura zadania
struct FetchTask {
    FetchTaskType type;
    int id;  // stationId lub sensorId

    FetchTask(FetchTaskType taskType, int entityId = 0) : type(taskType), id(entityId) {}
};

class DataFetchWorker : public QObject
{
    Q_OBJECT

public:
    explicit DataFetchWorker(QObject* parent = nullptr);
    ~DataFetchWorker();

    // Dodaje zadanie do kolejki
    void addTask(const FetchTask& task);

    // Zatrzymuje wątek
    void stop();

signals:
    // Sygnały wysyłane po zakończeniu zadań
    void stationsDataFetched(const QJsonArray& stationsData);
    void sensorsDataFetched(int stationId, const QJsonArray& sensorsData);
    void measurementsDataFetched(int sensorId, const QJsonObject& measurementsData);
    void airQualityIndexFetched(int stationId, const QJsonObject& indexData);

    // Sygnał błędu
    void errorOccurred(const QString& errorMessage);

public slots:
    // Główna metoda wątku
    void process();

private:
    QMutex mutex;
    QWaitCondition condition;
    QQueue<FetchTask> taskQueue;
    bool abort;

    // Metody pobierające dane z API
    QJsonArray fetchAllStations();
    QJsonArray fetchSensors(int stationId);
    QJsonObject fetchMeasurements(int sensorId);
    QJsonObject fetchAirQualityIndex(int stationId);

    // Pomocnicza metoda do wykonywania zapytań HTTP
    QByteArray performHttpRequest(const QString& endpoint);
};

#endif // DATAFETCHWORKER_H
