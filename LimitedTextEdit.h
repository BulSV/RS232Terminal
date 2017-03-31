#ifndef LIMITED_TEXT_EDIT_H
#define LIMITED_TEXT_EDIT_H

#include <QTextEdit>

class LimitedTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit LimitedTextEdit(QWidget *parent = 0);
    void addLine(const QString &line);
    void clear();
    void setLinesLimit(int maxLinesCount);
    int linesLimit();
    int linesCount();
private:
    int maxLinesCount;
    int currentLinesCount;

};

#endif // LIMITED_TEXT_EDIT_H
