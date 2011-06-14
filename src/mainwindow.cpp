#include <QtGui>

#include "mainwindow.h"
#include "defines.h"

/*
TODO:
- Strip styles upon copy/paste
- Not saved -indicator
- Opening files with Windows' "Open with..."
- Tabbed interface; multiple files open
- Changing styles settings
*/


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    settings = new QSettings("org.hasseg", "QarkDown");
    preferencesDialog = new PreferencesDialog(settings);
    openFileIsDirty = false;

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

void MainWindow::newFile()
{
    editor->clear();
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
    settings->setValue(SETTING_LAST_FILE, QVariant(openFilePath));
    setDirty(false);
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

    // tab stop width
    int tabWidthInChars = settings->value(SETTING_TAB_WIDTH,
                                          QVariant(DEF_TAB_WIDTH)).toInt();
    QFontMetrics fontMetrics(font);
    editor->setTabStopWidth(fontMetrics.charWidth("m", 0) * tabWidthInChars);
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

void MainWindow::showPreferences()
{
    preferencesDialog->show();
}

void MainWindow::preferencesUpdated()
{
    applyPersistedFontInfo();
}

void MainWindow::setDirty(bool value)
{
    openFileIsDirty = value;
    setWindowTitle(QFileInfo(openFilePath).fileName()
                   + (openFileIsDirty ? " **" : ""));
}


void MainWindow::setupEditor()
{
    editor = new QTextEdit;
    editor->setAcceptRichText(false);
    highlighter = new HGMarkdownHighlighter(editor->document(), 1000);
    applyPersistedFontInfo();
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
    if (settings->contains(SETTING_LAST_FILE))
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
}

void MainWindow::commitDataHandler(QSessionManager &manager)
{
    qDebug() << "commitDataHandler. dirty =" << openFileIsDirty;

    if (!openFileIsDirty)
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
    settings->sync();
}

void MainWindow::handleContentsChange(int position, int charsRemoved, int charsAdded)
{
    Q_UNUSED(position); Q_UNUSED(charsRemoved); Q_UNUSED(charsAdded);
    setDirty(true);
}
