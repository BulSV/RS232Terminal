#ifndef MACROS_H
#define MACROS_H

#include <QObject>
#include <QSettings>

#include "MacrosEditWidget.h"
#include "MacrosWidget.h"

class Macros : public QObject
{
    Q_OBJECT
public:
    enum TimeMode
    {
        NONE = 0,
        INTERVAL = 1,
        PERIOD = 2
    };
    explicit Macros(QObject *parent = 0);
    virtual ~Macros();
    QWidget *getMacrosWidget() const;
    void saveSettings(QSettings *settings, int macrosIndex);
    void loadSettings(QSettings *settings, int macrosIndex);
    void setTimeMode(TimeMode timeMode);
    TimeMode getTimeMode() const;
    int getSendTime() const;
signals:
    void sendPackage(const QByteArray &package);
    void movedUp();
    void movedDown();
    void sendTimeChanged(int time);
    void packageChanged(const QByteArray &package);
    void timeModeChanged(MacrosWidget::TimeMode timeMode);
private:
    MacrosWidget *macrosWidget;
    MacrosEditWidget *macrosEditWidget;
    TimeMode timeMode;
};

#endif // MACROS_H
