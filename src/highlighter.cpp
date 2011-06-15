#include <QtGui>
#include "highlighter.h"


WorkerThread::~WorkerThread()
{
    if (result != NULL)
        free_elements(result);
    free(content);
}
void WorkerThread::run()
{
    if (content == NULL)
        return;
    markdown_to_elements(content, 0, &result);
}




HGMarkdownHighlighter::HGMarkdownHighlighter(QTextDocument *parent,
                                             double aWaitInterval) : QObject(parent)
{
    highlightingStyles = NULL;
    workerThread = NULL;
    cached_elements = NULL;
    _waitIntervalMilliseconds = (int)(aWaitInterval*1000);
    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(_waitIntervalMilliseconds);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    document = parent;
    connect(document, SIGNAL(contentsChange(int,int,int)),
            this, SLOT(handleContentsChange(int,int,int)));

    this->parse();
}

void HGMarkdownHighlighter::setStyles(QVector<HighlightingStyle> &styles)
{
    this->highlightingStyles = &styles;
}

double HGMarkdownHighlighter::waitInterval()
{
    return ((double)this->_waitIntervalMilliseconds)/1000.0;
}
void HGMarkdownHighlighter::setWaitInterval(double value)
{
    this->_waitIntervalMilliseconds = (int)(value*1000);
    timer->setInterval(_waitIntervalMilliseconds);
}



#define STY(type, format) styles->append((HighlightingStyle){type, format})
void HGMarkdownHighlighter::setDefaultStyles()
{
    QVector<HighlightingStyle> *styles = new QVector<HighlightingStyle>();

    QTextCharFormat header1; header1.setForeground(QBrush(Qt::black));
    header1.setBackground(QBrush(QColor(178,178,207)));
    header1.setFontWeight(QFont::Bold);
    STY(H1, header1);

    QTextCharFormat header2; header2.setForeground(QBrush(Qt::darkBlue));
    header2.setBackground(QBrush(QColor(204,204,227)));
    header2.setFontWeight(QFont::Bold);
    STY(H2, header2);

    QTextCharFormat smallerHeaders; smallerHeaders.setForeground(QBrush(Qt::darkBlue));
    smallerHeaders.setBackground(QBrush(QColor(230,230,240)));
    STY(H3, smallerHeaders);
    STY(H4, smallerHeaders);
    STY(H5, smallerHeaders);
    STY(H6, smallerHeaders);

    QTextCharFormat hrule; hrule.setForeground(QBrush(Qt::darkGray));
    hrule.setBackground(QBrush(Qt::lightGray));
    STY(HRULE, hrule);

    QTextCharFormat list; list.setForeground(QBrush(Qt::darkMagenta));
    STY(LIST_BULLET, list);
    STY(LIST_ENUMERATOR, list);

    QTextCharFormat link; link.setForeground(QBrush(Qt::darkCyan));
    link.setBackground(QBrush(QColor(237,241,242)));
    STY(LINK, link);
    STY(AUTO_LINK_URL, link);
    STY(AUTO_LINK_EMAIL, link);

    QTextCharFormat image; image.setForeground(QBrush(Qt::darkCyan));
    image.setBackground(QBrush(Qt::cyan));
    STY(IMAGE, image);

    QTextCharFormat ref; ref.setForeground(QBrush(QColor(213,178,178)));
    STY(REFERENCE, ref);

    QTextCharFormat code; code.setForeground(QBrush(Qt::darkGreen));
    code.setBackground(QBrush(QColor(235,242,235)));
    STY(CODE, code);
    STY(VERBATIM, code);

    QTextCharFormat emph; emph.setForeground(QBrush(Qt::darkYellow));
    emph.setFontItalic(true);
    STY(EMPH, emph);

    QTextCharFormat strong; strong.setForeground(QBrush(QColor(115,50,115)));
    strong.setFontWeight(QFont::Bold);
    STY(STRONG, strong);

    QTextCharFormat comment; comment.setForeground(QBrush(Qt::gray));
    STY(COMMENT, comment);

    QTextCharFormat blockquote; blockquote.setForeground(QBrush(Qt::darkRed));
    STY(BLOCKQUOTE, blockquote);

    this->setStyles(*styles);
}

void HGMarkdownHighlighter::clearFormatting()
{
    QTextBlock block = document->firstBlock();
    while (block.isValid()) {
        block.layout()->clearAdditionalFormats();
        block = block.next();
    }
}

void HGMarkdownHighlighter::highlight()
{
    if (cached_elements == NULL) {
        qDebug() << "cached_elements is NULL";
        return;
    }

    if (highlightingStyles == NULL)
        this->setDefaultStyles();

    this->clearFormatting();

    for (int i = 0; i < highlightingStyles->size(); i++)
    {
        HighlightingStyle style = highlightingStyles->at(i);
        element *elem_cursor = cached_elements[style.type];
        while (elem_cursor != NULL)
        {
            if (elem_cursor->end <= elem_cursor->pos) {
                elem_cursor = elem_cursor->next;
                continue;
            }

            // "The QTextLayout object can only be modified from the
            // documentChanged implementation of a QAbstractTextDocumentLayout
            // subclass. Any changes applied from the outside cause undefined
            // behavior." -- we are breaking this rule here. There might be
            // a better (more correct) way to do this.

            int startBlockNum = document->findBlock(elem_cursor->pos).blockNumber();
            int endBlockNum = document->findBlock(elem_cursor->end).blockNumber();
            for (int j = startBlockNum; j <= endBlockNum; j++)
            {
                QTextBlock block = document->findBlockByNumber(j);

                QTextLayout *layout = block.layout();
                QList<QTextLayout::FormatRange> list = layout->additionalFormats();
                int blockpos = block.position();
                QTextLayout::FormatRange r;
                r.format = style.format;

                if (j == startBlockNum) {
                    r.start = elem_cursor->pos - blockpos;
                    r.length = (startBlockNum == endBlockNum)
                                ? elem_cursor->end - elem_cursor->pos
                                : block.length() - r.start;
                } else if (j == endBlockNum) {
                    r.start = 0;
                    r.length = elem_cursor->end - blockpos;
                } else {
                    r.start = 0;
                    r.length = block.length();
                }

                list.append(r);
                layout->setAdditionalFormats(list);
            }

            elem_cursor = elem_cursor->next;
        }
    }

    document->markContentsDirty(0, document->characterCount());
}

void HGMarkdownHighlighter::parse()
{
    if (workerThread != NULL && workerThread->isRunning()) {
        parsePending = true;
        return;
    }

    QString content = document->toPlainText();
    QByteArray ba = content.toLatin1();
    char *content_cstring = strdup((char *)ba.data());

    if (workerThread != NULL)
        delete workerThread;
    workerThread = new WorkerThread();
    workerThread->content = content_cstring;
    connect(workerThread, SIGNAL(finished()), this, SLOT(threadFinished()));
    parsePending = false;
    workerThread->start();
}

void HGMarkdownHighlighter::threadFinished()
{
    if (parsePending) {
        this->parse();
        return;
    }

    if (cached_elements != NULL)
        free_elements(cached_elements);
    cached_elements = workerThread->result;
    workerThread->result = NULL;

    this->highlight();
}

void HGMarkdownHighlighter::handleContentsChange(int position, int charsRemoved,
                                                 int charsAdded)
{
    Q_UNUSED(position);
    if (charsRemoved == 0 && charsAdded == 0)
        return;
    //qDebug() << "contents changed. chars removed/added:" << charsRemoved << charsAdded;
    timer->stop();
    timer->start();
}

void HGMarkdownHighlighter::timerTimeout()
{
    this->parse();
}
