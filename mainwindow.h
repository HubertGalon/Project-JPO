#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QProgressBar>
#include <QComboBox>
#include <QPushButton>
#include <QStatusBar>

#include "datamanager.h"
#include "stationlistwidget.h"
#include "sensorlistwidget.h"
#include "chartwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleApiError(const QString& errorMessage);
    void loadStations();
    void onStationsLoaded(const QVector<Station>& stations);
    void onSensorsLoaded(const QVector<Sensor>& sensors);
    void onMeasurementsLoaded(const QVector<Measurement>& measurements);

    void onStationSelected(const Station& station);
    void onSensorSelected(const Sensor& sensor);
    void onBackToStationList();
    void onBackToSensorList();

    // Akcje menu
    void refreshData();
    void showAboutDialog();

    // Nowe sloty dla obsługi trybu offline
    void onNetworkStatusChanged(bool available);
    void onOfflineModeChanged(bool offline);
    void onDataSourceChanged(const QString& source);
    void onOfflineModeButtonToggled(bool checked);
    void onOfflineModeActionToggled(bool checked);

private:
    Ui::MainWindow *ui;
    DataManager *dataManager;
    QStackedWidget *centralWidget;
    StationListWidget *stationListWidget;
    SensorListWidget *sensorListWidget;
    ChartWidget *chartWidget;
    QStatusBar *statusBar;
    QProgressBar *progressBar;
    QLabel *statusLabel;

    // Nowe elementy UI
    QLabel *dataSourceLabel;
    QPushButton *offlineModeButton;

    // Zapamiętujemy aktualnie wybrane obiekty
    Station currentStation;
    Sensor currentSensor;
};

#endif // MAINWINDOW_H
