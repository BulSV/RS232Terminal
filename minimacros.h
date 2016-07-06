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
    int index;
    int mode;

public slots:
    void intervalToggled(bool);
    void periodToggled(bool);
    void timeChanged();
    void update(bool enabled, QString buttonText, int t);
    void activate(bool enabled);
    void delMac() { emit deleteSignal(index); }
    void sendPeriod();

signals:
    void deleteSignal(int);
    void setSend(QString, int);
};

#endif // MINIMACROS_H
