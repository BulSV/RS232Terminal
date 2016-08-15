#include "mylistwidget.h"

MyListWidget::MyListWidget(QWidget *parent) :
    QListWidget(parent)
{
    maxCount = 1000;
    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //setUniformItemSizes(true);
    setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 10pt");
}

void MyListWidget::addPackage(const QString &str)
{
    addItem(str);
    if (count() >= maxCount)
        delete takeItem(0);
}

void MyListWidget::setMaxCount(int maxCount)
{
    this->maxCount = maxCount;
}

void MyListWidget::setItemColor(int index, Qt::GlobalColor color)
{
    item(index)->setBackgroundColor(color);
    item(index)->setTextColor(Qt::white);
}
