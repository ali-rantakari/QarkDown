#include <QtGui>

#include "mainwindow.h"

/*
TODO:
- Opening files with Windows' "Open with..."
- Tabbed interface; multiple files open
- Changing styles settings
*/

#define MARKDOWN_FILES_FILTER "Markdown Files (*.mdtext *.md *.markdown *.txt *.text)"
#define SETTING_FONT "Font"
#define SETTING_LAST_FILE "LastFile"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    settings = new QSettings("org.hasseg", "QarkDown");
    openFileIsDirty = false;

    setupFileMenu();
    setupEditor();
    performStartupTasks();
    setCentralWidget(editor);
}

MainWindow::~MainWindow()
{
    delete settings;
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
    setWindowTitle(QFileInfo(openFilePath).fileName());

    file.close();
    settings->setValue(SETTING_LAST_FILE, QVariant(openFilePath));
    openFileIsDirty = false;
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
    openFileIsDirty = false;
}

void MainWindow::persistFontInfo()
{
    settings->setValue(SETTING_FONT, QVariant(editor->font().toString()));
}
void MainWindow::applyPersistedFontInfo()
{
    QFont font;

    if (settings->contains(SETTING_FONT))
        font.fromString(settings->value(SETTING_FONT).toString());
    else {
        font.setFamily("Courier");
        font.setFixedPitch(true);
        font.setPointSize(12);
    }

    editor->setFont(font);
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

void MainWindow::changeFont()
{
    bool ok;
    QFont newFont = QFontDialog::getFont(&ok, editor->font(), this, tr("Select New Font"));
    if (!ok)
        return;
    editor->setFont(newFont);
    persistFontInfo();
}


void MainWindow::setupEditor()
{
    editor = new QTextEdit;
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

    QMenu *fontOptionsMenu = new QMenu(tr("&Font"), this);
    menuBar()->addMenu(fontOptionsMenu);
    QKeySequence increaseFontSizeKey("Ctrl++");
    fontOptionsMenu->addAction(tr("Increase Font Size"), this, SLOT(increaseFontSize()),
                        increaseFontSizeKey);
    QKeySequence decreaseFontSizeKey("Ctrl+-");
    fontOptionsMenu->addAction(tr("Decrease Font Size"), this, SLOT(decreaseFontSize()),
                        decreaseFontSizeKey);
    fontOptionsMenu->addAction(tr("Select Font"), this, SLOT(changeFont()));
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
    openFileIsDirty = true;
}
