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

QString AirQualityIndex::getColorizedIndexName() const
{
    // Zwrócenie nazwy indeksu z odpowiednim kolorem na podstawie ID
    switch (indexLevelId) {
    case 0:
        return "<span style='color: #00FF00;'>Bardzo dobry</span>";
    case 1:
        return "<span style='color: #80FF00;'>Dobry</span>";
    case 2:
        return "<span style='color: #FFFF00;'>Umiarkowany</span>";
    case 3:
        return "<span style='color: #FF8000;'>Dostateczny</span>";
    case 4:
        return "<span style='color: #FF0000;'>Zły</span>";
    case 5:
        return "<span style='color: #800000;'>Bardzo zły</span>";
    default:
        return "<span style='color: #808080;'>Brak danych</span>";
    }
}

AirQualityIndex AirQualityIndex::fromJsonObject(const QJsonObject& json)
{
    return AirQualityIndex(json);
}
