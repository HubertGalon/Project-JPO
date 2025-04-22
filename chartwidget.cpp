#include "chartwidget.h"
#include "ui_chartwidget.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ChartWidget::ChartWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartWidget)
{
    ui->setupUi(this);

    // Inicjalizacja analizatora danych
    dataAnalyzer = new DataAnalyzer(this);

    // Inicjalizacja wykresu
    chart = new QChart();
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Dodajemy wykres do layoutu
    ui->chartLayout->addWidget(chartView);

    // Inicjalizacja ComboBox z przedziałami czasowymi
    ui->timeRangeComboBox->addItem("Ostatnie 24 godziny");
    ui->timeRangeComboBox->addItem("Ostatnie 3 dni");
    ui->timeRangeComboBox->addItem("Ostatni tydzień");
    ui->timeRangeComboBox->addItem("Ostatni miesiąc");
    ui->timeRangeComboBox->addItem("Niestandardowy okres");

    // Ukrywamy pola dat niestandardowych domyślnie
    ui->startDateEdit->setVisible(false);
    ui->endDateEdit->setVisible(false);
    ui->startDateLabel->setVisible(false);
    ui->endDateLabel->setVisible(false);

    // Połączenia sygnałów
    connect(ui->backButton, &QPushButton::clicked, this, &ChartWidget::onBackButtonClicked);
    connect(ui->timeRangeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ChartWidget::onTimeRangeChanged);
    connect(ui->startDateEdit, &QDateEdit::dateChanged, this, &ChartWidget::onDateRangeChanged);
    connect(ui->endDateEdit, &QDateEdit::dateChanged, this, &ChartWidget::onDateRangeChanged);
    connect(ui->analyzeButton, &QPushButton::clicked, this, &ChartWidget::onAnalyzeButtonClicked);
    connect(ui->saveChartButton, &QPushButton::clicked, this, &ChartWidget::onSaveChartButtonClicked);
    connect(ui->exportDataButton, &QPushButton::clicked, this, &ChartWidget::onExportDataButtonClicked);
}

ChartWidget::~ChartWidget()
{
    delete ui;
}

void ChartWidget::setMeasurements(const QVector<Measurement> &measurements)
{
    this->measurements = measurements;

    if (measurements.isEmpty()) {
        QMessageBox::warning(this, "Brak danych", "Nie ma dostępnych danych pomiarowych dla wybranego sensora.");
        return;
    }

    // Ustawiamy zakres dat dla kontrolek
    QDateTime latestDate = measurements.first().date;
    QDateTime earliestDate = measurements.first().date;

    for (const Measurement& m : measurements) {
        if (m.date > latestDate) {
            latestDate = m.date;
        }
        if (m.date < earliestDate) {
            earliestDate = m.date;
        }
    }

    ui->startDateEdit->setDateRange(earliestDate.date(), latestDate.date());
    ui->endDateEdit->setDateRange(earliestDate.date(), latestDate.date());

    ui->startDateEdit->setDate(earliestDate.date());
    ui->endDateEdit->setDate(latestDate.date());

    // Wyświetlamy dane dla ostatnich 24 godzin domyślnie
    ui->timeRangeComboBox->setCurrentIndex(0);

    // Aktualizujemy wykres
    updateChart(filterMeasurementsByTimeRange(0));

    // Ustawiamy nazwę parametru
    if (!measurements.isEmpty()) {
        ui->parameterLabel->setText(QString("Parametr: %1").arg(measurements.first().parameterCode));
    }
}

void ChartWidget::onBackButtonClicked()
{
    emit backRequested();
}

void ChartWidget::onTimeRangeChanged(int index)
{
    // Pokazujemy/ukrywamy kontrolki dat niestandardowych
    bool showCustomDates = (index == 4); // Niestandardowy okres
    ui->startDateEdit->setVisible(showCustomDates);
    ui->endDateEdit->setVisible(showCustomDates);
    ui->startDateLabel->setVisible(showCustomDates);
    ui->endDateLabel->setVisible(showCustomDates);

    // Aktualizujemy wykres
    updateChart(filterMeasurementsByTimeRange(index));
}

void ChartWidget::onDateRangeChanged()
{
    if (ui->timeRangeComboBox->currentIndex() == 4) { // Niestandardowy okres
        QDateTime startDate = QDateTime(ui->startDateEdit->date(), QTime(0, 0));
        QDateTime endDate = QDateTime(ui->endDateEdit->date(), QTime(23, 59, 59));

        QVector<Measurement> filteredMeasurements = dataAnalyzer->filterMeasurementsByDateRange(
            measurements, startDate, endDate);

        updateChart(filteredMeasurements);
    }
}

