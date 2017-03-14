#ifndef MACROS_ITEM_WIDGET_H
#define MACROS_ITEM_WIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QTimer>
#include <QSettings>

#include "RightClickedButton.h"
#include "MacrosWidget.h"

class MacrosItemWidget : public QWidget
{
    Q_OBJECT
public:
    enum DataMode
    {
        HEX = 0,
        ASCII = 1,
        DEC = 2
    };
    explicit MacrosItemWidget(int i, QWidget *parent = 0);

    QCheckBox *interval;
    QCheckBox *period;
    QSpinBox *time;
    RightClickedButton *send;
    QPushButton *buttonUp;
    QPushButton *buttonDown;
    MacrosWidget *macrosWidget;
    QTimer *tPeriod;
    int index;

    int getMode() const;

    void setSettings(QSettings *settings, int index);

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
    void movedUp();
    void movedDown();
private slots:
    void checkMacros();
private:
    QHBoxLayout *mainLayout;
    QPushButton *del;

    int mode;
    QSettings *settings;

    void view();
    void connections();
};

#endif // MACROS_ITEM_WIDGET_H
