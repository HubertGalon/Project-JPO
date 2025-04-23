#include "databasehandler.h"
#include <QDebug>
#include <QStandardPaths>
#include <QMutexLocker>

DatabaseHandler::DatabaseHandler(QObject* parent) : QObject(parent)
{
    initDataDirectory();
}

DatabaseHandler::~DatabaseHandler()
{
}

void DatabaseHandler::saveStations(const QVector<Station>& stations)
{
    QJsonArray stationsArray;

    for (const Station& station : stations) {
        QJsonObject stationObj;
        stationObj["id"] = station.id;
        stationObj["stationName"] = station.stationName;
        stationObj["gegrLat"] = station.gegrLat;
        stationObj["gegrLon"] = station.gegrLon;
        stationObj["cityId"] = station.cityId;
        stationObj["cityName"] = station.cityName;
        stationObj["communeName"] = station.communeName;
        stationObj["districtName"] = station.districtName;
        stationObj["provinceName"] = station.provinceName;
        stationObj["addressStreet"] = station.addressStreet;

        stationsArray.append(stationObj);
    }

    QJsonDocument jsonDoc(stationsArray);

    // Zabezpieczamy dostęp do pliku za pomocą mutexa
    QMutexLocker locker(&mutex);
    saveJsonToFile(getStationsFilePath(), jsonDoc);
}

QVector<Station> DatabaseHandler::loadStations() const
{
    QVector<Station> stations;

    // Zabezpieczamy dostęp do pliku za pomocą mutexa
    QMutexLocker locker(&mutex);
    QJsonDocument jsonDoc = loadJsonFromFile(getStationsFilePath());

    if (jsonDoc.isNull() || !jsonDoc.isArray()) {
        return stations;
    }

    QJsonArray stationsArray = jsonDoc.array();

    for (const QJsonValue& value : stationsArray) {
        QJsonObject stationObj = value.toObject();

        Station station;
        station.id = stationObj["id"].toInt();
        station.stationName = stationObj["stationName"].toString();
        station.gegrLat = stationObj["gegrLat"].toDouble();
        station.gegrLon = stationObj["gegrLon"].toDouble();
        station.cityId = stationObj["cityId"].toInt();
        station.cityName = stationObj["cityName"].toString();
        station.communeName = stationObj["communeName"].toString();
        station.districtName = stationObj["districtName"].toString();
        station.provinceName = stationObj["provinceName"].toString();
        station.addressStreet = stationObj["addressStreet"].toString();

        stations.append(station);
    }

    return stations;
}

void DatabaseHandler::saveSensors(int stationId, const QVector<Sensor>& sensors)
{
    QJsonArray sensorsArray;

    for (const Sensor& sensor : sensors) {
        QJsonObject sensorObj;
        sensorObj["id"] = sensor.id;
        sensorObj["stationId"] = sensor.stationId;
        sensorObj["paramName"] = sensor.paramName;
        sensorObj["paramFormula"] = sensor.paramFormula;
        sensorObj["paramCode"] = sensor.paramCode;
        sensorObj["idParam"] = sensor.idParam;

        sensorsArray.append(sensorObj);
    }

    QJsonDocument jsonDoc(sensorsArray);

    // Zabezpieczamy dostęp do pliku za pomocą mutexa
    QMutexLocker locker(&mutex);
    saveJsonToFile(getSensorsFilePath(stationId), jsonDoc);
}

QVector<Sensor> DatabaseHandler::loadSensors(int stationId) const
{
    QVector<Sensor> sensors;

    // Zabezpieczamy dostęp do pliku za pomocą mutexa
    QMutexLocker locker(&mutex);
    QJsonDocument jsonDoc = loadJsonFromFile(getSensorsFilePath(stationId));

    if (jsonDoc.isNull() || !jsonDoc.isArray()) {
        return sensors;
    }

    QJsonArray sensorsArray = jsonDoc.array();

    for (const QJsonValue& value : sensorsArray) {
        QJsonObject sensorObj = value.toObject();

        Sensor sensor;
        sensor.id = sensorObj["id"].toInt();
        sensor.stationId = sensorObj["stationId"].toInt();
        sensor.paramName = sensorObj["paramName"].toString();
        sensor.paramFormula = sensorObj["paramFormula"].toString();
        sensor.paramCode = sensorObj["paramCode"].toString();
        sensor.idParam = sensorObj["idParam"].toInt();

        sensors.append(sensor);
    }

    return sensors;
}

