#include "stationlistwidget.h"
#include "ui_stationlistwidget.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QHBoxLayout>

StationListWidget::StationListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StationListWidget),
    searchRadiusKm(10.0) // Domyślny promień wyszukiwania: 10km
{
    ui->setupUi(this);

    // Inicjalizacja geocodera
    geocoder = new Geocoder(this);

    // Inicjalizacja dialogu postępu
    progressDialog = new QProgressDialog("Trwa wyszukiwanie adresu...", "Anuluj", 0, 0, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(500); // Pokaż dialog po 500ms

    // Połączenia sygnałów
    connect(ui->stationListWidget, &QListWidget::itemClicked, this, &StationListWidget::onStationItemClicked);
    connect(ui->searchButton, &QPushButton::clicked, this, &StationListWidget::onSearchButtonClicked);
    connect(ui->filterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StationListWidget::onFilterComboChanged);
    connect(ui->geolocateButton, &QPushButton::clicked, this, &StationListWidget::onGeolocationButtonClicked);

    // Połączenia geocodera
    connect(geocoder, &Geocoder::geocodingFinished, this, &StationListWidget::onGeocodingFinished);
    connect(geocoder, &Geocoder::geocodingError, this, &StationListWidget::onGeocodingError);
    connect(progressDialog, &QProgressDialog::canceled, [this]() {
        // Tutaj można dodać kod do anulowania geocodowania
    });

    // Inicjalizacja ComboBox
    ui->filterComboBox->addItem("Wszystkie stacje");
    ui->filterComboBox->addItem("Wyszukaj po mieście");
    ui->filterComboBox->addItem("Wyszukaj po adresie");

    // Dodajemy kontrolki do wyboru promienia wyszukiwania
    QLabel* radiusLabel = new QLabel("Promień wyszukiwania:", this);
    QComboBox* radiusComboBox = new QComboBox(this);
    radiusComboBox->addItem("5 km", 5.0);
    radiusComboBox->addItem("10 km", 10.0);
    radiusComboBox->addItem("20 km", 20.0);
    radiusComboBox->addItem("50 km", 50.0);
    radiusComboBox->setCurrentIndex(1); // 10 km domyślnie

    // Podłączenie zdarzenia zmiany promienia
    connect(radiusComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, radiusComboBox](int index) {
        searchRadiusKm = radiusComboBox->itemData(index).toDouble();
    });

    // Dodajemy kontrolki do layoutu
    QHBoxLayout* radiusLayout = new QHBoxLayout();
    radiusLayout->addWidget(radiusLabel);
    radiusLayout->addWidget(radiusComboBox);
    radiusLayout->addStretch();

    // Wstawiamy layout do głównego layoutu
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (mainLayout) {
        mainLayout->insertLayout(2, radiusLayout); // Wstawiamy po kontrolkach wyszukiwania
    }
}

StationListWidget::~StationListWidget()
{
    delete ui;
}

void StationListWidget::setStations(const QVector<Station> &stations)
{
    this->stations = stations;
    populateStationList(stations);

    ui->totalStationsLabel->setText(QString("Łączna liczba stacji: %1").arg(stations.size()));
}

void StationListWidget::onStationItemClicked(QListWidgetItem *item)
{
    int row = ui->stationListWidget->row(item);

    if (row >= 0 && row < currentDisplayedStations.size()) {
        emit stationSelected(currentDisplayedStations[row]);
    }
}

void StationListWidget::onSearchButtonClicked()
{
    int filterIndex = ui->filterComboBox->currentIndex();

    switch (filterIndex) {
    case 1: // Wyszukaj po mieście
        searchByCity();
        break;
    case 2: // Wyszukaj po adresie
        searchByAddress(ui->searchLineEdit->text().trimmed());
        break;
    default: // Wszystkie stacje
        populateStationList(stations);
        break;
    }
}

void StationListWidget::onFilterComboChanged(int index)
{
    ui->searchLineEdit->clear();

    if (index == 0) { // Wszystkie stacje
        ui->searchLineEdit->setPlaceholderText("");
        ui->searchLineEdit->setEnabled(false);
        ui->searchButton->setEnabled(false);
        ui->geolocateButton->setVisible(false);

        // Pokazujemy wszystkie stacje
        populateStationList(stations);
    } else if (index == 1) { // Wyszukaj po mieście
        ui->searchLineEdit->setPlaceholderText("Wprowadź nazwę miasta...");
        ui->searchLineEdit->setEnabled(true);
        ui->searchButton->setEnabled(true);
        ui->geolocateButton->setVisible(false);
    } else if (index == 2) { // Wyszukaj po adresie
        ui->searchLineEdit->setPlaceholderText("Wprowadź adres (np. Polanka 3, Poznań)...");
        ui->searchLineEdit->setEnabled(true);
        ui->searchButton->setEnabled(true);
        ui->geolocateButton->setVisible(false);
    }
}

