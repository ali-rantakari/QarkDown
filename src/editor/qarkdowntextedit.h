#ifndef QARKDOWNTEXTEDIT_H
#define QARKDOWNTEXTEDIT_H

#include <QPlainTextEdit>
#include <QEvent>
#include <QUrl>

#include "linenumberingplaintextedit.h"

class QarkdownTextEdit : public LineNumberingPlainTextEdit
{
    Q_OBJECT
public:
    explicit QarkdownTextEdit(QWidget *parent = 0);
    ~QarkdownTextEdit();

    QString indentString();
    void setIndentString(QString value);

    /** @brief How many spaces to consider an indentation.
      *
      * A "hint" for determining how many spaces to delete upon
      * unindenting a line that starts with spaces. The value 0 denotes
      * "no hint".
      */
    int spacesIndentWidthHint();
    void setSpacesIndentWidthHint(int value);

    /** @brief Keyboard modifiers required for clicking on anchors. */
    Qt::KeyboardModifiers anchorClickKeyboardModifiers();
    void setAnchorClickKeyboardModifiers(Qt::KeyboardModifiers value);

    bool highlightCurrentLine();
    void setHighlightCurrentLine(bool value);

    QColor currentLineHighlightColor();
    void setCurrentLineHighlightColor(QColor value);

protected:
    QString _indentString;
    int _spacesIndentWidthHint;
    Qt::KeyboardModifiers _anchorClickKeyModifiers;
    bool _highlightCurrentLine;
    QColor _lineHighlightColor;

    bool event(QEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    QString getAnchorHrefAtPos(QPoint pos);
    bool isBorderChar(QChar character);
    bool cursorIsBeforeLineContentStart(QTextCursor cursor);
    bool selectionContainsOnlyFullLines(QTextCursor selection);
    QList<int> getLineStartPositionsInSelection(QTextCursor selection);
    int guessNumOfSpacesToDeleteUponUnindenting();
    void indentSelectedLines();
    void unindentSelectedLines();
    void indentAtCursor();
    void unindentAtCursor();

signals:
    void anchorClicked(QUrl url);

private slots:
    void applyHighlightingToCurrentLine();
    void removeCurrentLineHighlighting();
};


#endif // QARKDOWNTEXTEDIT_H
