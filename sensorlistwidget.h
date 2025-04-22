#ifndef SENSORLISTWIDGET_H
#define SENSORLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVector>

#include "sensor.h"
#include "station.h"

namespace Ui {
class SensorListWidget;
}

class SensorListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SensorListWidget(QWidget *parent = nullptr);
    ~SensorListWidget();

    void setSensors(const QVector<Sensor>& sensors);
    void updateStationInfo(const Station& station);

signals:
    void sensorSelected(const Sensor& sensor);
    void backRequested();

private slots:
    void onSensorItemClicked(QListWidgetItem* item);
    void onBackButtonClicked();

private:
    Ui::SensorListWidget *ui;
    QVector<Sensor> sensors;
    Station currentStation;

    // Wypełnianie listy sensorów
    void populateSensorList();
};

#endif // SENSORLISTWIDGET_H
