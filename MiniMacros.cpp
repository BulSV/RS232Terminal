#include "MiniMacros.h"
#include <QMessageBox>

MiniMacros::MiniMacros(int i, QWidget *parent)
    :QWidget(parent)
    , layout(new QHBoxLayout(this))
    , del(new QPushButton(this))
    , interval(new QCheckBox(this))
    , period(new QCheckBox(this))
    , time(new QSpinBox(this))
    , send(new RightClickedButton(tr("Empty"), this))
    , editing(new Macros(this))
    , tPeriod(new QTimer(this))
{
    index = i;

    time->setRange(0, 999999);
    time->setValue(50);
    interval->setEnabled(false);
    period->setEnabled(false);

    QPushButton *buttonUp = new QPushButton(this);
    QPushButton *buttonDown = new QPushButton(this);
    buttonUp->setFixedSize(16, 13);
    buttonDown->setFixedSize(16, 13);
    buttonUp->setStyleSheet("border-image: url(:/Resources/arrow-up.png) stretch;");
    buttonDown->setStyleSheet("border-image: url(:/Resources/arrow-down.png) stretch;");

    QVBoxLayout *lay = new QVBoxLayout();
    lay->setSpacing(1);
    lay->addWidget(buttonUp);
    lay->addWidget(buttonDown);

    layout->addWidget(del);
    layout->addWidget(interval);
    layout->addWidget(period);
    layout->addWidget(time);
    layout->addWidget(send);
    layout->addLayout(lay);
    layout->setSpacing(5);

    setLayout(layout);

    send->setStyleSheet("font-weight: bold");
    interval->setFixedWidth(15);
    period->setFixedWidth(15);
    send->setFixedWidth(85);
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
    connect(buttonUp, SIGNAL(clicked(bool)), this, SLOT(sendMoveUp()));
    connect(buttonDown, SIGNAL(clicked(bool)), this, SLOT(sendMoveDown()));
}

void MiniMacros::sendPeriod()
{
    tPeriod->setInterval(time->value());

    emit setSend(editing->package->text(), mode);
}

void MiniMacros::sendMoveUp()
{
    emit moveUp(index);
}

void MiniMacros::sendMoveDown()
{
    emit moveDown(index);
}

void MiniMacros::intervalToggled(bool check)
{
    emit setIntervalSend(index, check);

    period->setChecked(false);
    period->setEnabled(!check);
    editing->update(time->value());
}

void MiniMacros::periodToggled(bool check)
{
    interval->setChecked(false);
    interval->setEnabled(!check);
    editing->update(time->value());
    if(check) {
        tPeriod->start();
    } else {
        tPeriod->stop();
    }
}

void MiniMacros::timeChanged()
{
    editing->update(time->value());
}

void MiniMacros::update(bool enabled, QString buttonText, int t)
{
    send->setText(buttonText);
    time->setValue(t);
    activate(enabled);
}

void MiniMacros::activate(bool enabled)
{
    interval->setEnabled(enabled);
    period->setEnabled(enabled);

    if(editing->rbHEX->isChecked()) {
        mode = 0;
    }
    if(editing->rbASCII->isChecked()) {
        mode = 1;
    }
    if(editing->rbDEC->isChecked()) {
        mode = 2;
    }
}

void MiniMacros::delMac()
{
    int button = QMessageBox::question(this, tr("Warning"),
                                       tr("Delete macros ") + send->text() + " ?",
                                       QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes) {
        emit deleteSignal(index);
    }
}
