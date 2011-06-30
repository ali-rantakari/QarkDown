#include <QtGui>

#include "mainwindow.h"
#include "defines.h"

/*
TODO:
- Line numbers
- Document the highlighter interface

- Get icon working on OS X
- Use QTextOption::ShowTabsAndSpaces
- Tabbed interface; multiple files open
- Changing styles settings
*/


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    settings = new QSettings("org.hasseg", "QarkDown");
    preferencesDialog = new PreferencesDialog(settings);

    setupFileMenu();
    setupEditor();
    performStartupTasks();
    setCentralWidget(editor);
}

MainWindow::~MainWindow()
{
    delete settings;
    delete preferencesDialog;
}

void MainWindow::show()
{
    QSize defaultSize(640, 512);
    resize(defaultSize);

    bool rememberWindow = settings->value(SETTING_REMEMBER_WINDOW,
                                          QVariant(DEF_REMEMBER_WINDOW)).toBool();
    if (rememberWindow) {
        restoreGeometry(settings->value(SETTING_WINDOW_GEOMETRY).toByteArray());
        restoreState(settings->value(SETTING_WINDOW_STATE).toByteArray());
    }

    QMainWindow::show();
}

void MainWindow::newFile()
{
    editor->clear();
    openFilePath = QString();
    setDirty(false);
}

void MainWindow::openFile(const QString &path)
{
    QString fileName = path;

    if (fileName.isNull())
        fileName = QFileDialog::getOpenFileName(this,
            tr("Open File"), "", MARKDOWN_FILES_FILTER);

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return;

    editor->setPlainText(file.readAll());
    openFilePath = fileName;

    file.close();
    setDirty(false);
    bool rememberLastFile = settings->value(SETTING_REMEMBER_LAST_FILE,
                                            QVariant(DEF_REMEMBER_LAST_FILE)).toBool();
    if (rememberLastFile)
        settings->setValue(SETTING_LAST_FILE, QVariant(openFilePath));
}

void MainWindow::saveFile()
{
    QString saveFilePath(openFilePath);
    if (saveFilePath.isNull())
        saveFilePath = QFileDialog::getSaveFileName(this,
            tr("Save File"), "", MARKDOWN_FILES_FILTER);

    QFile file(saveFilePath);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return;
    QTextStream fileStream(&file);
    fileStream << editor->toPlainText();
    file.close();

    openFilePath = saveFilePath;
    setWindowTitle(QFileInfo(openFilePath).fileName());
    setDirty(false);
}

void MainWindow::persistFontInfo()
{
    settings->setValue(SETTING_FONT, QVariant(editor->font().toString()));
}
void MainWindow::applyPersistedFontInfo()
{
    // font
    QFont font;
    if (settings->contains(SETTING_FONT))
        font.fromString(settings->value(SETTING_FONT).toString());
    else {
        font.setFamily(DEF_FONT_FAMILY);
        font.setPointSize(DEF_FONT_SIZE);
        font.setFixedPitch(true);
    }
    editor->setFont(font);

    // tab stop width (dependent on font)
    int tabWidthInChars = settings->value(SETTING_TAB_WIDTH,
                                          QVariant(DEF_TAB_WIDTH)).toInt();
    QFontMetrics fontMetrics(font);
    editor->setTabStopWidth(fontMetrics.charWidth("m", 0) * tabWidthInChars);
}

void MainWindow::selectTextToSearchFor()
{
    bool ok;
    QString str = QInputDialog::getText(this, tr("Find Text"),
                                        tr("Enter text to find:"),
                                        QLineEdit::Normal, searchString, &ok);
    if (!ok || str.isEmpty())
        return;
    searchString = str;
    findNextMenuAction->setEnabled(true);
    findPreviousMenuAction->setEnabled(true);
    findNextSearchMatch();
}

void MainWindow::findNextSearchMatch()
{
    if (searchString.isEmpty())
        return;
    editor->find(searchString);
}

