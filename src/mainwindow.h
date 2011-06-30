#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QSessionManager>

#include "peg-markdown-highlight/highlighter.h"
#include "preferencesdialog.h"
#include "editor/qarkdowntextedit.h"

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void show();

public slots:
    void newFile();
    void openFile(const QString &path = QString());
    void saveFile();

    void increaseFontSize();
    void decreaseFontSize();
    void showPreferences();
    void about();

    void selectTextToSearchFor();
    void findNextSearchMatch();
    void findPreviousSearchMatch();

    void preferencesUpdated();

    void anchorClicked(const QUrl &link);
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
    void applyEditorPreferences();
    void setDirty(bool value);

    PreferencesDialog *preferencesDialog;
    QSettings *settings;
    QarkdownTextEdit *editor;
    HGMarkdownHighlighter *highlighter;
    QString openFilePath;
    QString searchString;

    QAction *findNextMenuAction;
    QAction *findPreviousMenuAction;
};

#endif
