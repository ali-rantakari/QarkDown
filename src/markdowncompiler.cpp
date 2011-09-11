#include "markdowncompiler.h"

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

bool copyResourceToFile(QString resourcePath, QString targetFilePath)
{
    QFile source(resourcePath);
    QByteArray contents;
    if (!source.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open file for reading:" << source.fileName();
        return false;
    }
    contents = source.readAll();
    source.close();

    QFile target(targetFilePath);
    if (!target.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open file for writing:" << target.fileName();
        return false;
    }
    target.write(contents);
    target.close();

    return true;
}

QString MarkdownCompiler::getFilesystemPathForResourcePath(QString resourcePath)
{
    QString fileName = QFileInfo(resourcePath).fileName();
    fileName = "qarkdown-" + QCoreApplication::applicationVersion() + "-compiler-" + fileName;
    QString targetFileDir = QDir::tempPath();
    QString targetFilePath = targetFileDir + QDir::separator() + fileName;

    if (!QFile::exists(targetFilePath)) {
        if (!copyResourceToFile(resourcePath, targetFilePath))
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
                if (!copyResourceToFile(depsDirPath + QDir::separator() + depFileName,
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
    qDebug() << "output:" << output;

    return QString(output);
}

bool MarkdownCompiler::compileToHTMLFile(QString compilerPath, QString input,
                                         QString targetPath)
{
    QString htmlHeader =
            "<!DOCTYPE html>\n"
            "<html lang='en'>\n"
            "<head>\n"
            "    <meta charset='utf-8'/>\n"
            "    <title></title>\n"
            "</head>\n"
            "<body>\n";
    QString htmlFooter = "\n</body>\n</html>";

    QString compiled = this->compileSynchronously(input, compilerPath);
    if (compiled.isNull())
        return false;

    QFile file(targetPath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "compileToHTMLFile: Cannot open file for writing: '"+targetPath+"'";
        return false;
    }
    QTextStream fileStream(&file);
    fileStream << htmlHeader;
    fileStream << compiled;
    fileStream << htmlFooter;
    file.close();

    return true;
}










