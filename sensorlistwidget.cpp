#include "sensorlistwidget.h"
#include "ui_sensorlistwidget.h"

SensorListWidget::SensorListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SensorListWidget)
{
    ui->setupUi(this);

    // Połączenia sygnałów
    connect(ui->sensorListWidget, &QListWidget::itemClicked, this, &SensorListWidget::onSensorItemClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &SensorListWidget::onBackButtonClicked);
}

SensorListWidget::~SensorListWidget()
{
    delete ui;
}

void SensorListWidget::setSensors(const QVector<Sensor> &sensors)
{
    this->sensors = sensors;
    populateSensorList();

    ui->sensorCountLabel->setText(QString("Dostępne czujniki: %1").arg(sensors.size()));
}

void SensorListWidget::updateStationInfo(const Station& station)
{
    currentStation = station;
    ui->stationNameLabel->setText(station.stationName);
}

void SensorListWidget::onSensorItemClicked(QListWidgetItem *item)
{
    int row = ui->sensorListWidget->row(item);

    if (row >= 0 && row < sensors.size()) {
        emit sensorSelected(sensors[row]);
    }
}

void SensorListWidget::onBackButtonClicked()
{
    emit backRequested();
}

void SensorListWidget::populateSensorList()
{
    ui->sensorListWidget->clear();

    for (const Sensor& sensor : sensors) {
        QListWidgetItem* item = new QListWidgetItem(ui->sensorListWidget);

        QString displayText = QString("%1 (%2)").arg(sensor.paramName, sensor.paramFormula);

        item->setText(displayText);
        item->setToolTip(displayText);

        ui->sensorListWidget->addItem(item);
    }
}
