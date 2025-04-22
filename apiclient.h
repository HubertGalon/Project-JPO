#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThreadPool>
#include <QMutex>
#include <QFuture>

class ApiClient : public QObject
{
    Q_OBJECT

public:
    explicit ApiClient(QObject* parent = nullptr);
    virtual ~ApiClient();

    // Endpoint API GIOŚ
    static constexpr const char* BASE_URL = "https://api.gios.gov.pl/pjp-api/rest";

    // Metody pobierania danych (wielowątkowe)
    void getAllStations();
    void getSensors(int stationId);
    void getMeasurements(int sensorId);
    void getAirQualityIndex(int stationId);

signals:
    void stationsReceived(const QJsonArray& stationsData);
    void sensorsReceived(const QJsonArray& sensorsData);
    void measurementsReceived(const QJsonObject& measurementsData);
    void airQualityIndexReceived(const QJsonObject& indexData);
    void errorOccurred(const QString& errorMessage);

private:
    QNetworkAccessManager* networkManager;
    QThreadPool* threadPool;
    QMutex mutex; // Do synchronizacji dostępu do współdzielonych zasobów

    // Metoda pomocnicza do wysyłania zapytań w osobnym wątku
    void sendRequestAsync(const QString& endpoint, const QString& requestType);

    // Metody obsługi odpowiedzi dla poszczególnych typów zapytań
    void handleResponseData(const QByteArray& responseData, const QString& requestType);
};

#endif // APICLIENT_H
