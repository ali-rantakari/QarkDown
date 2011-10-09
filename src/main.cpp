#include <QApplication>

#include "mainwindow.h"
#include "qarkdownapplication.h"
#include "logger.h"

int main(int argc, char *argv[])
{
    QarkdownApplication app(argc, argv);

    for (int i = 0; i < argc; i++)
    {
        if (strcmp("-d", argv[i]) == 0)
            Logger::setAllLevelsEnabled(true);
    }

    MainWindow window;
    window.show();
    app.mainWindow = &window;
    if (argc > 1)
        window.openFile(argv[1]);
    return app.exec();
}
