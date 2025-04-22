#include "sensor.h"

Sensor::Sensor() :
    id(0),
    stationId(0),
    idParam(0)
{
}

Sensor::Sensor(const QJsonObject& json)
{
    id = json["id"].toInt();
    stationId = json["stationId"].toInt();

    // Parsowanie informacji o parametrze
    QJsonObject paramJson = json["param"].toObject();
    paramName = paramJson["paramName"].toString();
    paramFormula = paramJson["paramFormula"].toString();
    paramCode = paramJson["paramCode"].toString();
    idParam = paramJson["idParam"].toInt();
}

QVector<Sensor> Sensor::fromJsonArray(const QJsonArray& jsonArray)
{
    QVector<Sensor> sensors;

    for (const QJsonValue& value : jsonArray) {
        if (value.isObject()) {
            sensors.append(Sensor(value.toObject()));
        }
    }

    return sensors;
}
