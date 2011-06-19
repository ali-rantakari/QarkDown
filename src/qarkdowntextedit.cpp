#include "qarkdowntextedit.h"

#include <QDebug>
#include <QKeyEvent>
#include <QTextBlock>

QarkdownTextEdit::QarkdownTextEdit(QWidget *parent) :
    QTextBrowser(parent)
{
    this->setReadOnly(false);

    // Don't open links by yourself; emit the anchorClicked() signal
    // instead. Make links clickable.
    this->setOpenLinks(false);
    this->setTextInteractionFlags(this->textInteractionFlags()
                                  | Qt::LinksAccessibleByMouse);

    _indentString = "    ";
    _spacesIndentWidthHint = 4;
}

QString QarkdownTextEdit::indentString()
{
    return _indentString;
}
void QarkdownTextEdit::setIndentString(QString value)
{
    _indentString = value;
}

int QarkdownTextEdit::spacesIndentWidthHint()
{
    return _spacesIndentWidthHint;
}
void QarkdownTextEdit::setSpacesIndentWidthHint(int value)
{
    _spacesIndentWidthHint = value;
}


bool QarkdownTextEdit::selectionContainsOnlyFullLines(QTextCursor selection)
{
    QChar startChar = this->document()->characterAt(selection.anchor()-1);
    QChar lastChar = this->document()->characterAt(selection.position()-1);
    QChar afterLastChar = this->document()->characterAt(selection.position());
    bool startsAtLineStart = (startChar.isNull()
                              || startChar.unicode() == QChar::ParagraphSeparator
                              || startChar.unicode() == QChar::LineSeparator
                              );
    bool endsAtLineEnd = (afterLastChar.isNull()
                          || lastChar.unicode() == QChar::ParagraphSeparator
                          || lastChar.unicode() == QChar::LineSeparator
                          );
    return (startsAtLineStart && endsAtLineEnd);
}

QList<int> QarkdownTextEdit::getLineStartPositionsInSelection(QTextCursor selection)
{
    QList<int> lineStarts;
    QTextCursor c(document());
    c.setPosition(selection.selectionStart());
    QTextBlock b = c.block();
    int pos = b.position();
    while (pos < selection.selectionEnd())
    {
        lineStarts.append(pos);
        b = b.next();
        if (!b.isValid())
            break;
        pos = b.position();
    }
    return lineStarts;
}


bool QarkdownTextEdit::event(QEvent *e)
{
    if (e->type() != QEvent::KeyPress)
        return QTextEdit::event(e);

    QKeyEvent *ke = static_cast<QKeyEvent *>(e);
    if (ke->key() != Qt::Key_Tab
        && ke->key() != Qt::Key_Backtab)
        return QTextEdit::event(e);

    QTextCursor cursor = this->textCursor();

    if (!cursor.hasSelection()
        || cursor.hasComplexSelection())
        return QTextEdit::event(e);

    if (!selectionContainsOnlyFullLines(cursor)) {
        cursor.clearSelection();
        setTextCursor(cursor);
        return true;
    }

    if (ke->key() == Qt::Key_Tab)
        indentSelectedLines();
    else // backtab
        unindentSelectedLines();
    return true;
}


void QarkdownTextEdit::indentSelectedLines()
{
    QTextCursor cursor = this->textCursor();

    QList<int> lineStarts = getLineStartPositionsInSelection(cursor);

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

void QarkdownTextEdit::unindentSelectedLines()
{
    QTextCursor cursor = this->textCursor();

    QList<int> lineStarts = getLineStartPositionsInSelection(cursor);

    QTextCursor insertCursor(document());
    insertCursor.beginEditBlock();
    int deletedChars = 0;
    foreach (int lineStart, lineStarts)
    {
        int adjustedStart = lineStart - deletedChars;
        insertCursor.setPosition(adjustedStart);
        if (document()->characterAt(adjustedStart) == QChar('\t'))
        {
            // line starts with tab -> just delete the tab.
            insertCursor.deleteChar();
            deletedChars++;
        }
        else if (document()->characterAt(adjustedStart) == QChar(' '))
        {
            // line starts with a space -> must guess how many spaces to delete.
            int spacesToDelete = _spacesIndentWidthHint;
            if (spacesToDelete == 0 && _indentString.startsWith(" "))
                spacesToDelete = _indentString.length();

            while (spacesToDelete > 0 && document()->characterAt(adjustedStart) == QChar(' '))
            {
                insertCursor.deleteChar();
                deletedChars++;
                spacesToDelete--;
            }
        }
    }
    insertCursor.endEditBlock();
}

