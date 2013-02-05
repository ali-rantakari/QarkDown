#ifndef QARKDOWNAPPLICATION_H
#define QARKDOWNAPPLICATION_H

#include <QtWidgets/QApplication>
#include "mainwindow.h"

class QarkdownApplication : public QApplication
{
    Q_OBJECT
public:
    explicit QarkdownApplication(int &argc, char **argv);
    ~QarkdownApplication();

    MainWindow *mainWindow;

    QString copyrightYear();
    QString websiteURL();
    QString applicationStoragePath();
    bool copyResourceToFile(QString resourcePath, QString targetFilePath);

protected:
    bool event(QEvent *event);

signals:

public slots:

};

#endif // QARKDOWNAPPLICATION_H
