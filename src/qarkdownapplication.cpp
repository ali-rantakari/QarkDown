#include "qarkdownapplication.h"
#include <QFileOpenEvent>
#include <QDebug>

QarkdownApplication::QarkdownApplication(int &argc, char **argv) :
    QApplication(argc, argv)
{
    mainWindow = NULL;
#ifdef Q_OS_LINUX
    setWindowIcon(QIcon(":/appIcon.png"));
#endif
}

bool QarkdownApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen && mainWindow) {
        mainWindow->openFile(static_cast<QFileOpenEvent *>(event)->file());
        return true;
    }

    return QApplication::event(event);
}
