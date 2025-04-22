#ifndef SENSOR_H
#define SENSOR_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>

class Sensor
{
public:
    Sensor();
    Sensor(const QJsonObject& json);

    int id;
    int stationId;
    QString paramName;
    QString paramFormula;
    QString paramCode;
    int idParam;

    static QVector<Sensor> fromJsonArray(const QJsonArray& jsonArray);
};

#endif // SENSOR_H
