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

    try {
        if (json.isEmpty()) {
            throw std::runtime_error("Otrzymano pusty obiekt JSON");
        }

        if (!json.contains("key")) {
            throw std::runtime_error("Brak wymaganego pola 'key' w danych JSON");
        }

        QString paramCode = json["key"].toString();

        if (!json.contains("values")) {
            throw std::runtime_error("Brak wymaganego pola 'values' w danych JSON");
        }

        QJsonArray valuesArray = json["values"].toArray();

        for (const QJsonValue& value : valuesArray) {
            try {
                QJsonObject valueObj = value.toObject();

                if (!valueObj.contains("date")) {
                    qWarning() << "Brak pola 'date' w obiekcie pomiaru, pomijam";
                    continue;
                }

                QString dateStr = valueObj["date"].toString();
                QDateTime dateTime = QDateTime::fromString(dateStr, Qt::ISODate);

                if (!dateTime.isValid()) {
                    qWarning() << "Nieprawidłowy format daty:" << dateStr << ", pomijam";
                    continue;
                }

                if (valueObj["value"].isNull()) {
                    // Pomijamy NULL wartości
                    continue;
                }

                if (!valueObj["value"].isDouble() && !valueObj["value"].isString()) {
                    qWarning() << "Nieprawidłowy format wartości pomiaru, pomijam";
                    continue;
                }

                double measureValue = valueObj["value"].toDouble();

                measurements.append(Measurement(paramCode, dateTime, measureValue));
            } catch (const std::exception& e) {
                qWarning() << "Błąd przetwarzania pojedynczego pomiaru:" << e.what();
                // Kontynuujemy pętlę, aby przetworzyć pozostałe pomiary
            }
        }
    } catch (const std::exception& e) {
        qCritical() << "Błąd przetwarzania danych pomiarów:" << e.what();
        // Zwracamy to co udało się przetworzyć przed wystąpieniem błędu
    } catch (...) {
        qCritical() << "Nieznany błąd przetwarzania danych pomiarów";
        // Zwracamy to co udało się przetworzyć przed wystąpieniem błędu
    }

    return measurements;
}
