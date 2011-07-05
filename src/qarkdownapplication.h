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

protected:
    bool event(QEvent *event);

signals:

public slots:

};

#endif // QARKDOWNAPPLICATION_H
