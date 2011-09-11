#include "markdowncompiler.h"
#include "qarkdownapplication.h"

#include <QDebug>
#include <QDir>
#include <QCoreApplication>

MarkdownCompiler::MarkdownCompiler(QObject *parent) :
    QObject(parent)
{
    compilerProcess = NULL;
}
MarkdownCompiler::~MarkdownCompiler()
{
    if (compilerProcess != NULL)
        delete compilerProcess;
}

QString MarkdownCompiler::getHTMLTemplate()
{
    QString templateFilePath = HTML_TEMPLATE_FILE_PATH;
    if (!QFile::exists(templateFilePath))
        templateFilePath = ":/template.html";

    QFile templateFile(templateFilePath);
    if (!templateFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open file for reading:" << templateFile.fileName();
        return QString();
    }
    QTextStream stream(&templateFile);
    QString contents = stream.readAll();
    templateFile.close();
    return contents;
}

QString MarkdownCompiler::wrapHTMLContentInTemplate(QString htmlContent)
{
    QString templateStr = getHTMLTemplate();
    QRegExp contentCommentRE("\\<\\!--\\s*[Cc]ontent\\s*-->");
    QStringList parts = templateStr.split(contentCommentRE, QString::SkipEmptyParts);
    if (parts.count() == 2)
    {
        return parts[0] + htmlContent + parts[1];
    }
    else
    {
        _errorString = (parts.count() < 2)
                       ? "HTML template does not contain a content comment."
                       : "HTML template contains more than one content comment.";
        return QString();
    }
}

QString MarkdownCompiler::getFilesystemPathForResourcePath(QString resourcePath)
{
    QString fileName = QFileInfo(resourcePath).fileName();
    fileName = "qarkdown-" + QCoreApplication::applicationVersion() + "-compiler-" + fileName;
    QString targetFileDir = QDir::tempPath();
    QString targetFilePath = targetFileDir + QDir::separator() + fileName;

    if (!QFile::exists(targetFilePath)) {
        if (!((QarkdownApplication*)qApp)->copyResourceToFile(resourcePath,
                                                              targetFilePath))
            return QString();
    }

    if (!QFileInfo(targetFilePath).isExecutable()) {
        if (!QFile(targetFilePath).setPermissions(QFile::ExeUser))
            return QString();
    }

    if (QFile::exists(resourcePath + ".dependencies"))
    {
        QString depsDirPath = resourcePath + ".dependencies";
        QStringList depFiles = QDir(depsDirPath).entryList();
        foreach(QString depFileName, depFiles)
        {
            qDebug() << "Compiler dependency:" << depFileName;
            QString depTargetPath = targetFileDir + QDir::separator() + depFileName;
            if (!QFile::exists(depTargetPath)) {
                if (!((QarkdownApplication*)qApp)->copyResourceToFile(
                            depsDirPath + QDir::separator() + depFileName,
                            depTargetPath))
                    return QString();
            }
        }
    }

    return targetFilePath;
}

QString MarkdownCompiler::errorString()
{
    return _errorString;
}

QString MarkdownCompiler::compileSynchronously(QString input, QString compilerPath)
{
    qDebug() << "Compiling with compiler:" << compilerPath;
    _errorString = QString();

    QString actualCompilerPath(compilerPath);
    bool isResourcePath = compilerPath.startsWith(":/");
    if (isResourcePath) {
        actualCompilerPath = getFilesystemPathForResourcePath(compilerPath);
        qDebug() << "Adjusted path to: '"+actualCompilerPath+"'";
    }

    QProcess syncCompilerProcess;

    // We need to supply an empty QStringList as the arguments (even if we
    // don't wish to supply arguments) so that QProcess understands that the
    // first argument is the executable path, and escapes spaces in the path
    // correctly:
    syncCompilerProcess.start(actualCompilerPath, QStringList(), QProcess::ReadWrite);

    if (!syncCompilerProcess.waitForStarted()) {
        // TODO: handle error
        qDebug() << "Cannot start process:" << actualCompilerPath;
        _errorString = syncCompilerProcess.errorString();
        return QString();
    }

    syncCompilerProcess.write(input.toUtf8());
    syncCompilerProcess.closeWriteChannel();

    if (!syncCompilerProcess.waitForFinished()) {
        // TODO: handle error
        qDebug() << "Error while waiting process to finish:" << actualCompilerPath;
        _errorString = syncCompilerProcess.errorString();
        return QString();
    }

    if (syncCompilerProcess.exitStatus() != QProcess::NormalExit) {
        // TODO: handle error
        qDebug() << "Process returned non-normal exit status:" << actualCompilerPath;
        _errorString = syncCompilerProcess.errorString();
        return QString();
    }

    QByteArray output = syncCompilerProcess.readAll();

    return QString(output);
}

bool MarkdownCompiler::compileToHTMLFile(QString compilerPath, QString input,
                                         QString targetPath)
{
    QString compilationOutput = this->compileSynchronously(input, compilerPath);
    if (compilationOutput.isNull())
        return false;

    QFile file(targetPath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "compileToHTMLFile: Cannot open file for writing: '"+targetPath+"'";
        return false;
    }

    QString finalHTML = wrapHTMLContentInTemplate(compilationOutput);

    QTextStream fileStream(&file);
    fileStream << finalHTML;
    file.close();

    return (!finalHTML.isNull());
}










