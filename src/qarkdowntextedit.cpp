#include "qarkdowntextedit.h"

#include <QDebug>
#include <QKeyEvent>
#include <QTextBlock>

QarkdownTextEdit::QarkdownTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
    _indentString = "    ";
}

QString QarkdownTextEdit::indentString()
{
    return _indentString;
}
void QarkdownTextEdit::setIndentString(QString value)
{
    _indentString = value;
}


bool QarkdownTextEdit::event(QEvent *e)
{
    if (e->type() != QEvent::KeyPress)
        return QTextEdit::event(e);

    QKeyEvent *ke = static_cast<QKeyEvent *>(e);
    if (ke->key() != Qt::Key_Tab)
        return QTextEdit::event(e);

    QTextCursor cursor = this->textCursor();

    if (!cursor.hasSelection()
        || cursor.hasComplexSelection())
        return QTextEdit::event(e);

    // Tab pressed with a 'normal' selection:
    bool onlyFullLinesSelected = false;

    QChar startChar = this->document()->characterAt(cursor.anchor()-1);
    QChar lastChar = this->document()->characterAt(cursor.position()-1);
    QChar afterLastChar = this->document()->characterAt(cursor.position());
    bool startsAtLineStart = (startChar.isNull()
                              || startChar.unicode() == QChar::ParagraphSeparator
                              || startChar.unicode() == QChar::LineSeparator
                              );
    bool endsAtLineEnd = (afterLastChar.isNull()
                          || lastChar.unicode() == QChar::ParagraphSeparator
                          || lastChar.unicode() == QChar::LineSeparator
                          );

    if (startsAtLineStart && endsAtLineEnd)
        onlyFullLinesSelected = true;

    if (!onlyFullLinesSelected) {
        cursor.clearSelection();
        return true;
    }

    // Only full lines are selected.
    indentSelectedLines();

    return true;
}


void QarkdownTextEdit::indentSelectedLines()
{
    QTextCursor cursor = this->textCursor();

    // Find line start positions
    QList<int> lineStarts;
    QTextCursor c(document());
    c.setPosition(cursor.selectionStart());
    QTextBlock b = c.block();
    int pos = b.position();
    while (pos < cursor.selectionEnd())
    {
        lineStarts.append(pos);
        b = b.next();
        if (!b.isValid())
            break;
        pos = b.position();
    }

    // Insert indentString to line start positions
    QTextCursor insertCursor(document());
    insertCursor.beginEditBlock();
    int shift = 0;
    foreach (int lineStart, lineStarts)
    {
        insertCursor.setPosition(lineStart+shift);
        insertCursor.insertText(_indentString);
        shift += _indentString.length();
    }
    insertCursor.endEditBlock();

    // Adjust selection to include first added indentString
    int selEnd = cursor.selectionEnd();
    cursor.setPosition(cursor.selectionStart()-_indentString.length());
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);
    this->setTextCursor(cursor);
}

