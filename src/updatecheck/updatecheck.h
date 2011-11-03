#ifndef UPDATECHECK_H
#define UPDATECHECK_H

#include <QObject>
#include <QSettings>
#include <QProgressDialog>
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
    void checkForUpdatesNow(bool userInitiated = true);

private:
    QProgressDialog *_progressDialog;
    QString _baseURL;
    QSettings *_settings;
    QNetworkAccessManager *_nam;
    QString _latestVersion;
    QNetworkReply *_activeReply;
    bool _canceled;

    HGUpdateInfoDialog *_updateInfoDialog;

    void handleLatestVersionInfo(QString latestVersion, bool userInitiated);
    void handleWhatsChanged(QString whatsChangedHTML);

private slots:
    void replyFinished(QNetworkReply*);
    void authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

    void canceledFromProgressDialog();
    void handleRemindMeLater();
    void handleSkipThisVersion();
    void handleUpdateAccepted();
    void handleError(QString errorMessage, bool userInitiated);
};

#endif
