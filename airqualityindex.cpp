#include "airqualityindex.h"

AirQualityIndex::AirQualityIndex() :
    stationId(0),
    indexLevelId(0)
{
}

AirQualityIndex::AirQualityIndex(const QJsonObject& json)
{
    stationId = json["id"].toInt();

    QString calcDateStr = json["stCalcDate"].toString();
    calcDate = QDateTime::fromString(calcDateStr, Qt::ISODate);

    // Parsowanie poziomu indeksu
    QJsonObject indexLevelJson = json["stIndexLevel"].toObject();
    indexLevelId = indexLevelJson["id"].toInt();
    indexLevelName = indexLevelJson["indexLevelName"].toString();

    QString sourceDateStr = json["stSourceDataDate"].toString();
    sourceDataDate = QDateTime::fromString(sourceDateStr, Qt::ISODate);
}


AirQualityIndex AirQualityIndex::fromJsonObject(const QJsonObject& json)
{
    return AirQualityIndex(json);
}
