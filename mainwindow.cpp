#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QApplication>
#include <QStyle>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Monitor Jakości Powietrza");
    resize(1000, 700);

    // Inicjalizacja menedżera danych
    dataManager = new DataManager(this);

    // Inicjalizacja widgetów
    stationListWidget = new StationListWidget(this);
    sensorListWidget = new SensorListWidget(this);
    chartWidget = new ChartWidget(this);

    // Tworzenie centralnego widgetu z układem kartowym
    centralWidget = new QStackedWidget(this);
    centralWidget->addWidget(stationListWidget);
    centralWidget->addWidget(sensorListWidget);
    centralWidget->addWidget(chartWidget);
    setCentralWidget(centralWidget);

    // Pasek statusu
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    // Widżet postępu
    progressBar = new QProgressBar(this);
    progressBar->setMaximumWidth(150);
    progressBar->setVisible(false);
    statusBar->addPermanentWidget(progressBar);

    // Label statusu
    statusLabel = new QLabel("Gotowy", this);
    statusBar->addWidget(statusLabel);

    // Label trybu pracy (online/offline)
    dataSourceLabel = new QLabel("Tryb: Online", this);
    dataSourceLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    dataSourceLabel->setMinimumWidth(120);
    statusBar->addPermanentWidget(dataSourceLabel);

    // Przycisk do przełączania trybu online/offline
    offlineModeButton = new QPushButton("Tryb Offline", this);
    offlineModeButton->setCheckable(true);
    offlineModeButton->setMaximumWidth(100);
    statusBar->addPermanentWidget(offlineModeButton);

    // Połączenia sygnałów
    connect(dataManager, &DataManager::apiError, this, &MainWindow::handleApiError);
    connect(dataManager, &DataManager::stationsLoaded, this, &MainWindow::onStationsLoaded);
    connect(dataManager, &DataManager::sensorsLoaded, this, &MainWindow::onSensorsLoaded);
    connect(dataManager, &DataManager::measurementsLoaded, this, &MainWindow::onMeasurementsLoaded);

    // Nowe połączenia
    connect(dataManager, &DataManager::networkStatusChanged, this, &MainWindow::onNetworkStatusChanged);
    connect(dataManager, &DataManager::offlineModeChanged, this, &MainWindow::onOfflineModeChanged);
    connect(dataManager, &DataManager::dataSourceChanged, this, &MainWindow::onDataSourceChanged);
    connect(offlineModeButton, &QPushButton::toggled, this, &MainWindow::onOfflineModeButtonToggled);

    connect(stationListWidget, &StationListWidget::stationSelected, this, &MainWindow::onStationSelected);
    connect(sensorListWidget, &SensorListWidget::sensorSelected, this, &MainWindow::onSensorSelected);
    connect(sensorListWidget, &SensorListWidget::backRequested, this, &MainWindow::onBackToStationList);
    connect(chartWidget, &ChartWidget::backRequested, this, &MainWindow::onBackToSensorList);

    // Połączenia dla menu
    connect(ui->actionOdswiezDane, &QAction::triggered, this, &MainWindow::refreshData);
    connect(ui->actionZamknij, &QAction::triggered, this, &QApplication::quit);
    connect(ui->actionOProgramie, &QAction::triggered, this, &MainWindow::showAboutDialog);

    // Dodajemy akcję przełączania trybu offline do menu
    QAction* offlineAction = new QAction("Tryb Offline", this);
    offlineAction->setCheckable(true);
    ui->menuPlik->insertAction(ui->actionOdswiezDane, offlineAction);
    ui->menuPlik->insertSeparator(ui->actionOdswiezDane);

    connect(offlineAction, &QAction::toggled, this, &MainWindow::onOfflineModeActionToggled);
    connect(offlineModeButton, &QPushButton::toggled, offlineAction, &QAction::setChecked);
    connect(offlineAction, &QAction::toggled, offlineModeButton, &QPushButton::setChecked);

    // Początkowe ładowanie stacji
    loadStations();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNetworkStatusChanged(bool available)
{
    if (available) {
        statusLabel->setText("Połączenie internetowe dostępne");
    } else {
        statusLabel->setText("Brak połączenia internetowego");

        // Pokaż komunikat o przejściu w tryb offline jeśli nie mamy połączenia
        QMessageBox::information(this, "Brak połączenia",
                                 "Brak połączenia z internetem. Aplikacja przejdzie w tryb offline i będzie używać zapisanych danych, jeśli są dostępne.");
    }
}

void MainWindow::onOfflineModeChanged(bool offline)
{
    // Aktualizacja UI w zależności od trybu
    offlineModeButton->setChecked(offline);

    if (offline) {
        offlineModeButton->setText("Tryb Online");
        dataSourceLabel->setText("Tryb: Offline");
    } else {
        offlineModeButton->setText("Tryb Offline");
        dataSourceLabel->setText("Tryb: Online");
    }
}

