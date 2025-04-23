#include "geocoder.h"
#include <QDebug>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <QEventLoop>

Geocoder::Geocoder(QObject* parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    threadPool = new QThreadPool(this);

    // Ustawiamy maksymalną liczbę wątków
    threadPool->setMaxThreadCount(QThread::idealThreadCount());
    qDebug() << "Geocoder: Inicjalizacja puli wątków, maksymalna liczba wątków:" << threadPool->maxThreadCount();
}

Geocoder::~Geocoder()
{
    threadPool->waitForDone(); // Czekamy na zakończenie wszystkich wątków przed zniszczeniem obiektu
}

void Geocoder::geocodeAddress(const QString& address)
{
    if (address.isEmpty()) {
        emit geocodingError("Pusty adres");
        return;
    }

    // Uruchamiamy geokodowanie w osobnym wątku
    geocodeAddressAsync(address);
}

void Geocoder::geocodeAddressAsync(const QString& address)
{
    // Tworzymy kopię wskaźnika this, aby uniknąć problemów z konwersją
    Geocoder* thisPtr = this;

    // Tworzymy funkcję lambda, która zostanie wykonana w osobnym wątku
    auto future = QtConcurrent::run(threadPool, [thisPtr, address]() {
        try {
            QUrl url(thisPtr->apiUrl);
            QUrlQuery query;

            // Parametry zapytania dla API Nominatim
            query.addQueryItem("q", address);
            query.addQueryItem("format", "json");
            query.addQueryItem("limit", "1");
            query.addQueryItem("addressdetails", "1");
            query.addQueryItem("countrycodes", "pl");
            query.addQueryItem("email", "example@example.com");

            url.setQuery(query);

            QNetworkRequest request(url);
            request.setHeader(QNetworkRequest::UserAgentHeader, "AirQualityMonitor/1.0");
            request.setRawHeader("Accept-Language", "pl,en");

            qDebug() << "Wysyłanie asynchronicznego zapytania geokodowania:" << url.toString();

            // Tworzymy lokalny QNetworkAccessManager dla tego wątku
            QNetworkAccessManager manager;
            QEventLoop loop;
            QTimer timer;

            // Ustawiamy timeout
            timer.setSingleShot(true);
            timer.setInterval(10000); // 10 sekund timeout

            // Łączymy sygnały
            QObject::connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
            QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

            // Rozpoczynamy odliczanie i wysyłamy zapytanie
            timer.start();
            QNetworkReply* reply = manager.get(request);

            // Czekamy na odpowiedź lub timeout
            loop.exec();

            // Sprawdzamy, czy wystąpił timeout
            if (timer.isActive()) {
                timer.stop();
            } else {
                // Timeout
                reply->abort();

                // Emitujemy sygnał o błędzie w wątku głównym
                QMetaObject::invokeMethod(thisPtr, "geocodingError", Qt::QueuedConnection,
                                          Q_ARG(QString, "Przekroczony czas oczekiwania na odpowiedź geokodowania - sprawdź połączenie z internetem"));
                reply->deleteLater();
                return;
            }

            // Sprawdzamy, czy wystąpił błąd
            if (reply->error() != QNetworkReply::NoError) {
                QString errorString = reply->errorString();
                qDebug() << "Błąd geokodowania:" << errorString;

                // Emitujemy sygnał o błędzie w wątku głównym
                QMetaObject::invokeMethod(thisPtr, "geocodingError", Qt::QueuedConnection,
                                          Q_ARG(QString, "Błąd połączenia: " + errorString));
                reply->deleteLater();
                return;
            }

            // Odczytujemy dane odpowiedzi
            QByteArray responseData = reply->readAll();
            reply->deleteLater();

            // Przetwarzamy dane w wątku głównym
            QMetaObject::invokeMethod(thisPtr, [thisPtr, responseData]() {
                try {
                    thisPtr->handleGeocodingResponse(responseData);
                } catch (const std::exception& e) {
                    emit thisPtr->geocodingError(QString("Błąd przetwarzania odpowiedzi: %1").arg(e.what()));
                } catch (...) {
                    emit thisPtr->geocodingError("Nieznany błąd przetwarzania odpowiedzi");
                }
            }, Qt::QueuedConnection);
        } catch (const std::exception& e) {
            QMetaObject::invokeMethod(thisPtr, "geocodingError", Qt::QueuedConnection,
                                      Q_ARG(QString, QString("Błąd geokodowania: %1").arg(e.what())));
        } catch (...) {
            QMetaObject::invokeMethod(thisPtr, "geocodingError", Qt::QueuedConnection,
                                      Q_ARG(QString, "Nieznany błąd geokodowania"));
        }
    });
}

void Geocoder::handleGeocodingResponse(const QByteArray& responseData)
{
    try {
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &jsonError);

        if (jsonError.error != QJsonParseError::NoError) {
            throw std::runtime_error(jsonError.errorString().toStdString());
        }

        if (jsonDoc.isNull() || !jsonDoc.isArray()) {
            emit geocodingError("Otrzymano nieprawidłowe dane JSON");
            return;
        }

        QJsonArray results = jsonDoc.array();

        if (results.isEmpty()) {
            emit geocodingError("Nie znaleziono adresu");
            return;
        }

        // Bierzemy pierwszy wynik (najbardziej odpowiadający)
        QJsonObject result = results.first().toObject();

        double lat = result["lat"].toString().toDouble();
        double lon = result["lon"].toString().toDouble();
        QString displayName = result["display_name"].toString();

        GeocodingResult geocodingResult(displayName, lat, lon);

        qDebug() << "Pomyślnie zgeokodowano adres:" << displayName;
        qDebug() << "Współrzędne:" << lat << "," << lon;

        emit geocodingFinished(geocodingResult);
    } catch (const std::exception& e) {
        emit geocodingError(QString("Błąd przetwarzania odpowiedzi geokodowania: %1").arg(e.what()));
    } catch (...) {
        emit geocodingError("Nieznany błąd przetwarzania odpowiedzi geokodowania");
    }
}
