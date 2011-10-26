#include "markdowncompiler.h"
#include "defines.h"
#include "qarkdownapplication.h"
#include "logger.h"

#include <QDir>
#include <QCoreApplication>
#include <QTextStream>

MarkdownCompiler::MarkdownCompiler(QSettings *appSettings, QObject *parent) :
    QObject(parent)
{
    settings = appSettings;
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
        Logger::warning("Cannot open file for reading: " + templateFile.fileName());
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
            Logger::info("Compiler dependency: " + depFileName);
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

QString getDefaultArgsForCompiler(QString compilerPath)
{
    if (compilerPath.startsWith(":/compilers/multimarkdown/multimarkdown-"))
        return "-c"; // markdown compatibility mode
    return "";
}

QString MarkdownCompiler::getSavedArgsForCompiler(QString compilerPath)
{
    QMap<QString, QVariant> argsMap = settings->value(SETTING_COMPILER_ARGS).toMap();

    if (!argsMap.contains(compilerPath))
        return getDefaultArgsForCompiler(compilerPath);
    else
        return argsMap.value(compilerPath).toString();
}

QStringList MarkdownCompiler::getArgsListForCompiler(QString compilerPath)
{
    QString args = getSavedArgsForCompiler(compilerPath);
    if (args.trimmed().length() == 0)
        return QStringList();
    QRegExp whitespaceRE("\\s+");
    return args.split(whitespaceRE, QString::SkipEmptyParts);
}

QString MarkdownCompiler::getUserReadableCompilerName(QString compilerPath)
{
    QString ret = compilerPath;
    if (ret.startsWith(":/"))
        ret = QFileInfo(ret).fileName();
    return ret;
}


QPair<QString, QString> MarkdownCompiler::compileSynchronously(QString input, QString compilerPath)
{
    Logger::info("Compiling with compiler: " + compilerPath);
    _errorString = QString();

    QString actualCompilerPath(compilerPath);
    bool isResourcePath = compilerPath.startsWith(":/");
    if (isResourcePath) {
        actualCompilerPath = getFilesystemPathForResourcePath(compilerPath);
        Logger::info("Adjusted path to: '"+actualCompilerPath+"'");
    }

    QProcess syncCompilerProcess;
    QStringList compilerArgsList = getArgsListForCompiler(compilerPath);
    Logger::debug("Compiler args: "+compilerArgsList.join(", "));

    // We need to supply an empty QStringList as the arguments (even if we
    // don't wish to supply arguments) so that QProcess understands that the
    // first argument is the executable path, and escapes spaces in the path
    // correctly:
    syncCompilerProcess.start(actualCompilerPath,
                              compilerArgsList,
                              QProcess::ReadWrite);

    if (!syncCompilerProcess.waitForStarted()) {
        Logger::warning("Cannot start process: " + actualCompilerPath);
        _errorString = syncCompilerProcess.errorString();
        return QPair<QString, QString>(QString(), QString());
    }

    syncCompilerProcess.write(input.toUtf8());
    syncCompilerProcess.closeWriteChannel();

    if (!syncCompilerProcess.waitForFinished()) {
        Logger::warning("Error while waiting process to finish: " + actualCompilerPath);
        _errorString = syncCompilerProcess.errorString();
        return QPair<QString, QString>(QString(), QString());
    }

    if (syncCompilerProcess.exitStatus() != QProcess::NormalExit) {
        Logger::warning("Process returned non-normal exit status: " + actualCompilerPath);
        _errorString = syncCompilerProcess.errorString();
        return QPair<QString, QString>(QString(), QString());
    }

    QString stderrString = QString::fromUtf8(syncCompilerProcess.readAllStandardError().constData());

    QTextStream in(&syncCompilerProcess);
    in.setCodec("UTF-8");
    QString stdoutString = in.readAll();

    return QPair<QString, QString>(stdoutString, stderrString);
}

bool MarkdownCompiler::compileToHTMLFile(QString compilerPath, QString input,
                                         QString targetPath)
{
    QPair<QString, QString> compilationOutput = this->compileSynchronously(input, compilerPath);
    if (compilationOutput.first.isNull())
    {
        if (_errorString.isNull() && !compilationOutput.second.isNull())
            _errorString = compilationOutput.second;
        return false;
    }

    QFile file(targetPath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        Logger::warning("compileToHTMLFile: Cannot open file for writing: '"+targetPath+"'");
        return false;
    }

    QString finalHTML = wrapHTMLContentInTemplate(compilationOutput.first);

    QTextStream fileStream(&file);
    fileStream << finalHTML;
    file.close();

    return (!finalHTML.isNull());
}










