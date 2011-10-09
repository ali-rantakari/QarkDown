#include <QtGui>
#include "highlighter.h"
#include "logger.h"

extern "C" {
#include "pmh_styleparser.h"
}


WorkerThread::~WorkerThread()
{
    if (result != NULL)
        pmh_free_elements(result);
    free(content);
}
void WorkerThread::run()
{
    if (content == NULL)
        return;
    pmh_markdown_to_elements(content, pmh_EXT_NONE, &result);
}




HGMarkdownHighlighter::HGMarkdownHighlighter(QTextDocument *parent,
                                             double aWaitInterval) : QObject(parent)
{
    highlightingStyles = NULL;
    workerThread = NULL;
    cached_elements = NULL;
    _makeLinksClickable = false;
    styleParsingErrorList = new QStringList();
    _waitIntervalMilliseconds = (int)(aWaitInterval*1000);
    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(_waitIntervalMilliseconds);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    document = parent;
    beginListeningForContentChanged();

    this->parse();
}

HGMarkdownHighlighter::~HGMarkdownHighlighter()
{
    delete styleParsingErrorList;
    delete timer;
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

bool HGMarkdownHighlighter::makeLinksClickable()
{
    return _makeLinksClickable;
}
void HGMarkdownHighlighter::setMakeLinksClickable(bool value)
{
    _makeLinksClickable = value;
}


void HGMarkdownHighlighter::beginListeningForContentChanged()
{
    connect(document, SIGNAL(contentsChange(int,int,int)),
            this, SLOT(handleContentsChange(int,int,int)));
}
void HGMarkdownHighlighter::stopListeningForContentChanged()
{
    disconnect(this, SLOT(handleContentsChange(int,int,int)));
}


#define STY(type, format) styles->append((HighlightingStyle){type, format})
void HGMarkdownHighlighter::setDefaultStyles()
{
    QVector<HighlightingStyle> *styles = new QVector<HighlightingStyle>();

    QTextCharFormat header1; header1.setForeground(QBrush(Qt::black));
    header1.setBackground(QBrush(QColor(178,178,207)));
    header1.setFontWeight(QFont::Bold);
    STY(pmh_H1, header1);

    QTextCharFormat header2; header2.setForeground(QBrush(Qt::darkBlue));
    header2.setBackground(QBrush(QColor(204,204,227)));
    header2.setFontWeight(QFont::Bold);
    STY(pmh_H2, header2);

    QTextCharFormat smallerHeaders; smallerHeaders.setForeground(QBrush(Qt::darkBlue));
    smallerHeaders.setBackground(QBrush(QColor(230,230,240)));
    STY(pmh_H3, smallerHeaders);
    STY(pmh_H4, smallerHeaders);
    STY(pmh_H5, smallerHeaders);
    STY(pmh_H6, smallerHeaders);

    QTextCharFormat hrule; hrule.setForeground(QBrush(Qt::darkGray));
    hrule.setBackground(QBrush(Qt::lightGray));
    STY(pmh_HRULE, hrule);

    QTextCharFormat list; list.setForeground(QBrush(Qt::darkMagenta));
    STY(pmh_LIST_BULLET, list);
    STY(pmh_LIST_ENUMERATOR, list);

    QTextCharFormat link; link.setForeground(QBrush(Qt::darkCyan));
    link.setBackground(QBrush(QColor(237,241,242)));
    STY(pmh_LINK, link);
    STY(pmh_AUTO_LINK_URL, link);
    STY(pmh_AUTO_LINK_EMAIL, link);

    QTextCharFormat image; image.setForeground(QBrush(Qt::darkCyan));
    image.setBackground(QBrush(Qt::cyan));
    STY(pmh_IMAGE, image);

    QTextCharFormat ref; ref.setForeground(QBrush(QColor(213,178,178)));
    STY(pmh_REFERENCE, ref);

    QTextCharFormat code; code.setForeground(QBrush(Qt::darkGreen));
    code.setBackground(QBrush(QColor(235,242,235)));
    STY(pmh_CODE, code);
    STY(pmh_VERBATIM, code);

    QTextCharFormat emph; emph.setForeground(QBrush(Qt::darkYellow));
    emph.setFontItalic(true);
    STY(pmh_EMPH, emph);

    QTextCharFormat strong; strong.setForeground(QBrush(QColor(115,50,115)));
    strong.setFontWeight(QFont::Bold);
    STY(pmh_STRONG, strong);

    QTextCharFormat comment; comment.setForeground(QBrush(Qt::gray));
    STY(pmh_COMMENT, comment);

    QTextCharFormat blockquote; blockquote.setForeground(QBrush(Qt::darkRed));
    STY(pmh_BLOCKQUOTE, blockquote);

    this->setStyles(*styles);
}

QColor colorFromARGBStyle(pmh_attr_argb_color *color)
{
    QColor qcolor;
    qcolor.setAlpha(color->alpha);
    qcolor.setRed(color->red);
    qcolor.setGreen(color->green);
    qcolor.setBlue(color->blue);
    return qcolor;
}

QBrush brushFromARGBStyle(pmh_attr_argb_color *color)
{
    return QBrush(colorFromARGBStyle(color));
}

QTextCharFormat getCharFormatFromStyleAttributes(pmh_style_attribute *list)
{
    QTextCharFormat format;
    while (list != NULL)
    {
        if (list->type == pmh_attr_type_foreground_color)
            format.setForeground(brushFromARGBStyle(list->value->argb_color));
        else if (list->type == pmh_attr_type_background_color)
            format.setBackground(brushFromARGBStyle(list->value->argb_color));
        else if (list->type == pmh_attr_type_font_style)
        {
            if (list->value->font_styles->bold)
                format.setFontWeight(QFont::Bold);
            if (list->value->font_styles->italic)
                format.setFontItalic(true);
            if (list->value->font_styles->underlined)
                format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        }
        list = list->next;
    }
    return format;
}

QPalette getDefaultPlainTextEditPalette()
{
    static bool hasBeenCached = false;
    static QPalette palette;
    if (!hasBeenCached)
    {
        QPlainTextEdit *pte = new QPlainTextEdit();
        palette = pte->palette();
        delete pte;
        hasBeenCached = true;
    }
    return palette;
}


void styleParserErrorCallback(char *error_message, int line_number, void *context)
{
    ((HGMarkdownHighlighter*)context)->handleStyleParsingError(error_message,
                                                               line_number);
}

void HGMarkdownHighlighter::handleStyleParsingError(char *error_message,
                                                    int line_number)
{
    styleParsingErrorList->append(QString("(Line %1): ").arg(line_number)
                                  + QString(error_message));
}

void HGMarkdownHighlighter::getStylesFromStylesheet(QString filePath, QPlainTextEdit *editor)
{
    QString stylesheet;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        stylesheet = stream.readAll();
    }
    QByteArray arr = stylesheet.toUtf8();
    const char *stylesheet_cstring = arr.data();

    QVector<HighlightingStyle> *styles = new QVector<HighlightingStyle>();

    styleParsingErrorList->clear();
    pmh_style_collection *raw_styles = pmh_parse_styles((char *)stylesheet_cstring,
                                                        &styleParserErrorCallback,
                                                        this);
    if (styleParsingErrorList->count() > 0)
        emit styleParsingErrors(styleParsingErrorList);

    // Set language element styles
    for (int i = 0; i < pmh_NUM_LANG_TYPES; i++)
    {
        pmh_style_attribute *cur = raw_styles->element_styles[i];
        if (cur == NULL)
            continue;
        pmh_element_type lang_element_type = cur->lang_element_type;
        QTextCharFormat format = getCharFormatFromStyleAttributes(cur);
        STY(lang_element_type, format);
    }

    this->setStyles(*styles);

    // Set editor styles
    if (editor != NULL)
    {
        QPalette palette = getDefaultPlainTextEditPalette();

        // Editor area styles
        if (raw_styles->editor_styles != NULL)
        {
            pmh_style_attribute *cur = raw_styles->editor_styles;
            while (cur != NULL)
            {
                if (cur->type == pmh_attr_type_background_color)
                    palette.setColor(QPalette::Base, colorFromARGBStyle(cur->value->argb_color));
                else if (cur->type == pmh_attr_type_foreground_color)
                    palette.setColor(QPalette::Text, colorFromARGBStyle(cur->value->argb_color));
                cur = cur->next;
            }
        }

        // Selection styles
        if (raw_styles->editor_selection_styles != NULL)
        {
            pmh_style_attribute *cur = raw_styles->editor_selection_styles;
            while (cur != NULL)
            {
                if (cur->type == pmh_attr_type_background_color)
                    palette.setColor(QPalette::Highlight, colorFromARGBStyle(cur->value->argb_color));
                else if (cur->type == pmh_attr_type_foreground_color)
                    palette.setColor(QPalette::HighlightedText, colorFromARGBStyle(cur->value->argb_color));
                cur = cur->next;
            }
        }

        // Current line styles (not applied; simply stored into a public
        // ivar so that someone else can read it from there)
        if (raw_styles->editor_current_line_styles != NULL)
        {
            pmh_style_attribute *cur = raw_styles->editor_current_line_styles;
            while (cur != NULL)
            {
                if (cur->type == pmh_attr_type_background_color)
                    currentLineHighlightColor = colorFromARGBStyle(cur->value->argb_color);
                cur = cur->next;
            }
        }
        else
            currentLineHighlightColor = QColor();

        editor->setPalette(palette);
    }

    pmh_free_style_collection(raw_styles);
}