void MainWindow::findPreviousSearchMatch()
{
    if (searchString.isEmpty())
        return;
    editor->find(searchString, QTextDocument::FindBackward);
}

void MainWindow::increaseFontSize()
{
    QFont font(editor->font());
    font.setPointSize(editor->font().pointSize() + 1);
    editor->setFont(font);
    persistFontInfo();
}

void MainWindow::decreaseFontSize()
{
    QFont font(editor->font());
    font.setPointSize(editor->font().pointSize() - 1);
    editor->setFont(font);
    persistFontInfo();
}

void MainWindow::applyHighlighterPreferences()
{
    double highlightInterval = settings->value(SETTING_HIGHLIGHT_INTERVAL,
                                               QVariant(DEF_HIGHLIGHT_INTERVAL)).toDouble();
    highlighter->setWaitInterval(highlightInterval);
    bool clickableLinks = settings->value(SETTING_CLICKABLE_LINKS,
                                          QVariant(DEF_CLICKABLE_LINKS)).toBool();
    highlighter->setMakeLinksClickable(clickableLinks);
}

void MainWindow::applyEditorPreferences()
{
    // Indentation
    bool indentWithTabs = settings->value(SETTING_INDENT_WITH_TABS,
                                          QVariant(DEF_INDENT_WITH_TABS)).toBool();
    int tabWidthInChars = settings->value(SETTING_TAB_WIDTH,
                                          QVariant(DEF_TAB_WIDTH)).toInt();
    editor->setSpacesIndentWidthHint(tabWidthInChars);
    if (indentWithTabs)
        editor->setIndentString("\t");
    else
    {
        QString indentStr = " ";
        for (int i = 1; i < tabWidthInChars; i++)
            indentStr += " ";
        editor->setIndentString(indentStr);
    }

    // Current line highlighting
    bool highlightCurrentLine = settings->value(SETTING_HIGHLIGHT_CURRENT_LINE,
                                                QVariant(DEF_HIGHLIGHT_CURRENT_LINE)).toBool();
    editor->setHighlightCurrentLine(highlightCurrentLine);
    QColor lineHighlightColor = settings->value(SETTING_LINE_HIGHLIGHT_COLOR,
                                                QVariant(DEF_LINE_HIGHLIGHT_COLOR)).value<QColor>();
    editor->setCurrentLineHighlightColor(lineHighlightColor);
}

void MainWindow::showPreferences()
{
    preferencesDialog->show();
}

void MainWindow::preferencesUpdated()
{
    applyPersistedFontInfo();
    applyHighlighterPreferences();
    applyEditorPreferences();
    highlighter->highlightNow();
}

void MainWindow::setDirty(bool value)
{
    editor->document()->setModified(value);
    QString dirtyFlagStr = (value ? " **" : "");
    if (!openFilePath.isNull())
        setWindowTitle(QFileInfo(openFilePath).fileName()+dirtyFlagStr);
    else
        setWindowTitle("<untitled>"+dirtyFlagStr);
}


void MainWindow::setupEditor()
{
    editor = new QarkdownTextEdit;
    editor->setAnchorClickKeyboardModifiers(Qt::ControlModifier);
    highlighter = new HGMarkdownHighlighter(editor->document());

    applyPersistedFontInfo();
    applyHighlighterPreferences();
    applyEditorPreferences();
}

