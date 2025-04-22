#ifndef STATIONLISTWIDGET_H
#define STATIONLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QVector>
#include <QProgressDialog>

#include "station.h"
#include "geocoder.h"

namespace Ui {
class StationListWidget;
}

class StationListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StationListWidget(QWidget *parent = nullptr);
    ~StationListWidget();

    void setStations(const QVector<Station>& stations);

signals:
    void stationSelected(const Station& station);

private slots:
    void onStationItemClicked(QListWidgetItem* item);
    void onSearchButtonClicked();
    void onFilterComboChanged(int index);
    void onGeolocationButtonClicked();

    // Nowe sloty dla geocodera
    void onGeocodingFinished(const GeocodingResult& result);
    void onGeocodingError(const QString& errorMessage);

private:
    Ui::StationListWidget *ui;
    QVector<Station> stations;
    Geocoder* geocoder;
    QProgressDialog* progressDialog;
    double searchRadiusKm; // Promień wyszukiwania w km

    // Wypełnianie listy stacji
    void populateStationList(const QVector<Station>& stationsToShow);

    // Metody wyszukiwania
    void searchByCity();
    void searchByAddress(const QString& address);

    // Dane aktualnie wyświetlanych stacji
    QVector<Station> currentDisplayedStations;
};

#endif // STATIONLISTWIDGET_H
