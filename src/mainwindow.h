#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QSessionManager>

#include "highlighter.h"
#include "preferencesdialog.h"
#include "qarkdowntextedit.h"

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void newFile();
    void openFile(const QString &path = QString());
    void saveFile();

    void increaseFontSize();
    void decreaseFontSize();
    void showPreferences();

    void preferencesUpdated();

    void commitDataHandler(QSessionManager &manager);
    void aboutToQuitHandler();
    void handleContentsChange(int position, int charsRemoved, int charsAdded);

private:
    void setupEditor();
    void setupFileMenu();
    void performStartupTasks();
    void persistFontInfo();
    void applyPersistedFontInfo();
    void applyHighlighterPreferences();
    void setDirty(bool value);

    PreferencesDialog *preferencesDialog;
    QSettings *settings;
    QarkdownTextEdit *editor;
    HGMarkdownHighlighter *highlighter;
    QString openFilePath;
};

#endif
