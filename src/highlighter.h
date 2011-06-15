#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QTextCharFormat>
#include <QThread>

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
    void setStyles(QVector<HighlightingStyle> &styles);
    double waitInterval();
    void setWaitInterval(double value);

protected:

private slots:
    void handleContentsChange(int position, int charsRemoved, int charsAdded);
    void threadFinished();
    void timerTimeout();

private:
    int _waitIntervalMilliseconds;
    QTimer *timer;
    QTextDocument *document;
    WorkerThread *workerThread;
    bool parsePending;
    element **cached_elements;
    QVector<HighlightingStyle> *highlightingStyles;

    void clearFormatting();
    void highlight();
    void parse();
    void setDefaultStyles();
};

#endif