void MainWindow::onDataSourceChanged(const QString& source)
{
    // Aktualizujemy informację o źródle danych
    if (source == "online") {
        dataSourceLabel->setText("Dane: Online");
        dataSourceLabel->setStyleSheet("background-color: #e6ffe6;"); // Jasny zielony
    } else {
        dataSourceLabel->setText("Dane: Offline");
        dataSourceLabel->setStyleSheet("background-color: #fff2e6;"); // Jasny pomarańczowy
    }
}

void MainWindow::onOfflineModeButtonToggled(bool checked)
{
    // Przełączanie trybu offline
    dataManager->setOfflineMode(checked);

    // Jeśli przełączyliśmy się na tryb online, możemy odświeżyć dane
    if (!checked && dataManager->isNetworkAvailable()) {
        refreshData();
    }
}

void MainWindow::onOfflineModeActionToggled(bool checked)
{
    // To samo co powyżej, ale dla akcji menu
    dataManager->setOfflineMode(checked);

    // Jeśli przełączyliśmy się na tryb online, możemy odświeżyć dane
    if (!checked && dataManager->isNetworkAvailable()) {
        refreshData();
    }
}

void MainWindow::handleApiError(const QString& errorMessage)
{
    statusLabel->setText("Błąd");
    progressBar->setVisible(false);

    // Jeśli nie mamy połączenia, pokazujemy bardziej przyjazny komunikat
    if (!dataManager->isNetworkAvailable()) {
        QMessageBox::warning(this, "Brak danych",
                             "Brak połączenia z internetem i brak zapisanych danych dla tego widoku. "
                             "Spróbuj połączyć się z internetem lub wybierz inną stację/sensor, dla której mogą być dostępne dane offline.");
    } else {
        QMessageBox::critical(this, "Błąd API", errorMessage);
    }
}

void MainWindow::loadStations()
{
    statusLabel->setText("Pobieranie stacji...");
    progressBar->setVisible(true);

    dataManager->getAllStations();
}

void MainWindow::onStationsLoaded(const QVector<Station>& stations)
{
    // Aktualizujemy widżet listy stacji
    stationListWidget->setStations(stations);
    statusLabel->setText("Gotowy");
    progressBar->setVisible(false);
}

void MainWindow::onStationSelected(const Station& station)
{
    currentStation = station;
    statusLabel->setText("Pobieranie sensorów...");
    progressBar->setVisible(true);

    // Aktualizujemy tytuł widżetu sensorów
    sensorListWidget->updateStationInfo(station);

    dataManager->getSensors(station.id);
}

void MainWindow::onSensorsLoaded(const QVector<Sensor>& sensors)
{
    sensorListWidget->setSensors(sensors);
    centralWidget->setCurrentWidget(sensorListWidget);
    statusLabel->setText("Gotowy");
    progressBar->setVisible(false);
}

void MainWindow::onSensorSelected(const Sensor& sensor)
{
    currentSensor = sensor;
    statusLabel->setText("Pobieranie danych pomiarowych...");
    progressBar->setVisible(true);

    dataManager->getMeasurements(sensor.id);
}

void MainWindow::onMeasurementsLoaded(const QVector<Measurement>& measurements)
{
    chartWidget->setMeasurements(measurements);
    centralWidget->setCurrentWidget(chartWidget);
    statusLabel->setText("Gotowy");
    progressBar->setVisible(false);
}

void MainWindow::onBackToStationList()
{
    centralWidget->setCurrentWidget(stationListWidget);
}

void MainWindow::onBackToSensorList()
{
    centralWidget->setCurrentWidget(sensorListWidget);
}

void MainWindow::refreshData()
{
    // Określamy, który widżet jest aktualnie widoczny, aby odświeżyć odpowiednie dane
    QWidget* currentWidget = centralWidget->currentWidget();

    if (currentWidget == stationListWidget) {
        loadStations();
    } else if (currentWidget == sensorListWidget && currentStation.id != 0) {
        dataManager->getSensors(currentStation.id);
    } else if (currentWidget == chartWidget && currentSensor.id != 0) {
        dataManager->getMeasurements(currentSensor.id);
    }
}

void MainWindow::showAboutDialog()
{
    QMessageBox::about(this, "O programie",
                       "Monitor Jakości Powietrza v1.0\n\n"
                       "Aplikacja do monitorowania jakości powietrza w Polsce.\n"
                       "Wykorzystuje dane z API Głównego Inspektoratu Ochrony Środowiska.\n\n"
                       "© 2024/2025");
}
