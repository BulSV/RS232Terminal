#include "LimitedItemsListWidget.h"

LimitedItemsListWidget::LimitedItemsListWidget(QWidget *parent)
    : QListWidget(parent)
{
    maxItemsCount = 1000;
    setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 10pt");
}

void LimitedItemsListWidget::addItem(const QString &str)
{
    QListWidget::addItem(str);
    if(count() >= maxItemsCount) {
        delete takeItem(0);
    }
}

void LimitedItemsListWidget::setItemsLimit(int maxCount)
{
    this->maxItemsCount = maxCount;
}

int LimitedItemsListWidget::itemsLimit()
{
    return maxItemsCount;
}

void LimitedItemsListWidget::setItemColor(int index, const QColor &color)
{
    item(index)->setBackgroundColor(color);
    item(index)->setTextColor(Qt::white);
}
