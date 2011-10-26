#ifndef MARKDOWNCOMPILER_H
#define MARKDOWNCOMPILER_H

#include <QObject>
#include <QProcess>
#include <QSettings>

class MarkdownCompiler : public QObject
{
    Q_OBJECT
public:
    explicit MarkdownCompiler(QSettings *appSettings, QObject *parent = 0);
    ~MarkdownCompiler();

    QPair<QString, QString> compileSynchronously(QString input, QString compilerPath);
    bool compileToHTMLFile(QString compilerPath, QString input, QString targetPath);
    QString getUserReadableCompilerName(QString compilerPath);
    QString errorString();
    QString getHTMLTemplate();
    QString wrapHTMLContentInTemplate(QString htmlContent);
    QString getSavedArgsForCompiler(QString compilerPath);
    QStringList getArgsListForCompiler(QString compilerPath);

private:
    QSettings *settings;
    QProcess *compilerProcess;
    QString _errorString;
    QString getFilesystemPathForResourcePath(QString resourcePath);

signals:

public slots:

};

#endif // MARKDOWNCOMPILER_H
