#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QTextCharFormat>
#include <QThread>
#include <QPlainTextEdit>

extern "C" {
#include "markdown_parser.h"
}

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class WorkerThread : public QThread
{
public:
    ~WorkerThread();
    void run();
    char *content;
    element **result;
};

struct HighlightingStyle
{
    element_type type;
    QTextCharFormat format;
};


class HGMarkdownHighlighter : public QObject
{
    Q_OBJECT

public:
    HGMarkdownHighlighter(QTextDocument *parent = 0, double aWaitInterval = 1);
    ~HGMarkdownHighlighter();
    QColor currentLineHighlightColor;

    void highlightNow();
    void parseAndHighlightNow();

    void setStyles(QVector<HighlightingStyle> &styles);
    void getStylesFromStylesheet(QString filePath, QPlainTextEdit *editor);

    double waitInterval();
    void setWaitInterval(double value);
    bool makeLinksClickable();
    void setMakeLinksClickable(bool value);

    void handleStyleParsingError(char *error_message);

signals:
    void styleParsingErrors(QStringList *errors);

protected:
    void beginListeningForContentChanged();
    void stopListeningForContentChanged();

private slots:
    void handleContentsChange(int position, int charsRemoved, int charsAdded);
    void threadFinished();
    void timerTimeout();

private:
    bool _makeLinksClickable;
    int _waitIntervalMilliseconds;
    QTimer *timer;
    QTextDocument *document;
    WorkerThread *workerThread;
    bool parsePending;
    element **cached_elements;
    QVector<HighlightingStyle> *highlightingStyles;
    QString cachedContent;
    QStringList *styleParsingErrorList;

    void clearFormatting();
    void highlight();
    void parse();
    void setDefaultStyles();

};

#endif
