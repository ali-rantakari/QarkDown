#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QSessionManager>
#include <QStringList>

#include "peg-markdown-highlight/highlighter.h"
#include "preferencesdialog.h"
#include "editor/qarkdowntextedit.h"
#include "markdowncompiler.h"

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
    void revertToSaved();

    void increaseFontSize();
    void decreaseFontSize();
    void showPreferences();
    void about();

    void selectTextToSearchFor();
    void findNextSearchMatch();
    void findPreviousSearchMatch();

    void formatSelectionEmphasized();
    void formatSelectionStrong();
    void formatSelectionCode();

    void preferencesUpdated();

    void openRecentFile();
    void compileToTempHTML();
    void compileToHTMLAs();
    void recompileToHTML();

    void anchorClicked(const QUrl &link);
    void commitDataHandler(QSessionManager &manager);
    void aboutToQuitHandler();
    void handleContentsChange(int position, int charsRemoved, int charsAdded);

    void reportStyleParsingErrors(QStringList *list);

private:
    QString getMarkdownFilesFilter();
    void setupEditor();
    void setupFileMenu();
    void updateRecentFilesMenu();
    void performStartupTasks();
    void addToRecentFiles(QString filePath);
    void persistFontInfo();
    void applyPersistedFontInfo();
    void applyHighlighterPreferences();
    void applyEditorPreferences();
    void setDirty(bool value);
    bool compileToHTMLFile(QString targetPath);

    MarkdownCompiler *compiler;
    QString lastCompileTargetPath;

    PreferencesDialog *preferencesDialog;
    QSettings *settings;
    QarkdownTextEdit *editor;
    HGMarkdownHighlighter *highlighter;
    QString openFilePath;
    QString searchString;

    QMenu *recentFilesMenu;
    QList<QAction *> *recentFilesMenuActions;

    QAction *findNextMenuAction;
    QAction *findPreviousMenuAction;
    QAction *revertToSavedMenuAction;
    QAction *recompileAction;
};

#endif
