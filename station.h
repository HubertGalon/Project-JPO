#ifndef STATION_H
#define STATION_H

#include <QString>
#include <QJsonObject>

class Station
{
public:
    Station();
    Station(const QJsonObject& json);

    int id;
    QString stationName;
    double gegrLat;
    double gegrLon;
    int cityId;
    QString cityName;
    QString communeName;
    QString districtName;
    QString provinceName;
    QString addressStreet;

    QString getFullAddress() const;
    double calculateDistance(double targetLat, double targetLon) const;

    static QVector<Station> fromJsonArray(const QJsonArray& jsonArray);
};

#endif // STATION_H