void HGMarkdownHighlighter::clearFormatting()
{
    QTextBlock block = document->firstBlock();
    while (block.isValid())
    {
        block.layout()->clearAdditionalFormats();
        block = block.next();
    }
}

void HGMarkdownHighlighter::highlight()
{
    if (cached_elements == NULL) {
        Logger::warning("cached_elements is NULL");
        return;
    }

    if (highlightingStyles == NULL)
        this->setDefaultStyles();

    this->clearFormatting();

    qDebug() << "characterCount" << document->characterCount();
    qDebug() << "toPlainText length" << document->toPlainText().length();

    // QTextDocument::characterCount returns a value one higher than the
    // actual character count.
    // See: https://bugreports.qt.nokia.com//browse/QTBUG-4841
    // document->toPlainText().length() would give us the correct value
    // but it's probably too slow.
    unsigned long max_offset = document->characterCount() - 1;

    for (int i = 0; i < highlightingStyles->size(); i++)
    {
        HighlightingStyle style = highlightingStyles->at(i);
        pmh_element *elem_cursor = cached_elements[style.type];
        while (elem_cursor != NULL)
        {
            unsigned long pos = elem_cursor->pos;
            unsigned long end = elem_cursor->end;

            if (end <= pos || max_offset < pos)
            {
                elem_cursor = elem_cursor->next;
                continue;
            }

            if (max_offset < end)
                end = max_offset;

            // "The QTextLayout object can only be modified from the
            // documentChanged implementation of a QAbstractTextDocumentLayout
            // subclass. Any changes applied from the outside cause undefined
            // behavior." -- we are breaking this rule here. There might be
            // a better (more correct) way to do this.

            int startBlockNum = document->findBlock(pos).blockNumber();
            int endBlockNum = document->findBlock(end).blockNumber();
            for (int j = startBlockNum; j <= endBlockNum; j++)
            {
                QTextBlock block = document->findBlockByNumber(j);

                QTextLayout *layout = block.layout();
                QList<QTextLayout::FormatRange> list = layout->additionalFormats();
                int blockpos = block.position();
                QTextLayout::FormatRange r;
                r.format = style.format;

                if (_makeLinksClickable
                    && (elem_cursor->type == pmh_LINK
                        || elem_cursor->type == pmh_AUTO_LINK_URL
                        || elem_cursor->type == pmh_AUTO_LINK_EMAIL
                        || elem_cursor->type == pmh_REFERENCE)
                    && elem_cursor->address != NULL)
                {
                    QString address(elem_cursor->address);
                    if (elem_cursor->type == pmh_AUTO_LINK_EMAIL && !address.startsWith("mailto:"))
                        address = "mailto:" + address;
                    QTextCharFormat linkFormat(r.format);
                    linkFormat.setAnchor(true);
                    linkFormat.setAnchorHref(address);
                    linkFormat.setToolTip(address);
                    r.format = linkFormat;
                }

                if (j == startBlockNum) {
                    r.start = pos - blockpos;
                    r.length = (startBlockNum == endBlockNum)
                                ? end - pos
                                : block.length() - r.start;
                } else if (j == endBlockNum) {
                    r.start = 0;
                    r.length = end - blockpos;
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
        pmh_free_elements(cached_elements);
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

    //Logger::debug("contents changed. chars removed/added:" + charsRemoved + " " + charsAdded);

    timer->stop();
    timer->start();
}

void HGMarkdownHighlighter::timerTimeout()
{
    this->parse();
}

void HGMarkdownHighlighter::highlightNow()
{
    highlight();
}

void HGMarkdownHighlighter::parseAndHighlightNow()
{
    parse();
}

