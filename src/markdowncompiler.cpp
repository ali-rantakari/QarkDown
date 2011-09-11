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


QString MarkdownCompiler::getFilesystemPathForResourcePath(QString resourcePath)
{
    QString fileName = QFileInfo(resourcePath).fileName();
    fileName = "qarkdown-" + QCoreApplication::applicationVersion() + "-compiler-" + fileName;
    QString path = QDir::tempPath() + QDir::separator() + fileName;
    QFile target(path);

    if (!QFile::exists(path))
    {
        QFile source(resourcePath);
        QByteArray contents;
        if (!source.open(QIODevice::ReadOnly))
            return QString();
        contents = source.readAll();
        source.close();

        if (!target.open(QIODevice::WriteOnly))
            return QString();
        target.write(contents);
        target.close();
    }

    if (!QFileInfo(path).isExecutable()) {
        if (!target.setPermissions(QFile::ExeUser))
            return QString();
    }

    return path;
}


QString MarkdownCompiler::compileSynchronously(QString input, QString compilerPath)
{
    qDebug() << "Compiling with compiler: '"+compilerPath+"'";

    QString actualCompilerPath(compilerPath);
    bool isResourcePath = compilerPath.startsWith(":/");
    if (isResourcePath) {
        actualCompilerPath = getFilesystemPathForResourcePath(compilerPath);
        qDebug() << "Adjusted path to: '"+actualCompilerPath+"'";
    }

    QProcess syncCompilerProcess;
    syncCompilerProcess.start(actualCompilerPath, QProcess::ReadWrite);
    if (!syncCompilerProcess.waitForStarted()) {
        // TODO: handle error
        qDebug() << "Cannot start process: '"+actualCompilerPath+"'";
        return QString();
    }

    syncCompilerProcess.write(input.toUtf8());
    syncCompilerProcess.closeWriteChannel();

    if (!syncCompilerProcess.waitForFinished()) {
        // TODO: handle error
        qDebug() << "Error while waiting process to finish: '"+actualCompilerPath+"'";
        return QString();
    }

    if (syncCompilerProcess.exitStatus() != QProcess::NormalExit) {
        // TODO: handle error
        qDebug() << "Process returned non-normal exit status: '"+actualCompilerPath+"'";
        return QString();
    }

    QByteArray output = syncCompilerProcess.readAll();

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










