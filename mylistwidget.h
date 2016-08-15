#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H

#include <QListWidget>

class MyListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit MyListWidget(QWidget *parent = 0);
    void addPackage(const QString &str);
    void setMaxCount(int maxCount);
    int getMaxCount() { return maxCount; }
    void setItemColor(int index, Qt::GlobalColor color);

private:
    int maxCount;

};

#endif // MYLISTWIDGET_H
