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
    enum TimeMode
    {
        NONE = 0,
        INTERVAL = 1,
        PERIOD = 2
    };

    explicit MacrosItemWidget(QWidget *parent = 0);

    int getMode() const;

    void saveSettings(QSettings *settings, int macrosIndex);
    void loadSettings(QSettings *settings, int macrosIndex);
    void setTimeMode(int mode);
    int getTimeMode() const;
    void setTime(int time);

public slots:
    void timeChanged();
    void update(bool enabled, QString buttonText, int t);
    void activate(bool enabled);
    void delMac();
    void sendPeriod();
    void sendPackage();
signals:
    void deleteSignal(int);
    void setSend(QString, int);
    void setIntervalSend(int, bool);
    void movedUp();
    void movedDown();
    void package(const QString& package, int mode);
private slots:
    void checkMacros();
private:
    QHBoxLayout *mainLayout;
    QPushButton *del;
    QCheckBox *interval;
    QCheckBox *period;
    QSpinBox *time;
    RightClickedButton *send;
    QPushButton *buttonUp;
    QPushButton *buttonDown;
    MacrosWidget *macrosWidget;
    QTimer *tPeriod;

    int mode;

    void view();
    void connections();
};

#endif // MACROS_ITEM_WIDGET_H
