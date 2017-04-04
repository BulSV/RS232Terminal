#include <QIcon>

#include "ClickableLabel.h"

ClickableLabel::ClickableLabel(QWidget *parent)
    : QLabel(parent)
    , checkable(false)
{
    setLineWidth(1);
}

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
{
    setLineWidth(1);
}

void ClickableLabel::setCheckable(bool checkable)
{
    this->checkable = checkable;
}

void ClickableLabel::mousePressEvent(QMouseEvent *e)
{
    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Sunken);
    if(checkable) {
        emit clicked(true);
    }

    QLabel::mousePressEvent(e);
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *e)
{
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);

    emit clicked();

    QLabel::mouseReleaseEvent(e);
}

void ClickableLabel::enterEvent(QEvent *e)
{
    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Raised);

    QLabel::enterEvent(e);
}

void ClickableLabel::leaveEvent(QEvent *e)
{
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);

    QLabel::leaveEvent(e);
}

