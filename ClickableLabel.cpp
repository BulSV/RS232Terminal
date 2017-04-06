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
    , checkable(false)
    , checked(false)
{
    setLineWidth(1);
}

void ClickableLabel::setCheckable(bool checkable)
{
    this->checkable = checkable;
}

void ClickableLabel::setChecked(bool checked)
{
    if(checkable) {
        this->checked = checked;
        if(checked) {
            setFrameShape(QFrame::Panel);
            setFrameShadow(QFrame::Sunken);
        } else {
            setFrameShape(QFrame::NoFrame);
            setFrameShadow(QFrame::Plain);
        }

        emit clicked(checked);
    }
}

bool ClickableLabel::isChecked() const
{
    return checked;
}

void ClickableLabel::mousePressEvent(QMouseEvent *e)
{
    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Sunken);
    if(checkable) {
        checked = !checked;
    }

    QLabel::mousePressEvent(e);
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *e)
{
    if(!checked) {
        setFrameShape(QFrame::NoFrame);
        setFrameShadow(QFrame::Plain);
    }

    emit clicked(checked);

    QLabel::mouseReleaseEvent(e);
}

void ClickableLabel::enterEvent(QEvent *e)
{
    if(!checked) {
        setFrameShape(QFrame::Panel);
        setFrameShadow(QFrame::Raised);
    }

    QLabel::enterEvent(e);
}

void ClickableLabel::leaveEvent(QEvent *e)
{
    if(!checked) {
        setFrameShape(QFrame::NoFrame);
        setFrameShadow(QFrame::Plain);
    }

    QLabel::leaveEvent(e);
}

