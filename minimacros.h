#ifndef MINIMACROS_H
#define MINIMACROS_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
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
    int index;

public slots:
    void intervalToggled(bool);
    void periodToggled(bool);
    void delMac() { emit deleteSignal(index); }

signals:
    void deleteSignal(int);
};

#endif // MINIMACROS_H
