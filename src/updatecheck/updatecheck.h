#ifndef UPDATECHECK_H
#define UPDATECHECK_H

#include <QObject>
#include <QSettings>
#include <QtNetwork/QtNetwork>
#include "hgupdateinfodialog.h"

class HGUpdateCheck : public QObject
{
    Q_OBJECT
public:
    HGUpdateCheck(QString baseURL, QSettings *settings,
                  QWidget *parentWidget);
    ~HGUpdateCheck();

    void checkForUpdatesIfNecessary();
    void checkForUpdatesNow();

private:
    QString _baseURL;
    QSettings *_settings;
    QNetworkAccessManager *_nam;
    QString _latestVersion;

    HGUpdateInfoDialog *_updateInfoDialog;

    void handleLatestVersionInfo(QString latestVersion);
    void handleWhatsChanged(QString whatsChangedHTML);

private slots:
    void replyFinished(QNetworkReply*);
    void handleRemindMeLater();
    void handleSkipThisVersion();
    void handleUpdateAccepted();
};

#endif
