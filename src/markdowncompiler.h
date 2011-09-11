#ifndef MARKDOWNCOMPILER_H
#define MARKDOWNCOMPILER_H

#include <QObject>
#include <QProcess>

class MarkdownCompiler : public QObject
{
    Q_OBJECT
public:
    explicit MarkdownCompiler(QObject *parent = 0);
    ~MarkdownCompiler();

    QString compileSynchronously(QString input, QString compilerPath);
    bool compileToHTMLFile(QString compilerPath, QString input, QString targetPath);

private:
    QProcess *compilerProcess;
    QString getFilesystemPathForResourcePath(QString resourcePath);

signals:

public slots:

};

#endif // MARKDOWNCOMPILER_H