void MainWindow::setupFileMenu()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);
    fileMenu->addAction(tr("&New"), this, SLOT(newFile()),
                        QKeySequence::New);
    fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()),
                        QKeySequence::Open);
    fileMenu->addAction(tr("&Save"), this, SLOT(saveFile()),
                        QKeySequence::Save);
    fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()),
                        QKeySequence::Quit);

    QMenu *editMenu = new QMenu(tr("&Edit"), this);
    menuBar()->addMenu(editMenu);
    editMenu->addAction(tr("Find..."), this, SLOT(selectTextToSearchFor()),
                        QKeySequence::Find);
    findNextMenuAction = editMenu->addAction(tr("Find Next"),
                                             this, SLOT(findNextSearchMatch()),
                                             QKeySequence::FindNext);
    findPreviousMenuAction = editMenu->addAction(tr("Find Previous"),
                                                 this, SLOT(findPreviousSearchMatch()),
                                                 QKeySequence::FindPrevious);
    findNextMenuAction->setEnabled(false);
    findPreviousMenuAction->setEnabled(false);

    QMenu *toolsMenu = new QMenu(tr("&Tools"), this);
    menuBar()->addMenu(toolsMenu);
    QKeySequence increaseFontSizeKey("Ctrl++");
    toolsMenu->addAction(tr("Increase Font Size"), this, SLOT(increaseFontSize()),
                         increaseFontSizeKey);
    QKeySequence decreaseFontSizeKey("Ctrl+-");
    toolsMenu->addAction(tr("Decrease Font Size"), this, SLOT(decreaseFontSize()),
                         decreaseFontSizeKey);
    toolsMenu->addAction(tr("Preferences..."), this, SLOT(showPreferences()),
                         QKeySequence::Preferences);
}

void MainWindow::performStartupTasks()
{
    bool rememberLastFile = settings->value(SETTING_REMEMBER_LAST_FILE,
                                            QVariant(DEF_REMEMBER_LAST_FILE)).toBool();
    if (rememberLastFile && settings->contains(SETTING_LAST_FILE))
        openFile(settings->value(SETTING_LAST_FILE).toString());

    //connect(qApp, SIGNAL(saveStateRequest(QSessionManager&)),
    //        this, SLOT(saveStateHandler(QSessionManager&)), Qt::DirectConnection);
    connect(qApp, SIGNAL(commitDataRequest(QSessionManager&)),
            this, SLOT(commitDataHandler(QSessionManager&)), Qt::DirectConnection);
    connect(qApp, SIGNAL(aboutToQuit()),
            this, SLOT(aboutToQuitHandler()), Qt::DirectConnection);
    connect(editor->document(), SIGNAL(contentsChange(int,int,int)),
            this, SLOT(handleContentsChange(int,int,int)));
    connect(preferencesDialog, SIGNAL(updated()),
            this, SLOT(preferencesUpdated()));
    connect(editor, SIGNAL(anchorClicked(QUrl)),
            this, SLOT(anchorClicked(QUrl)));
}

void MainWindow::anchorClicked(const QUrl &link)
{
    qDebug() << "anchor clicked:" << link;
    QDesktopServices::openUrl(link);
}

void MainWindow::commitDataHandler(QSessionManager &manager)
{
    qDebug() << "commitDataHandler. dirty =" << editor->document()->isModified();

    if (!editor->document()->isModified())
        return;

    if (manager.allowsInteraction())
    {
        qDebug() << "allows interaction.";

        int ret = QMessageBox::warning(
                    this,
                    tr("QarkDown"),
                    tr("Save changes to file?"),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
                    );
        switch (ret)
        {
            case QMessageBox::Save:
                manager.release();
                saveFile();
                break;
            case QMessageBox::Discard:
                break;
            case QMessageBox::Cancel:
            default:
                manager.cancel();
        }
    }
    else // no permission for interaction
    {
        qDebug() << "interaction not allowed -- saving.";
        saveFile();
    }
}

void MainWindow::aboutToQuitHandler()
{
    // No user interaction allowed here
    bool rememberWindow = settings->value(SETTING_REMEMBER_WINDOW,
                                          QVariant(DEF_REMEMBER_WINDOW)).toBool();
    if (rememberWindow) {
        settings->setValue(SETTING_WINDOW_GEOMETRY, saveGeometry());
        settings->setValue(SETTING_WINDOW_STATE, saveState());
    }
    settings->sync();
}

void MainWindow::handleContentsChange(int position, int charsRemoved, int charsAdded)
{
    Q_UNUSED(position); Q_UNUSED(charsRemoved); Q_UNUSED(charsAdded);
    setDirty(true);
}
