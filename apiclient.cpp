#include "apiclient.h"
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

ApiClient::ApiClient(QObject* parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    threadPool = new QThreadPool(this);

    // Ustawiamy maksymalną liczbę wątków
    threadPool->setMaxThreadCount(QThread::idealThreadCount());
    qDebug() << "ApiClient: Inicjalizacja puli wątków, maksymalna liczba wątków:" << threadPool->maxThreadCount();
}

ApiClient::~ApiClient()
{
    threadPool->waitForDone(); // Czekamy na zakończenie wszystkich wątków przed zniszczeniem obiektu
}

void ApiClient::getAllStations()
{
    sendRequestAsync("/station/findAll", "stations");
}

void ApiClient::getSensors(int stationId)
{
    sendRequestAsync(QString("/station/sensors/%1").arg(stationId), "sensors");
}

void ApiClient::getMeasurements(int sensorId)
{
    sendRequestAsync(QString("/data/getData/%1").arg(sensorId), "measurements");
}

void ApiClient::getAirQualityIndex(int stationId)
{
    sendRequestAsync(QString("/aqindex/getIndex/%1").arg(stationId), "airQualityIndex");
}

void ApiClient::sendRequestAsync(const QString& endpoint, const QString& requestType)
{
    QUrl url(QString("%1%2").arg(BASE_URL, endpoint));
    qDebug() << "Wysyłanie asynchronicznego zapytania do:" << url.toString();

    // Tworzymy kopię wskaźnika this, aby uniknąć problemów z konwersją
    ApiClient* thisPtr = this;

    // Tworzymy funkcję lambda, która zostanie wykonana w osobnym wątku
    auto future = QtConcurrent::run(threadPool, [thisPtr, url, requestType]() {
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        // Tworzymy lokalny QNetworkAccessManager dla tego wątku
        QNetworkAccessManager manager;
        QEventLoop loop;

        // Łączymy sygnał finished z lokalną pętlą zdarzeń
        QObject::connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

        // Wysyłamy zapytanie synchronicznie w tym wątku
        QNetworkReply* reply = manager.get(request);

        // Czekamy na odpowiedź
        loop.exec();

        // Obsługa błędów
        if (reply->error() != QNetworkReply::NoError) {
            QString errorString = reply->errorString();
            qDebug() << "Błąd API:" << errorString;

            // Emitujemy sygnał o błędzie w wątku głównym
            QMetaObject::invokeMethod(thisPtr, "errorOccurred", Qt::QueuedConnection,
                                      Q_ARG(QString, "Błąd połączenia: " + errorString));
            reply->deleteLater();
            return;
        }

        // Odczytujemy dane odpowiedzi
        QByteArray responseData = reply->readAll();
        reply->deleteLater();

        // Przetwarzamy dane w wątku głównym
        QMetaObject::invokeMethod(thisPtr, [thisPtr, responseData, requestType]() {
            thisPtr->handleResponseData(responseData, requestType);
        }, Qt::QueuedConnection);
    });
}

void ApiClient::handleResponseData(const QByteArray& responseData, const QString& requestType)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

    if (jsonDoc.isNull()) {
        emit errorOccurred("Otrzymano nieprawidłowe dane JSON");
        return;
    }

    // Obsługa różnych typów odpowiedzi
    if (requestType == "stations") {
        if (jsonDoc.isArray()) {
            emit stationsReceived(jsonDoc.array());
        }
        else {
            emit errorOccurred("Nieoczekiwany format odpowiedzi dla stacji");
        }
    }
    else if (requestType == "sensors") {
        if (jsonDoc.isArray()) {
            emit sensorsReceived(jsonDoc.array());
        }
        else {
            emit errorOccurred("Nieoczekiwany format odpowiedzi dla sensorów");
        }
    }
    else if (requestType == "measurements") {
        if (jsonDoc.isObject()) {
            emit measurementsReceived(jsonDoc.object());
        }
        else {
            emit errorOccurred("Nieoczekiwany format odpowiedzi dla pomiarów");
        }
    }
    else if (requestType == "airQualityIndex") {
        if (jsonDoc.isObject()) {
            emit airQualityIndexReceived(jsonDoc.object());
        }
        else {
            emit errorOccurred("Nieoczekiwany format odpowiedzi dla indeksu jakości");
        }
    }
}
