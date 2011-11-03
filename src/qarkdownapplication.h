#ifndef QARKDOWNAPPLICATION_H
#define QARKDOWNAPPLICATION_H

#include <QApplication>
#include "mainwindow.h"

class QarkdownApplication : public QApplication
{
    Q_OBJECT
public:
    explicit QarkdownApplication(int &argc, char **argv);
    MainWindow *mainWindow;

    QString applicationStoragePath();
    bool copyResourceToFile(QString resourcePath, QString targetFilePath);

    void commitData(QSessionManager &manager);
    void saveState(QSessionManager &manager);

protected:
    bool event(QEvent *event);

signals:

public slots:

};

#endif // QARKDOWNAPPLICATION_H
