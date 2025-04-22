#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMutex>

#include "station.h"
#include "sensor.h"
#include "measurement.h"

class DatabaseHandler : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseHandler(QObject *parent = nullptr);
    ~DatabaseHandler();

    // Operacje na stacjach
    void saveStations(const QVector<Station>& stations);
    QVector<Station> loadStations() const;

    // Operacje na sensorach
    void saveSensors(int stationId, const QVector<Sensor>& sensors);
    QVector<Sensor> loadSensors(int stationId) const;

    // Operacje na pomiarach
    void saveMeasurements(int sensorId, const QVector<Measurement>& measurements);
    QVector<Measurement> loadMeasurements(int sensorId) const;

private:
    QDir dataDir;
    mutable QMutex mutex; // Muteks do synchronizacji dostępu do plików

    // Inicjalizacja katalogu danych
    void initDataDirectory();



    // Metody obsługi plików JSON
    bool saveJsonToFile(const QString& filePath, const QJsonDocument& jsonDoc) const;
    QJsonDocument loadJsonFromFile(const QString& filePath) const;

protected:
    // Metody pomocnicze
    QString getStationsFilePath() const;
    QString getSensorsFilePath(int stationId) const;
    QString getMeasurementsFilePath(int sensorId) const;
};



#endif // DATABASEHANDLER_H
