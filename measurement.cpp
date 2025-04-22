#include "measurement.h"

Measurement::Measurement() :
    value(0.0),
    isValid(false)
{
}

Measurement::Measurement(const QString& parameterCode, const QDateTime& date, double value) :
    parameterCode(parameterCode),
    date(date),
    value(value),
    isValid(true)
{
}

QVector<Measurement> Measurement::fromJsonObject(const QJsonObject& json)
{
    QVector<Measurement> measurements;

    QString paramCode = json["key"].toString();

    QJsonArray valuesArray = json["values"].toArray();
    for (const QJsonValue& value : valuesArray) {
        QJsonObject valueObj = value.toObject();

        QString dateStr = valueObj["date"].toString();
        QDateTime dateTime = QDateTime::fromString(dateStr, Qt::ISODate);

        if (valueObj["value"].isNull()) {
            // Pomijamy NULL wartości
            continue;
        }

        double measureValue = valueObj["value"].toDouble();

        measurements.append(Measurement(paramCode, dateTime, measureValue));
    }

    return measurements;
}
