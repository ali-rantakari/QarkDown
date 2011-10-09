#include "qarkdownapplication.h"
#include "logger.h"
#include <QFileOpenEvent>
#include <QDesktopServices>

struct applicationVersion
{
    int major;
    int minor;
    int tiny;
} appVersion = {0, 1, 2};

QarkdownApplication::QarkdownApplication(int &argc, char **argv) :
    QApplication(argc, argv)
{
    mainWindow = NULL;
#ifdef Q_OS_LINUX
    setWindowIcon(QIcon(":/appIcon.png"));
#endif
    QCoreApplication::setApplicationName("QarkDown");
    QCoreApplication::setApplicationVersion(QString().sprintf("%i.%i.%i", appVersion.major, appVersion.minor, appVersion.tiny));
}

QString QarkdownApplication::applicationStoragePath()
{
    QString appName = QCoreApplication::applicationName();
    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    // DataLocation should be defined on all but embedded platforms but just
    // to be safe we do this:
    if (path.isEmpty())
        path = QDir::homePath() + "/." + appName;

    // Let's make sure the path exists
    if (!QFile::exists(path)) {
        QDir dir;
        dir.mkpath(path);
    }

    return path;
}

bool QarkdownApplication::copyResourceToFile(QString resourcePath, QString targetFilePath)
{
    QFile source(resourcePath);
    if (!source.open(QIODevice::ReadOnly)) {
        Logger::warning("Cannot open file for reading: " + source.fileName());
        return false;
    }
    QByteArray contents = source.readAll();
    source.close();

    QFile target(targetFilePath);
    if (!target.open(QIODevice::WriteOnly)) {
        Logger::warning("Cannot open file for writing: " + target.fileName());
        return false;
    }
    target.write(contents);
    target.close();

    return true;
}

bool QarkdownApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen && mainWindow) {
        mainWindow->openFile(static_cast<QFileOpenEvent *>(event)->file());
        return true;
    }

    return QApplication::event(event);
}
