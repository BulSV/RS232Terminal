#include "LimitedTextEdit.h"

LimitedTextEdit::LimitedTextEdit(QWidget *parent)
    : QTextEdit(parent)
    , maxLinesCount(1000)
    , currentLinesCount(0)
{
    setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 10pt");
}

void LimitedTextEdit::addLine(const QString &line)
{
    QTextEdit::append("->" + line + "\n");
    ++currentLinesCount;
    if(currentLinesCount >= maxLinesCount) {
        --currentLinesCount;
    }
}

void LimitedTextEdit::clear()
{
    QTextEdit::clear();
    currentLinesCount = 0;
}

void LimitedTextEdit::setLinesLimit(int maxLinesCount)
{
    this->maxLinesCount = maxLinesCount;
}

int LimitedTextEdit::linesLimit()
{
    return maxLinesCount;
}

int LimitedTextEdit::linesCount()
{
    return currentLinesCount;
}
