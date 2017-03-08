#include "RightClickedButton.h"

RightClickedButton::RightClickedButton(QString title, QWidget *parent)
    : QPushButton(title, parent)
{

}

void RightClickedButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton) {
        emit rightClicked();
    }
    if(e->button() == Qt::LeftButton) {
        click();
    }
}
