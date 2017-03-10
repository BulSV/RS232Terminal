#include <QMessageBox>

#include "MacrosItemWidget.h"

MacrosItemWidget::MacrosItemWidget(int i, QWidget *parent)
    :QWidget(parent)
    , layout(new QHBoxLayout(this))
    , del(new QPushButton(this))
    , interval(new QCheckBox(this))
    , period(new QCheckBox(this))
    , time(new QSpinBox(this))
    , send(new RightClickedButton(tr("Empty"), this))
    , macrosWidget(new MacrosWidget(this))
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

    connect(interval, SIGNAL(toggled(bool)), this, SLOT(checkMacros()));
    connect(period, SIGNAL(toggled(bool)), this, SLOT(checkMacros()));
    connect(send, SIGNAL(rightClicked()), macrosWidget, SLOT(show()));
    connect(send, SIGNAL(clicked()), this, SLOT(sendPeriod()));
    connect(time, SIGNAL(valueChanged(int)), this, SLOT(timeChanged()));
    connect(del, SIGNAL(pressed()), this, SLOT(delMac()));
    connect(macrosWidget, SIGNAL(upd(bool, QString, int)), this, SLOT(update(bool, QString, int)));
    connect(macrosWidget, SIGNAL(act(bool)), this, SLOT(activate(bool)));
    connect(tPeriod, SIGNAL(timeout()), this, SLOT(sendPeriod()));
    connect(buttonUp, SIGNAL(clicked(bool)), this, SLOT(sendMoveUp()));
    connect(buttonDown, SIGNAL(clicked(bool)), this, SLOT(sendMoveDown()));
}

void MacrosItemWidget::sendPeriod()
{
    tPeriod->setInterval(time->value());

    emit setSend(macrosWidget->package->text(), mode);
}

void MacrosItemWidget::sendMoveUp()
{
    emit moveUp(index);
}

void MacrosItemWidget::sendMoveDown()
{
    emit moveDown(index);
}

void MacrosItemWidget::checkMacros()
{
    if(interval->isChecked() && period->isChecked()) {
        if(dynamic_cast<QCheckBox*>(sender())) {
            dynamic_cast<QCheckBox*>(sender())->setChecked(false);
        }

        return;
    }

    emit setIntervalSend(index, interval->isChecked());

    macrosWidget->update(time->value());

    if(period->isChecked()) {
        tPeriod->start();
    } else {
        tPeriod->stop();
    }
}

void MacrosItemWidget::timeChanged()
{
    macrosWidget->update(time->value());
}

void MacrosItemWidget::update(bool enabled, QString buttonText, int t)
{
    send->setText(buttonText);
    time->setValue(t);
    activate(enabled);
}

void MacrosItemWidget::activate(bool enabled)
{
    interval->setEnabled(enabled);
    period->setEnabled(enabled);

    if(macrosWidget->rbHEX->isChecked()) {
        mode = 0;
    }
    if(macrosWidget->rbASCII->isChecked()) {
        mode = 1;
    }
    if(macrosWidget->rbDEC->isChecked()) {
        mode = 2;
    }
}

void MacrosItemWidget::delMac()
{
    int button = QMessageBox::question(this, tr("Warning"),
                                       tr("Delete macros ") + send->text() + " ?",
                                       QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes) {
        emit deleteSignal(index);
    }
}