void ChartWidget::onAnalyzeButtonClicked()
{
    QVector<Measurement> currentMeasurements = filterMeasurementsByTimeRange(ui->timeRangeComboBox->currentIndex());

    if (currentMeasurements.isEmpty()) {
        QMessageBox::warning(this, "Brak danych", "Nie ma danych do analizy w wybranym okresie.");
        return;
    }

    AnalysisResult result = dataAnalyzer->analyzeMeasurements(currentMeasurements);
    displayAnalysisResults(result);
}

void ChartWidget::onSaveChartButtonClicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Zapisz wykres", "", "Obrazy PNG (*.png);;Wszystkie pliki (*)");

    if (!filePath.isEmpty()) {
        QPixmap pixmap = chartView->grab();
        pixmap.save(filePath);

        QMessageBox::information(this, "Sukces", "Wykres został zapisany.");
    }
}

void ChartWidget::onExportDataButtonClicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Eksportuj dane", "", "Pliki JSON (*.json);;Wszystkie pliki (*)");

    if (!filePath.isEmpty()) {
        // Tworzymy strukturę JSON
        QJsonArray dataArray;
        QVector<Measurement> currentMeasurements = filterMeasurementsByTimeRange(ui->timeRangeComboBox->currentIndex());

        for (const Measurement& m : currentMeasurements) {
            QJsonObject measurementObj;
            measurementObj["date"] = m.date.toString("yyyy-MM-dd HH:mm:ss");
            measurementObj["value"] = m.value;
            measurementObj["parameterCode"] = m.parameterCode;
            dataArray.append(measurementObj);
        }

        QJsonObject rootObject;
        rootObject["parameterCode"] = currentMeasurements.isEmpty() ? "" : currentMeasurements.first().parameterCode;
        rootObject["timeRange"] = ui->timeRangeComboBox->currentText();
        rootObject["exportDate"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        rootObject["measurements"] = dataArray;

        // Tworzymy dokument JSON
        QJsonDocument jsonDoc(rootObject);

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            // Zapisujemy JSON do pliku
            file.write(jsonDoc.toJson(QJsonDocument::Indented)); // Format z wcięciami dla czytelności
            file.close();
            QMessageBox::information(this, "Sukces", "Dane zostały wyeksportowane do pliku JSON.");
        } else {
            QMessageBox::critical(this, "Błąd", "Nie można otworzyć pliku do zapisu.");
        }
    }
}

void ChartWidget::createChart()
{
    chart->removeAllSeries();

    // Usuwamy istniejące osie
    QList<QAbstractAxis*> axes = chart->axes();
    for (QAbstractAxis* axis : axes) {
        chart->removeAxis(axis);
        delete axis;
    }

    // Osie wykresu
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("dd.MM HH:mm");
    axisX->setTitleText("Data i czas");
    axisX->setGridLineVisible(true);
    axisX->setMinorGridLineVisible(false);
    axisX->setLabelsAngle(-45); // Pochylenie etykiet dla lepszej czytelności

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%.2f");
    axisY->setTitleText("Wartość");
    axisY->setGridLineVisible(true);
    axisY->setMinorGridLineVisible(false);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    // Ustawiamy właściwości wykresu
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);
    chart->setBackgroundVisible(false);
}

