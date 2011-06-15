#ifndef QARKDOWNTEXTEDIT_H
#define QARKDOWNTEXTEDIT_H

#include <QTextEdit>
#include <QEvent>

class QarkdownTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit QarkdownTextEdit(QWidget *parent = 0);
    QString indentString();
    void setIndentString(QString value);

protected:
    bool event(QEvent *e);
    QString _indentString;
    bool selectionContainsOnlyFullLines(QTextCursor selection);
    QList<int> getLineStartPositionsInSelection(QTextCursor selection);
    void indentSelectedLines();

signals:

public slots:

};

#endif // QARKDOWNTEXTEDIT_H
