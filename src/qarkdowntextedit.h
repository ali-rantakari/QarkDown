#ifndef QARKDOWNTEXTEDIT_H
#define QARKDOWNTEXTEDIT_H

#include <QTextEdit>
#include <QEvent>
#include <QUrl>

class QarkdownTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit QarkdownTextEdit(QWidget *parent = 0);

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

protected:
    QString _indentString;
    int _spacesIndentWidthHint;

    bool event(QEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    QString getAnchorHrefAtPos(QPoint pos);
    bool selectionContainsOnlyFullLines(QTextCursor selection);
    QList<int> getLineStartPositionsInSelection(QTextCursor selection);
    void indentSelectedLines();
    void unindentSelectedLines();

signals:
    void anchorClicked(QUrl url);

public slots:

};

#endif // QARKDOWNTEXTEDIT_H
