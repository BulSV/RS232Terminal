#include <QMessageBox>

#include "MacrosItemWidget.h"

MacrosItemWidget::MacrosItemWidget(int i, QWidget *parent)
    :QWidget(parent)
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
{
    index = i;

    time->setRange(0, 999999);
    time->setValue(50);
    interval->setEnabled(false);
    period->setEnabled(false);

    view();
    connections();
}

int MacrosItemWidget::getMode() const
{
    return mode;
}

void MacrosItemWidget::sendPeriod()
{
    tPeriod->setInterval(time->value());

    emit setSend(macrosWidget->package->text(), mode);
}

void MacrosItemWidget::sendMoveUp()
{
    emit movedUp();
}

void MacrosItemWidget::sendMoveDown()
{
    emit movedDown();
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

void MacrosItemWidget::setSettings(QSettings *settings, int index)
{
    this->settings = settings;
    if(!macrosWidget->openPath(settings->value("macros/"+QString::number(index)+"/path").toString())) {
        QString mode = settings->value("macros/"+QString::number(index)+"/mode").toString();
        if(mode == "HEX") {
            macrosWidget->rbHEX->setChecked(true);
        }
        if(mode == "DEC") {
            macrosWidget->rbDEC->setChecked(true);
        }
        if(mode == "ASCII") {
            macrosWidget->rbASCII->setChecked(true);
        }
        macrosWidget->package->setText(settings->value("macros/"+QString::number(index)+"/packege").toString());
        time->setValue(settings->value("macros/"+QString::number(index)+"/interval").toInt());
    }
    interval->setChecked(settings->value("macros/"+QString::number(index)+"/checked_interval").toBool());
    period->setChecked(settings->value("macros/"+QString::number(index)+"/checked_period").toBool());
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
    connect(buttonUp, SIGNAL(clicked(bool)), this, SLOT(sendMoveUp()));
    connect(buttonDown, SIGNAL(clicked(bool)), this, SLOT(sendMoveDown()));
}
