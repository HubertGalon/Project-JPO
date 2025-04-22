#ifndef GEOCODER_H
#define GEOCODER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QPair>
#include <QThreadPool>
#include <QFuture>

// Struktura do przechowywania wyników geokodowania
struct GeocodingResult {
    QString formattedAddress;
    double latitude;
    double longitude;
    bool isValid;

    GeocodingResult() : latitude(0.0), longitude(0.0), isValid(false) {}

    GeocodingResult(const QString& address, double lat, double lon)
        : formattedAddress(address), latitude(lat), longitude(lon), isValid(true) {}
};

class Geocoder : public QObject
{
    Q_OBJECT

public:
    explicit Geocoder(QObject* parent = nullptr);
    ~Geocoder();

    // Metoda do konwersji adresu na współrzędne (asynchroniczna)
    void geocodeAddress(const QString& address);

signals:
    // Sygnał emitowany po zakończeniu geokodowania
    void geocodingFinished(const GeocodingResult& result);

    // Sygnał emitowany w przypadku błędu
    void geocodingError(const QString& errorMessage);

private:
    QNetworkAccessManager* networkManager;
    QThreadPool* threadPool;

    // Używamy OpenStreetMap Nominatim API (darmowe)
    const QString apiUrl = "https://nominatim.openstreetmap.org/search";

    // Metoda do wykonania zapytania geokodowania w osobnym wątku
    void geocodeAddressAsync(const QString& address);

    // Metoda do przetwarzania odpowiedzi
    void handleGeocodingResponse(const QByteArray& responseData);
};

#endif // GEOCODER_H
