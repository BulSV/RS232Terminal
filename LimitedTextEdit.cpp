#include <QTextBlock>

#include "LimitedTextEdit.h"

#include <QDebug>

LimitedTextEdit::LimitedTextEdit(QWidget *parent)
    : QTextEdit(parent)
    , maxLinesCount(1000)
{
    document()->setUndoRedoEnabled(false);
    setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 10pt");
}

void LimitedTextEdit::addLine(const QString &line)
{
    QString promtSymbols = "->";
    if(!timeFormat.isEmpty()) {
        QString timeString;
        timeString.append("[");
        timeString .append(time.currentTime().toString(timeFormat));
        timeString.append("]:");
        QTextEdit::append(timeString);
        promtSymbols.clear();
    }
    QTextEdit::append(promtSymbols + line);
}

void LimitedTextEdit::setLinesLimit(int maxLinesCount)
{
    this->maxLinesCount = maxLinesCount;
    document()->setMaximumBlockCount(maxLinesCount);
}

int LimitedTextEdit::linesLimit()
{
    return maxLinesCount;
}

int LimitedTextEdit::linesCount()
{
    return document()->lineCount();
}

void LimitedTextEdit::displayTime(const QString &format)
{
    timeFormat = format;
}
