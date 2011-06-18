#include <QApplication>

#include "mainwindow.h"
#include "qarkdownapplication.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QarkdownApplication app(argc, argv);
    MainWindow window;
    window.resize(640, 512);
    window.show();
    app.mainWindow = &window;
    if (argc > 1)
        window.openFile(argv[1]);
    return app.exec();
}
