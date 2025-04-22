#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Ustawienie informacji o aplikacji
    QCoreApplication::setOrganizationName("AirQualityMonitor");
    QCoreApplication::setApplicationName("Monitor Jakości Powietrza");
    QCoreApplication::setApplicationVersion("1.0");

    // Ustawienie stylu aplikacji
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // Utworzenie i wyświetlenie głównego okna
    MainWindow w;
    w.show();

    return a.exec();
}
