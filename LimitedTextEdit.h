#ifndef LIMITED_TEXT_EDIT_H
#define LIMITED_TEXT_EDIT_H

#include <QTextEdit>
#include <QTime>

class LimitedTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit LimitedTextEdit(QWidget *parent = 0);
    void addLine(const QString &line);
    void setLinesLimit(int maxLinesCount);
    int linesLimit();
    int linesCount();
    void displayTime(const QString &format);
private:
    int maxLinesCount;
    QString timeFormat;
    QTime time;

};

#endif // LIMITED_TEXT_EDIT_H
