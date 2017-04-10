#include <QIcon>
#include <QMouseEvent>

#include "ClickableLabel.h"

#include <QDebug>

ClickableLabel::ClickableLabel(QWidget *parent)
    : QLabel(parent)
    , checkable(false)
{
    setLineWidth(1);
    setAlignment(Qt::AlignCenter);
    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Raised);
}

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
    , checkable(false)
    , checked(false)
{
    setLineWidth(1);
    setAlignment(Qt::AlignCenter);
    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Raised);
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
            setFrameShape(QFrame::Panel);
            setFrameShadow(QFrame::Raised);
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
    if(e->button() == Qt::LeftButton) {
        setFrameShape(QFrame::Panel);
        setFrameShadow(QFrame::Sunken);
        if(checkable) {
            checked = !checked;
        }
    }

    QLabel::mousePressEvent(e);
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        if(!checked) {
            setFrameShape(QFrame::Panel);
            setFrameShadow(QFrame::Raised);
        }
        emit clicked(checked);
    }
    if(e->button() == Qt::RightButton) {
        emit rightClicked();
    }

    QLabel::mouseReleaseEvent(e);
}
