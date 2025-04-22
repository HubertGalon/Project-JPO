#ifndef NETWORKCHECKER_H
#define NETWORKCHECKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>

class NetworkChecker : public QObject
{
    Q_OBJECT

public:
    explicit NetworkChecker(QObject* parent = nullptr);
    ~NetworkChecker();

    bool isNetworkAvailable();
    void startMonitoring(int intervalMs = 10000);
    void stopMonitoring();

signals:
    void networkStatusChanged(bool available);

private slots:
    void checkNetworkStatus();
    void onNetworkReplyFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* networkManager;
    QTimer* networkCheckTimer;
    bool lastNetworkStatus;
    const QString testUrl = "https://api.gios.gov.pl/pjp-api/rest/station/findAll";
};

#endif // NETWORKCHECKER_H