void DatabaseHandler::saveMeasurements(int sensorId, const QVector<Measurement>& measurements)
{
    QJsonArray measurementsArray;

    for (const Measurement& measurement : measurements) {
        QJsonObject measurementObj;
        measurementObj["parameterCode"] = measurement.parameterCode;
        measurementObj["date"] = measurement.date.toString(Qt::ISODate);
        measurementObj["value"] = measurement.value;
        measurementObj["isValid"] = measurement.isValid;

        measurementsArray.append(measurementObj);
    }

    QJsonDocument jsonDoc(measurementsArray);

    // Zabezpieczamy dostęp do pliku za pomocą mutexa
    QMutexLocker locker(&mutex);
    saveJsonToFile(getMeasurementsFilePath(sensorId), jsonDoc);
}

QVector<Measurement> DatabaseHandler::loadMeasurements(int sensorId) const
{
    QVector<Measurement> measurements;

    // Zabezpieczamy dostęp do pliku za pomocą mutexa
    QMutexLocker locker(&mutex);
    QJsonDocument jsonDoc = loadJsonFromFile(getMeasurementsFilePath(sensorId));

    if (jsonDoc.isNull() || !jsonDoc.isArray()) {
        return measurements;
    }

    QJsonArray measurementsArray = jsonDoc.array();

    for (const QJsonValue& value : measurementsArray) {
        QJsonObject measurementObj = value.toObject();

        Measurement measurement;
        measurement.parameterCode = measurementObj["parameterCode"].toString();
        measurement.date = QDateTime::fromString(measurementObj["date"].toString(), Qt::ISODate);
        measurement.value = measurementObj["value"].toDouble();
        measurement.isValid = measurementObj["isValid"].toBool();

        measurements.append(measurement);
    }

    return measurements;
}

void DatabaseHandler::initDataDirectory()
{
    // Używamy katalogu AppData/Local dla Windows
    QString dataDirPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    dataDir = QDir(dataDirPath);

    // Jeśli katalog nie istnieje, tworzymy go
    if (!dataDir.exists()) {
        dataDir.mkpath(".");
    }

    qDebug() << "Katalog bazy danych: " << dataDir.absolutePath();
}

QString DatabaseHandler::getStationsFilePath() const
{
    return dataDir.filePath("stations.json");
}

QString DatabaseHandler::getSensorsFilePath(int stationId) const
{
    return dataDir.filePath(QString("sensors_%1.json").arg(stationId));
}

QString DatabaseHandler::getMeasurementsFilePath(int sensorId) const
{
    return dataDir.filePath(QString("measurements_%1.json").arg(sensorId));
}

bool DatabaseHandler::saveJsonToFile(const QString& filePath, const QJsonDocument& jsonDoc) const
{
    try {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            qWarning() << "Nie można otworzyć pliku do zapisu:" << filePath;
            throw std::runtime_error("Nie można otworzyć pliku do zapisu: " + filePath.toStdString());
        }

        file.write(jsonDoc.toJson());
        file.close();

        return true;
    } catch (const std::exception& e) {
        qWarning() << "Błąd podczas zapisu do pliku:" << e.what();
        return false;
    } catch (...) {
        qWarning() << "Nieznany błąd podczas zapisu do pliku";
        return false;
    }
}

QJsonDocument DatabaseHandler::loadJsonFromFile(const QString& filePath) const
{
    try {
        QFile file(filePath);
        if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
            if (!file.exists()) {
                qWarning() << "Plik nie istnieje:" << filePath;
            } else {
                qWarning() << "Nie można otworzyć pliku do odczytu:" << filePath;
            }
            return QJsonDocument();
        }

        QByteArray jsonData = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "Błąd parsowania JSON:" << parseError.errorString();
            throw std::runtime_error("Błąd parsowania JSON: " + parseError.errorString().toStdString());
        }

        return jsonDoc;
    } catch (const std::exception& e) {
        qWarning() << "Błąd podczas odczytu z pliku:" << e.what();
        return QJsonDocument();
    } catch (...) {
        qWarning() << "Nieznany błąd podczas odczytu z pliku";
        return QJsonDocument();
    }
}
