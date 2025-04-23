#include "station.h"
#include <QJsonArray>
#include <QtMath>

Station::Station() :
    id(0),
    gegrLat(0.0),
    gegrLon(0.0),
    cityId(0)
{
}

Station::Station(const QJsonObject& json)
{
    qDebug() << "Parsowanie stacji z JSON:" << json;

    id = json["id"].toInt();
    stationName = json["stationName"].toString();

    // Sprawdzamy format współrzędnych w JSON
    qDebug() << "gegrLat w JSON:" << json["gegrLat"].toString() << "typ:" << json["gegrLat"].type();
    qDebug() << "gegrLon w JSON:" << json["gegrLon"].toString() << "typ:" << json["gegrLon"].type();

    // Próbujemy różne metody konwersji
    if (json["gegrLat"].isDouble()) {
        gegrLat = json["gegrLat"].toDouble();
    } else if (json["gegrLat"].isString()) {
        gegrLat = json["gegrLat"].toString().toDouble();
    } else {
        gegrLat = 0.0;
        qDebug() << "Błąd: Nie można odczytać gegrLat";
    }

    if (json["gegrLon"].isDouble()) {
        gegrLon = json["gegrLon"].toDouble();
    } else if (json["gegrLon"].isString()) {
        gegrLon = json["gegrLon"].toString().toDouble();
    } else {
        gegrLon = 0.0;
        qDebug() << "Błąd: Nie można odczytać gegrLon";
    }

    qDebug() << "Odczytane współrzędne:" << gegrLat << gegrLon;

    // Parsowanie informacji o mieście
    QJsonObject cityJson = json["city"].toObject();
    cityId = cityJson["id"].toInt();
    cityName = cityJson["name"].toString();

    // Parsowanie informacji o gminie
    QJsonObject communeJson = cityJson["commune"].toObject();
    communeName = communeJson["communeName"].toString();
    districtName = communeJson["districtName"].toString();
    provinceName = communeJson["provinceName"].toString();

    // Ulica
    addressStreet = json["addressStreet"].toString();
}

QString Station::getFullAddress() const
{
    QString address = stationName;

    if (!addressStreet.isEmpty()) {
        address += ", " + addressStreet;
    }

    address += ", " + cityName;

    return address;
}

double Station::calculateDistance(double targetLat, double targetLon) const
{
    // Implementacja wzoru Haversine do obliczania odległości między współrzędnymi
    const double earthRadius = 6371.0; // Promień Ziemi w km

    // Sprawdzanie, czy współrzędne są w odpowiednim zakresie
    if (gegrLat == 0.0 || gegrLon == 0.0 || targetLat == 0.0 || targetLon == 0.0) {
        qDebug() << "Błąd: Nieprawidłowe współrzędne: stacja (" << gegrLat << "," << gegrLon
                 << "), cel (" << targetLat << "," << targetLon << ")";
        return 999999.0; // Duża wartość, aby stacja nie została uwzględniona w wynikach
    }

    double dLat = qDegreesToRadians(targetLat - gegrLat);
    double dLon = qDegreesToRadians(targetLon - gegrLon);

    double a = qSin(dLat / 2) * qSin(dLat / 2) +
               qCos(qDegreesToRadians(gegrLat)) * qCos(qDegreesToRadians(targetLat)) *
                   qSin(dLon / 2) * qSin(dLon / 2);

    double c = 2 * qAtan2(qSqrt(a), qSqrt(1 - a));
    double distance = earthRadius * c;

    qDebug() << "Odległość od " << stationName << " (" << gegrLat << "," << gegrLon
             << ") do punktu (" << targetLat << "," << targetLon << "): " << distance << " km";

    return distance;
}
QVector<Station> Station::fromJsonArray(const QJsonArray& jsonArray)
{
    QVector<Station> stations;

    for (const QJsonValue& value : jsonArray) {
        if (value.isObject()) {
            stations.append(Station(value.toObject()));
        }
    }

    return stations;
}