void ChartWidget::updateChart(const QVector<Measurement> &measurementsToShow)
{
    if (measurementsToShow.isEmpty()) {
        chart->setTitle("Brak danych w wybranym okresie");
        return;
    }

    createChart();

    QLineSeries *series = new QLineSeries();

    // Znajdujemy zakres dat
    QDateTime minDate = measurementsToShow.first().date;
    QDateTime maxDate = measurementsToShow.first().date;

    // Dodajemy punkty do serii
    for (const Measurement& m : measurementsToShow) {
        series->append(m.date.toMSecsSinceEpoch(), m.value);

        // Aktualizujemy zakres dat
        if (m.date < minDate) minDate = m.date;
        if (m.date > maxDate) maxDate = m.date;
    }

    chart->addSeries(series);

    // Podłączamy serie do osi
    series->attachAxis(chart->axes(Qt::Horizontal).first());
    series->attachAxis(chart->axes(Qt::Vertical).first());

    // Ustawiamy tytuł wykresu
    QString parameterCode = measurementsToShow.first().parameterCode;
    chart->setTitle(QString("Pomiary parametru %1").arg(parameterCode));

    // Aktualizujemy zakres osi Y
    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    if (axisY) {
        double minValue = std::numeric_limits<double>::max();
        double maxValue = std::numeric_limits<double>::lowest();

        for (const Measurement& m : measurementsToShow) {
            minValue = qMin(minValue, m.value);
            maxValue = qMax(maxValue, m.value);
        }

        // Dodajemy 10% marginesu
        double range = maxValue - minValue;
        if (range <= 0) range = 1.0;

        axisY->setRange(minValue - 0.1 * range, maxValue + 0.1 * range);
    }

    // Aktualizujemy zakres osi X
    QDateTimeAxis *axisX = qobject_cast<QDateTimeAxis*>(chart->axes(Qt::Horizontal).first());
    if (axisX) {
        // Ustawiamy dokładny zakres dat bez dodatkowych marginesów
        axisX->setRange(minDate, maxDate);

        // Dostosowujemy format daty w zależności od zakresu czasu
        qint64 rangeDays = minDate.daysTo(maxDate);
        if (rangeDays <= 1) {
            axisX->setFormat("HH:mm"); // Tylko godziny i minuty dla zakresów do 1 dnia
        } else if (rangeDays <= 7) {
            axisX->setFormat("dd.MM HH:mm"); // Dzień, miesiąc, godzina dla zakresów do tygodnia
        } else {
            axisX->setFormat("yyyy-MM-dd"); // Pełna data dla dłuższych okresów
        }

        // Dostosowujemy liczbę ticków w zależności od zakresu
        if (rangeDays <= 1) {
            axisX->setTickCount(6); // Mniej ticków dla krótszych okresów
        } else if (rangeDays <= 7) {
            axisX->setTickCount(8);
        } else {
            axisX->setTickCount(10);
        }
    }
}

QVector<Measurement> ChartWidget::filterMeasurementsByTimeRange(int timeRangeIndex)
{
    // Jeśli nie mamy pomiarów, zwracamy pustą listę
    if (measurements.isEmpty()) {
        return QVector<Measurement>();
    }

    QDateTime now = QDateTime::currentDateTime();
    QDateTime startDate;
    QDateTime endDate = now;

    // Znajdź najnowszy dostępny pomiar (może być starszy niż aktualny czas)
    QDateTime latestMeasurementDate = measurements.first().date;
    for (const Measurement& m : measurements) {
        if (m.date > latestMeasurementDate) {
            latestMeasurementDate = m.date;
        }
    }

    // Używamy najnowszej daty pomiaru jako punktu odniesienia
    switch (timeRangeIndex) {
    case 0: // Ostatnie 24 godziny
        startDate = latestMeasurementDate.addDays(-1);
        endDate = latestMeasurementDate;
        break;
    case 1: // Ostatnie 3 dni
        startDate = latestMeasurementDate.addDays(-3);
        endDate = latestMeasurementDate;
        break;
    case 2: // Ostatni tydzień
        startDate = latestMeasurementDate.addDays(-7);
        endDate = latestMeasurementDate;
        break;
    case 3: // Ostatni miesiąc
        startDate = latestMeasurementDate.addMonths(-1);
        endDate = latestMeasurementDate;
        break;
    case 4: // Niestandardowy okres
        startDate = QDateTime(ui->startDateEdit->date(), QTime(0, 0));
        endDate = QDateTime(ui->endDateEdit->date(), QTime(23, 59, 59));
        break;
    default:
        startDate = latestMeasurementDate.addDays(-1);
        endDate = latestMeasurementDate;
        break;
    }

    return dataAnalyzer->filterMeasurementsByDateRange(measurements, startDate, endDate);
}

void ChartWidget::displayAnalysisResults(const AnalysisResult &result)
{
    QString resultText = "Wyniki analizy:\n\n";
    resultText += QString("Wartość minimalna: %1 (dnia %2)\n").arg(result.minValue).arg(result.minDate.toString("yyyy-MM-dd HH:mm"));
    resultText += QString("Wartość maksymalna: %1 (dnia %2)\n").arg(result.maxValue).arg(result.maxDate.toString("yyyy-MM-dd HH:mm"));
    resultText += QString("Wartość średnia: %1\n").arg(result.avgValue);
    resultText += QString("Trend: %1 (%2)").arg(result.trend).arg(result.trendDescription);

    QMessageBox::information(this, "Analiza danych", resultText);
}
