#include "networkchecker.h"
#include <QDebug>
#include <QEventLoop>

NetworkChecker::NetworkChecker(QObject* parent) : QObject(parent),
    lastNetworkStatus(false)
{
    networkManager = new QNetworkAccessManager(this);
    networkCheckTimer = new QTimer(this);

    connect(networkCheckTimer, &QTimer::timeout, this, &NetworkChecker::checkNetworkStatus);
}

NetworkChecker::~NetworkChecker()
{
    stopMonitoring();
}

bool NetworkChecker::isNetworkAvailable()
{
    QNetworkRequest request(testUrl);
    QNetworkReply* reply = networkManager->get(request);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(3000);

    loop.exec();

    bool available = false;

    if (timer.isActive()) {
        timer.stop();
        if (reply->error() == QNetworkReply::NoError) {
            available = true;
        }
    }

    reply->deleteLater();
    return available;
}

void NetworkChecker::startMonitoring(int intervalMs)
{
    lastNetworkStatus = isNetworkAvailable();
    emit networkStatusChanged(lastNetworkStatus);
    networkCheckTimer->start(intervalMs);
}

void NetworkChecker::stopMonitoring()
{
    networkCheckTimer->stop();
}

void NetworkChecker::checkNetworkStatus()
{
    QNetworkRequest request(testUrl);
    QNetworkReply* reply = networkManager->get(request);

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->start(3000);

    connect(timer, &QTimer::timeout, [reply, timer, this]() {
        reply->abort();

        if (lastNetworkStatus) {
            lastNetworkStatus = false;
            emit networkStatusChanged(false);
            qDebug() << "Sieć niedostępna (timeout)";
        }

        timer->deleteLater();
    });

    connect(reply, &QNetworkReply::finished, this, [reply, timer, this]() {
        timer->stop();
        timer->deleteLater();

        bool currentStatus = (reply->error() == QNetworkReply::NoError);

        if (currentStatus != lastNetworkStatus) {
            lastNetworkStatus = currentStatus;
            emit networkStatusChanged(currentStatus);
            qDebug() << "Status sieci zmieniony na:" << (currentStatus ? "dostępna" : "niedostępna");
        }

        reply->deleteLater();
    });
}

void NetworkChecker::onNetworkReplyFinished(QNetworkReply* reply)
{
    bool currentStatus = (reply->error() == QNetworkReply::NoError);

    if (currentStatus != lastNetworkStatus) {
        lastNetworkStatus = currentStatus;
        emit networkStatusChanged(currentStatus);
    }

    reply->deleteLater();
}
