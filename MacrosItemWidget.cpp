#include <QMessageBox>

#include "MacrosItemWidget.h"

MacrosItemWidget::MacrosItemWidget(QWidget *parent)
    : QWidget(parent)
    , mainLayout(new QHBoxLayout(this))
    , del(new QPushButton(this))
    , interval(new QCheckBox(this))
    , period(new QCheckBox(this))
    , time(new QSpinBox(this))
    , send(new RightClickedButton(tr("Empty"), this))
    , buttonUp(new QPushButton(this))
    , buttonDown(new QPushButton(this))
    , macrosWidget(new MacrosWidget(this))
    , tPeriod(new QTimer(this))
    , mode(HEX)
{
    time->setRange(0, 999999);
    time->setValue(0);
    interval->setEnabled(false);
    period->setEnabled(false);

    view();
    connections();
}

int MacrosItemWidget::getMode() const
{
    return mode;
}

void MacrosItemWidget::saveSettings(QSettings *settings, int macrosIndex)
{
    macrosWidget->saveSettings(settings, macrosIndex);
    settings->setValue("macros/"+QString::number(macrosIndex)+"/time", time->value());
    settings->setValue("macros/"+QString::number(macrosIndex)+"/checked_interval", interval->isChecked());
    settings->setValue("macros/"+QString::number(macrosIndex)+"/checked_period", period->isChecked());
    settings->setValue("macros/"+QString::number(macrosIndex)+"/path", m->macrosWidget->path);
}

void MacrosItemWidget::loadSettings(QSettings *settings, int macrosIndex)
{
    if(!macrosWidget->openPath(settings->value("macros/"+QString::number(macrosIndex)+"/path").toString())) {
        macrosWidget->loadSettings(settings, macrosIndex);
    }
    time->setValue(settings->value("macros/"+QString::number(macrosIndex)+"/time").toInt());
    interval->setChecked(settings->value("macros/"+QString::number(macrosIndex)+"/checked_interval").toBool());
    period->setChecked(settings->value("macros/"+QString::number(macrosIndex)+"/checked_period").toBool());
}

void MacrosItemWidget::setTimeMode(int mode)
{
    switch (mode) {
    case INTERVAL:
        interval->setChecked(true);
        period->setChecked(false);
        break;
    case PERIOD:
        interval->setChecked(false);
        period->setChecked(true);
        break;
    default:
        interval->setChecked(false);
        period->setChecked(false);
        break;
    }
    checkMacros();
}

int MacrosItemWidget::getTimeMode() const
{
    if(interval->isChecked()) {
        return INTERVAL;
    }
    if(period->isChecked()) {
        return PERIOD;
    }
    return NONE;
}

void MacrosItemWidget::setTime(int time)
{
    this->time->setValue(time);
}

void MacrosItemWidget::sendPeriod()
{
    tPeriod->setInterval(time->value());

    emit setSend(macrosWidget->package->text(), mode);
}

void MacrosItemWidget::sendPackage()
{
    emit package(macrosWidget->getPackage(), getMode());
}

void MacrosItemWidget::checkMacros()
{
    if(interval->isChecked() && period->isChecked()) {
        QCheckBox *tempCheckBox = dynamic_cast<QCheckBox*>(sender());
        if(tempCheckBox) {
            tempCheckBox->setChecked(false);
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
        mode = HEX;
    }
    if(macrosWidget->rbASCII->isChecked()) {
        mode = ASCII;
    }
    if(macrosWidget->rbDEC->isChecked()) {
        mode = DEC;
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

void MacrosItemWidget::view()
{
    buttonUp->setFixedSize(16, 13);
    buttonDown->setFixedSize(16, 13);
    buttonUp->setStyleSheet("border-image: url(:/Resources/arrow-up.png) stretch;");
    buttonDown->setStyleSheet("border-image: url(:/Resources/arrow-down.png) stretch;");

    QVBoxLayout *upDownButtonsLayout = new QVBoxLayout;
    upDownButtonsLayout->setSpacing(1);
    upDownButtonsLayout->addWidget(buttonUp);
    upDownButtonsLayout->addWidget(buttonDown);

    mainLayout->addWidget(del);
    mainLayout->addWidget(interval);
    mainLayout->addWidget(period);
    mainLayout->addWidget(time);
    mainLayout->addWidget(send);
    mainLayout->addLayout(upDownButtonsLayout);
    mainLayout->setSpacing(5);

    setLayout(mainLayout);

    send->setStyleSheet("font-weight: bold");
    interval->setFixedWidth(15);
    period->setFixedWidth(15);
    send->setFixedWidth(85);
    del->setFixedSize(15, 15);
    del->setStyleSheet("border-image: url(:/Resources/del.png) stretch;");
}

void MacrosItemWidget::connections()
{
    connect(interval, SIGNAL(toggled(bool)), this, SLOT(checkMacros()));
    connect(period, SIGNAL(toggled(bool)), this, SLOT(checkMacros()));
    connect(send, SIGNAL(rightClicked()), macrosWidget, SLOT(show()));
    connect(send, SIGNAL(clicked()), this, SLOT(sendPeriod()));
    connect(time, SIGNAL(valueChanged(int)), this, SLOT(timeChanged()));
    connect(del, SIGNAL(pressed()), this, SLOT(delMac()));
    connect(macrosWidget, SIGNAL(upd(bool, QString, int)), this, SLOT(update(bool, QString, int)));
    connect(macrosWidget, SIGNAL(act(bool)), this, SLOT(activate(bool)));
    connect(tPeriod, SIGNAL(timeout()), this, SLOT(sendPeriod()));
    connect(buttonUp, &QPushButton::clicked, this, &MacrosItemWidget::movedUp);
    connect(buttonDown, &QPushButton::clicked, this, &MacrosItemWidget::movedDown);
}
