#ifndef LIMITED_ITEMS_LIST_WIDGET_H
#define LIMITED_ITEMS_LIST_WIDGET_H

#include <QListWidget>
#include <QColor>

class LimitedItemsListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit LimitedItemsListWidget(QWidget *parent = 0);
    void addItem(const QString &str);
    void setItemsLimit(int maxItemsCount);
    int itemsLimit();
    void setItemColor(int index, const QColor &color);
private:
    int maxItemsCount;

};

#endif // LIMITED_ITEMS_LIST_WIDGET_H
