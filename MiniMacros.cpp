#include "MiniMacros.h"

MiniMacros::MiniMacros(int i, QWidget *parent)
    :QWidget(parent)
    , layout(new QHBoxLayout(this))
    , del(new QPushButton(this))
    , interval(new QCheckBox("Interval", this))
    , period(new QCheckBox("Period", this))
    , time(new QSpinBox(this))
    , send(new MyPushButton("Empty", this))
    , editing(new Macros(this))
    , tPeriod(new QTimer(this))
{
    index = i;

    time->setRange(0, 999999);
    time->setValue(50);
    interval->setEnabled(false);
    period->setEnabled(false);

    layout->setMargin(3);
    layout->setSpacing(3);
    setLayout(layout);
    layout->addWidget(del);
    layout->addWidget(interval);
    layout->addWidget(period);
    layout->addWidget(time);
    layout->addWidget(send);

    send->setStyleSheet("font-weight: bold");
    interval->setFixedWidth(57);
    period->setFixedWidth(50);
    send->setFixedWidth(70);
    del->setFixedSize(15, 15);
    del->setStyleSheet("border-image: url(:/Resources/del.png) stretch;");

    connect(interval, SIGNAL(toggled(bool)), this, SLOT(intervalToggled(bool)));
    connect(period, SIGNAL(toggled(bool)), this, SLOT(periodToggled(bool)));
    connect(send, SIGNAL(rightClicked()), editing, SLOT(show()));
    connect(send, SIGNAL(clicked()), this, SLOT(sendPeriod()));
    connect(time, SIGNAL(valueChanged(int)), this, SLOT(timeChanged()));
    connect(del, SIGNAL(pressed()), this, SLOT(delMac()));
    connect(editing, SIGNAL(upd(bool, QString, int)), this, SLOT(update(bool, QString, int)));
    connect(editing, SIGNAL(act(bool)), this, SLOT(activate(bool)));
    connect(tPeriod, SIGNAL(timeout()), this, SLOT(sendPeriod()));
}

void MiniMacros::sendPeriod()
{
    tPeriod->setInterval(time->value());
    emit setSend(editing->package->text(), mode);
}

void MiniMacros::intervalToggled(bool check)
{
    period->setChecked(false);
    period->setEnabled(!check);
    editing->update(time->value());
}

void MiniMacros::periodToggled(bool check)
{
    interval->setChecked(false);
    interval->setEnabled(!check);   
    editing->update(time->value());
    if (check)
        tPeriod->start();
    else
        tPeriod->stop();
}

void MiniMacros::timeChanged()
{
    editing->update(time->value());
}

void MiniMacros::update(bool enabled, QString buttonText, int t)
{
    interval->setEnabled(enabled);
    period->setEnabled(enabled);
    send->setText(buttonText);
    time->setValue(t);
    if (editing->rbHEX->isChecked())
        mode = 0;
    if (editing->rbASCII->isChecked())
        mode = 1;
    if (editing->rbDEC->isChecked())
        mode = 2;
}

void MiniMacros::activate(bool enabled)
{
    interval->setEnabled(enabled);
    period->setEnabled(enabled);

    if (editing->rbHEX->isChecked())
        mode = 0;
    if (editing->rbASCII->isChecked())
        mode = 1;
    if (editing->rbDEC->isChecked())
        mode = 2;
}
