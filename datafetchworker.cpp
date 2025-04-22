#include "datafetchworker.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QEventLoop>
#include <QDebug>

DataFetchWorker::DataFetchWorker(QObject* parent) : QObject(parent), abort(false)
{
}

DataFetchWorker::~DataFetchWorker()
{
    // Ustawienie flagi abort zabezpiecza przed wyścigiem, gdy destruktor jest wywołany
    // a wątek jest w trakcie przetwarzania
    QMutexLocker locker(&mutex);
    abort = true;
    condition.wakeOne();
    mutex.unlock();
}

void DataFetchWorker::addTask(const FetchTask& task)
{
    QMutexLocker locker(&mutex);
    taskQueue.enqueue(task);
    condition.wakeOne();
}

void DataFetchWorker::stop()
{
    QMutexLocker locker(&mutex);
    abort = true;
    condition.wakeOne();
}

void DataFetchWorker::process()
{
    forever {
        // Pobieramy zadanie z kolejki
        FetchTask task;
        {
            QMutexLocker locker(&mutex);

            // Czekamy na nowe zadanie lub sygnał abort
            while (!abort && taskQueue.isEmpty()) {
                condition.wait(&mutex);
            }

            // Jeśli ustawiono flagę abort, kończymy
            if (abort) {
                return;
            }

            task = taskQueue.dequeue();
        }

        try {
            // Wykonujemy odpowiednie zadanie
            switch (task.type) {
            case FetchTaskType::FetchAllStations: {
                QJsonArray stationsData = fetchAllStations();
                emit stationsDataFetched(stationsData);
                break;
            }

            case FetchTaskType::FetchSensors: {
                QJsonArray sensorsData = fetchSensors(task.id);
                emit sensorsDataFetched(task.id, sensorsData);
                break;
            }

            case FetchTaskType::FetchMeasurements: {
                QJsonObject measurementsData = fetchMeasurements(task.id);
                emit measurementsDataFetched(task.id, measurementsData);
                break;
            }

            case FetchTaskType::FetchAirQualityIndex: {
                QJsonObject indexData = fetchAirQualityIndex(task.id);
                emit airQualityIndexFetched(task.id, indexData);
                break;
            }
            }
        } catch (const std::exception& e) {
            emit errorOccurred(QString("Błąd: %1").arg(e.what()));
        }
    }
}

QJsonArray DataFetchWorker::fetchAllStations()
{
    QByteArray responseData = performHttpRequest("/station/findAll");

    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (!jsonDoc.isArray()) {
        throw std::runtime_error("Nieprawidłowy format odpowiedzi dla stacji");
    }

    return jsonDoc.array();
}

QJsonArray DataFetchWorker::fetchSensors(int stationId)
{
    QByteArray responseData = performHttpRequest(QString("/station/sensors/%1").arg(stationId));

    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (!jsonDoc.isArray()) {
        throw std::runtime_error("Nieprawidłowy format odpowiedzi dla sensorów");
    }

    return jsonDoc.array();
}

QJsonObject DataFetchWorker::fetchMeasurements(int sensorId)
{
    QByteArray responseData = performHttpRequest(QString("/data/getData/%1").arg(sensorId));

    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (!jsonDoc.isObject()) {
        throw std::runtime_error("Nieprawidłowy format odpowiedzi dla pomiarów");
    }

    return jsonDoc.object();
}

QJsonObject DataFetchWorker::fetchAirQualityIndex(int stationId)
{
    QByteArray responseData = performHttpRequest(QString("/aqindex/getIndex/%1").arg(stationId));

    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (!jsonDoc.isObject()) {
        throw std::runtime_error("Nieprawidłowy format odpowiedzi dla indeksu jakości powietrza");
    }

    return jsonDoc.object();
}

QByteArray DataFetchWorker::performHttpRequest(const QString& endpoint)
{
    // Bazowy URL API GIOŚ
    const QString baseUrl = "https://api.gios.gov.pl/pjp-api/rest";

    // Tworzymy pełny URL
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Używamy blokującego podejścia, ponieważ jesteśmy już w osobnym wątku
    QNetworkAccessManager manager;
    QEventLoop loop;

    QNetworkReply* reply = manager.get(request);

    // Łączymy sygnał finished z wyjściem z pętli zdarzeń
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    // Uruchamiamy pętlę zdarzeń, która będzie działać do czasu otrzymania odpowiedzi
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        QString errorString = reply->errorString();
        qDebug() << "Błąd API:" << errorString;

        reply->deleteLater();
        throw std::runtime_error(errorString.toStdString());
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    return responseData;
}