void StationListWidget::onGeolocationButtonClicked()
{
    QString address = ui->searchLineEdit->text().trimmed();

    if (address.isEmpty()) {
        QMessageBox::warning(this, "Uwaga", "Wprowadź adres do geolokalizacji");
        return;
    }

    searchByAddress(address);
}

void StationListWidget::onGeocodingFinished(const GeocodingResult& result)
{
    progressDialog->hide();

    if (!result.isValid) {
        QMessageBox::warning(this, "Błąd geokodowania", "Nie udało się zlokalizować podanego adresu.");
        return;
    }

    qDebug() << "=== WYNIK GEOKODOWANIA ===";
    qDebug() << "Adres: " << result.formattedAddress;
    qDebug() << "Współrzędne: " << result.latitude << ", " << result.longitude;
    qDebug() << "Promień wyszukiwania: " << searchRadiusKm << " km";
    qDebug() << "===========================";

    // Filtrujemy stacje według odległości
    QVector<QPair<Station, double>> stationsWithDistance;

    qDebug() << "Rozpoczynam filtrowanie stacji...";
    qDebug() << "Liczba wszystkich stacji: " << stations.size();

    int stationsInRadius = 0;

    for (const Station& station : stations) {
        double distance = station.calculateDistance(result.latitude, result.longitude);

        // Sprawdzamy współrzędne stacji
        qDebug() << "Stacja: " << station.stationName << " (" << station.gegrLat << ", " << station.gegrLon << ")";

        if (distance <= searchRadiusKm) {
            stationsWithDistance.append(qMakePair(station, distance));
            stationsInRadius++;
        }
    }

    qDebug() << "Liczba stacji w promieniu " << searchRadiusKm << " km: " << stationsInRadius;

    // Sortowanie stacji według odległości
    std::sort(stationsWithDistance.begin(), stationsWithDistance.end(),
              [](const QPair<Station, double>& a, const QPair<Station, double>& b) {
                  return a.second < b.second;
              });

    // Wypełniamy listę stacji z informacją o odległości
    ui->stationListWidget->clear();
    currentDisplayedStations.clear();

    for (const auto& pair : stationsWithDistance) {
        const Station& station = pair.first;
        double distance = pair.second;

        QListWidgetItem* item = new QListWidgetItem(ui->stationListWidget);

        QString displayText = QString("%1\n%2\nOdległość: %3 km")
                                  .arg(station.stationName)
                                  .arg(station.getFullAddress())
                                  .arg(distance, 0, 'f', 2);

        item->setText(displayText);
        item->setToolTip(displayText);

        ui->stationListWidget->addItem(item);
        currentDisplayedStations.append(station);
    }

    if (stationsWithDistance.isEmpty()) {
        QMessageBox::information(this, "Brak stacji",
                                 QString("Nie znaleziono stacji pomiarowych w promieniu %1 km od podanego adresu. "
                                         "Spróbuj zwiększyć promień wyszukiwania.").arg(searchRadiusKm));
    }

    ui->totalStationsLabel->setText(QString("Znaleziono %1 stacji w promieniu %2 km od %3")
                                        .arg(stationsWithDistance.size())
                                        .arg(searchRadiusKm)
                                        .arg(result.formattedAddress));
}

void StationListWidget::onGeocodingError(const QString& errorMessage)
{
    progressDialog->hide();
    QMessageBox::warning(this, "Błąd geokodowania", "Wystąpił błąd podczas lokalizacji adresu: " + errorMessage);
}

void StationListWidget::populateStationList(const QVector<Station> &stationsToShow)
{
    ui->stationListWidget->clear();
    currentDisplayedStations = stationsToShow;

    for (const Station& station : stationsToShow) {
        QListWidgetItem* item = new QListWidgetItem(ui->stationListWidget);

        QString displayText = QString("%1\n%2").arg(station.stationName, station.getFullAddress());

        item->setText(displayText);
        item->setToolTip(displayText);

        ui->stationListWidget->addItem(item);
    }
}

void StationListWidget::searchByCity()
{
    QString cityName = ui->searchLineEdit->text().trimmed();

    if (cityName.isEmpty()) {
        QMessageBox::warning(this, "Uwaga", "Wprowadź nazwę miasta");
        return;
    }

    QVector<Station> filteredStations;

    for (const Station& station : stations) {
        if (station.cityName.toLower().contains(cityName.toLower())) {
            filteredStations.append(station);
        }
    }

    populateStationList(filteredStations);

    ui->totalStationsLabel->setText(QString("Znaleziono %1 stacji w mieście %2").arg(filteredStations.size()).arg(cityName));
}

void StationListWidget::searchByAddress(const QString& address)
{
    if (address.isEmpty()) {
        QMessageBox::warning(this, "Uwaga", "Wprowadź adres");
        return;
    }

    // Pokazujemy dialog postępu
    progressDialog->setLabelText(QString("Wyszukiwanie adresu: %1...").arg(address));
    progressDialog->show();

    // Wywołujemy geokodowanie
    geocoder->geocodeAddress(address);
}
