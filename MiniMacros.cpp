#include "MiniMacros.h"

MiniMacros::MiniMacros(int i, QWidget *parent)
    :QWidget(parent)
    , layout(new QHBoxLayout(this))
    , del(new QPushButton(this))
    , interval(new QCheckBox("Interval", this))
    , period(new QCheckBox("Period", this))
    , time(new QSpinBox(this))
    , send(new MyPushButton("Empty", this))
    , editing(new Macros)
{
    index = i;
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
    send->setFixedWidth(50);
    del->setFixedSize(15, 15);
    del->setStyleSheet("border-image: url(:/Resources/del.png) stretch;");

    connect(interval, SIGNAL(toggled(bool)), this, SLOT(intervalToggled(bool)));
    connect(period, SIGNAL(toggled(bool)), this, SLOT(periodToggled(bool)));
    connect(send, SIGNAL(rightClicked()), editing, SLOT(show()));
    connect(del, SIGNAL(pressed()), this, SLOT(delMac()));
}

void MiniMacros::intervalToggled(bool check)
{
    period->setChecked(false);
    period->setEnabled(!check);
}

void MiniMacros::periodToggled(bool check)
{
    interval->setChecked(false);
    interval->setEnabled(!check);
}
