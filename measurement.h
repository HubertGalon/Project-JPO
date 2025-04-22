#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>

class Measurement
{
public:
    Measurement();
    Measurement(const QString& parameterCode, const QDateTime& date, double value);

    QString parameterCode;
    QDateTime date;
    double value;
    bool isValid;

    static QVector<Measurement> fromJsonObject(const QJsonObject& json);
};

#endif // MEASUREMENT_H
