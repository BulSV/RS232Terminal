#ifndef MINI_MACROS_H
#define MINI_MACROS_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QTimer>
#include "RightClickedButton.h"
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
    RightClickedButton *send;
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
    void sendMoveUp();
    void sendMoveDown();

signals:
    void deleteSignal(int);
    void setSend(QString, int);
    void setIntervalSend(int, bool);
    void moveUp(int);
    void moveDown(int);
};

#endif // MINI_MACROS_H
