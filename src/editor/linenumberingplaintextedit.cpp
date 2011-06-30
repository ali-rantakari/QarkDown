#include "linenumberingplaintextedit.h"

#include <QTextBlock>
#include <QPainter>

LineNumberingPlainTextEdit::LineNumberingPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)),
            this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)),
            this, SLOT(updateLineNumberArea(QRect,int)));

    updateLineNumberAreaWidth(0);
}

LineNumberingPlainTextEdit::~LineNumberingPlainTextEdit()
{
    delete lineNumberArea;
}




void LineNumberingPlainTextEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                      lineNumberAreaWidth(), cr.height()));
}


int LineNumberingPlainTextEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, document()->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void LineNumberingPlainTextEdit::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void LineNumberingPlainTextEdit::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void LineNumberingPlainTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString lineNumberString = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.setFont(this->font());
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, lineNumberString);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}




LineNumberArea::LineNumberArea(LineNumberingPlainTextEdit *ed) : QWidget(ed)
{
    editor = ed;
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    editor->lineNumberAreaPaintEvent(event);
}

QSize LineNumberArea::sizeHint() const {
    return QSize(editor->lineNumberAreaWidth(), 0);
}
