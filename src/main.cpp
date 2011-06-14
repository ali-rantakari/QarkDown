#include <QApplication>

#include "mainwindow.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.resize(640, 512);
    window.show();
    if (argc > 1)
        window.openFile(argv[1]);
    return app.exec();
}
