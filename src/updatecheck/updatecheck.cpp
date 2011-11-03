#include "updatecheck.h"
#include <QDebug>
#include <QTextStream>
#include <QDesktopServices>

HGUpdateCheck::HGUpdateCheck(QString baseURL,
                             QSettings *settings,
                             QWidget *parentWidget)
    : QObject(parentWidget)
{
    _baseURL = baseURL;
    _settings = settings;

    _updateInfoDialog = new HGUpdateInfoDialog(parentWidget);
    connect(_updateInfoDialog, SIGNAL(skipThisVersion()),
            this, SLOT(handleSkipThisVersion()));
    connect(_updateInfoDialog, SIGNAL(remindLater()),
            this, SLOT(handleRemindMeLater()));
    connect(_updateInfoDialog, SIGNAL(updateAccepted()),
            this, SLOT(handleUpdateAccepted()));

    _nam = new QNetworkAccessManager(this);
    connect(_nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

HGUpdateCheck::~HGUpdateCheck()
{
    delete _nam;
    delete _updateInfoDialog;
}

#define kSettingKeyLastUpdateTime "LastUpdateCheckTime"
#define kSettingKeyLastSkippedVersion "LastSkippedVersion"

#define kRequestKindAttribute QNetworkRequest::User
#define kRequestKind_UpdateCheck "UpdateCheck"
#define kRequestKind_WhatsChanged "WhatsChanged"

void HGUpdateCheck::checkForUpdatesIfNecessary()
{
    // TODO: check last update time etc.

    checkForUpdatesNow();
}

void HGUpdateCheck::checkForUpdatesNow()
{
    QNetworkRequest request(QUrl(_baseURL + "?versioncheck=y"));
    request.setAttribute(kRequestKindAttribute, kRequestKind_UpdateCheck);
    //qDebug() << "send:" << request.url();
    _nam->get(request);
}

int compareVersionNumbers(QString first, QString second)
{
    QStringList firstParts = first.split('.');
    QStringList secondParts = second.split('.');

    int longerCount = (firstParts.size() < secondParts.size())
                      ? secondParts.size() : firstParts.size();
    for (int i = 0; i < longerCount; i++)
    {
        int firstInt = (i < firstParts.size()) ? firstParts.at(i).toInt() : 0;
        int secondInt = (i < secondParts.size()) ? secondParts.at(i).toInt() : 0;
        if (firstInt < secondInt)
            return 1;
        else if (firstInt > secondInt)
            return -1;
    }
    return 0;
}

void HGUpdateCheck::handleLatestVersionInfo(QString latestVersion)
{
    _latestVersion = latestVersion;

    QString lastSkippedVersion = _settings->value(kSettingKeyLastSkippedVersion).toString();
    if (!lastSkippedVersion.isNull()
        && compareVersionNumbers(lastSkippedVersion,
                                 latestVersion) < 1)
    {
        qDebug() << "user wants to skip this version.";
        return;
    }

    if (compareVersionNumbers(qApp->applicationVersion(),
                              latestVersion) == 1)
    {
        // Update available
        QNetworkRequest request(QUrl(_baseURL + "?whatschanged=y"));
        request.setAttribute(kRequestKindAttribute, kRequestKind_WhatsChanged);
        //qDebug() << "send:" << request.url();
        _nam->get(request);
        return;
    }

    _settings->setValue(kSettingKeyLastUpdateTime, QDate());

    qDebug() << "up to date.";
}

void HGUpdateCheck::handleWhatsChanged(QString whatsChangedHTML)
{
    _updateInfoDialog->presentUpdate(_latestVersion, whatsChangedHTML);
}


void HGUpdateCheck::handleRemindMeLater()
{
    // do nothing.
}
void HGUpdateCheck::handleSkipThisVersion()
{
    _settings->setValue(kSettingKeyLastSkippedVersion, _latestVersion);
}
void HGUpdateCheck::handleUpdateAccepted()
{
    QDesktopServices::openUrl(QUrl(_baseURL));
}


void HGUpdateCheck::replyFinished(QNetworkReply *reply)
{
    QNetworkRequest request = reply->request();
    QVariant requestKind = request.attribute(kRequestKindAttribute);

    // follow redirects:
    QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!redirectUrl.isEmpty())
    {
        redirectUrl = redirectUrl.resolved(redirectUrl);
        QNetworkRequest redirectRequest(redirectUrl);
        redirectRequest.setAttribute(kRequestKindAttribute, requestKind);
        //qDebug() << "redirect:" << redirectRequest.url();
        _nam->get(redirectRequest);
        reply->deleteLater();
        return;
    }

    QTextStream textStream(reply);
    textStream.setCodec("UTF-8");
    QString responseStr = textStream.readAll();

    qDebug() << "got response:" << responseStr;

    if (requestKind == kRequestKind_UpdateCheck)
    {
        handleLatestVersionInfo(responseStr);
    }
    else if (requestKind == kRequestKind_WhatsChanged)
    {
        handleWhatsChanged(responseStr);
    }

    reply->deleteLater();
}
