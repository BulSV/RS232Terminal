#ifndef MACROS_ITEM_WIDGET_H
#define MACROS_ITEM_WIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QTimer>
#include "RightClickedButton.h"
#include "MacrosWidget.h"

class MacrosItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MacrosItemWidget(int i, QWidget *parent = 0);

    QHBoxLayout *layout;
    QPushButton *del;
    QCheckBox *interval;
    QCheckBox *period;
    QSpinBox *time;
    RightClickedButton *send;
    MacrosWidget *macrosWidget;
    QTimer *tPeriod;
    unsigned short int index;
    unsigned short int mode;

public slots:
    void timeChanged();
    void update(bool enabled, QString buttonText, int t);
    void activate(bool enabled);
    void delMac();
    void sendPeriod();
    void sendMoveUp();
    void sendMoveDown();

signals:
    void deleteSignal(int);
    void setSend(QString, int);
    void setIntervalSend(int, bool);
    void moveUp(int);
    void moveDown(int);
private slots:
    void checkMacros();
};

#endif // MACROS_ITEM_WIDGET_H
