#include <QTextBlock>

#include "LimitedTextEdit.h"

#include <QDebug>

LimitedTextEdit::LimitedTextEdit(QWidget *parent)
    : QTextEdit(parent)
    , maxLinesCount(1000)
{
    setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 10pt");
}

void LimitedTextEdit::addLine(const QString &line)
{
    if(!timeFormat.isEmpty()) {
        QString timeString;
        timeString.append("[");
        timeString .append(time.currentTime().toString(timeFormat));
        timeString.append("]:");
        QTextEdit::append(timeString);
    }
    QTextEdit::append("->" + line);
    if(document()->lineCount() >= maxLinesCount) {
        QTextBlock block = document()->begin();
        for(int i = maxLinesCount; i < linesCount(); ++i) {
            QTextCursor cursor(block);
            cursor.select(QTextCursor::BlockUnderCursor);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            block.next();
        }
    }
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
    return document()->lineCount();
}

void LimitedTextEdit::displayTime(const QString &format)
{
    timeFormat = format;
}
