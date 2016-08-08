#ifndef MINIMACROS_H
#define MINIMACROS_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QTimer>
#include "MyPushButton.h"
#include "Macros.h"

class MiniMacros : public QWidget
{
    Q_OBJECT
public:
    explicit MiniMacros(int i, QWidget *parent = 0);

    QHBoxLayout *layout;
    QPushButton *del;
    QCheckBox *interval;
    QCheckBox *period;
    QSpinBox *time;
    MyPushButton *send;
    Macros *editing;
    QTimer *tPeriod;
    unsigned short int index;
    unsigned short int mode;

public slots:
    void intervalToggled(bool);
    void periodToggled(bool);
    void timeChanged();
    void update(bool enabled, QString buttonText, int t);
    void activate(bool enabled);
    void delMac();
    void sendPeriod();

signals:
    void deleteSignal(int);
    void setSend(QString, int);
    void setIntervalSend(int, bool);
};

#endif // MINIMACROS_H
