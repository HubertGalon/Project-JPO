#ifndef AIRQUALITYINDEX_H
#define AIRQUALITYINDEX_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class AirQualityIndex
{
public:
    AirQualityIndex();
    AirQualityIndex(const QJsonObject& json);

    int stationId;
    QDateTime calcDate;
    int indexLevelId;
    QString indexLevelName;
    QDateTime sourceDataDate;

    // Metoda pomocnicza do formatowania indeksu
    QString getColorizedIndexName() const;

    // Metoda konwertująca JSON na obiekt
    static AirQualityIndex fromJsonObject(const QJsonObject& json);
};

#endif // AIRQUALITYINDEX_H
