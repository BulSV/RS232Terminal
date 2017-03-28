#include <QMessageBox>

#include "MacrosWidget.h"

MacrosWidget::MacrosWidget(QWidget *parent)
: QWidget(parent)
, buttonDelete(new QPushButton(this))
, checkBoxInterval(new QCheckBox(this))
, checkBoxPeriod(new QCheckBox(this))
, spinBoxTime(new QSpinBox(this))
, buttonSend(new RightClickedButton(tr("Empty"), this))
, buttonUp(new QPushButton(this))
, buttonDown(new QPushButton(this))
, macrosEditWidget(new MacrosEditWidget(this))
, timerPeriod(new QTimer(this))
{
    spinBoxTime->setRange(0, 999999);
    spinBoxTime->setValue(0);
    checkBoxInterval->setEnabled(false);
    checkBoxPeriod->setEnabled(false);

    view();
    connections();
}

void MacrosWidget::saveSettings(QSettings *settings, int macrosIndex)
{
    QString macrosIndexString = QString::number(macrosIndex);
    settings->setValue("macroses/" + macrosIndexString + "/interval", intervalIsChecked());
    settings->setValue("macroses/" + macrosIndexString + "/period", periodIsChecked());
    settings->setValue("macroses/" + macrosIndexString + "time", getTime());

    macrosEditWidget->saveSettings(settings, macrosIndex);
}

void MacrosWidget::loadSettings(QSettings *settings, int macrosIndex)
{
    QString macrosIndexString = QString::number(macrosIndex);
    setCheckedInterval(settings->value("macroses/" + macrosIndexString + "/interval").toBool());
    setCheckedPeriod(settings->value("macroses/" + macrosIndexString + "/period").toBool());
    setTime(settings->value("macroses/" + macrosIndexString + "/time").toInt());

    macrosEditWidget->loadSettings(settings, macrosIndex);
}

void MacrosWidget::setCheckedInterval(bool check)
{
    checkBoxInterval->setChecked(check);
}

bool MacrosWidget::intervalIsChecked() const
{
    return checkBoxInterval->isChecked();
}

void MacrosWidget::setCheckedPeriod(bool check)
{
    checkBoxPeriod->setChecked(check);
}

bool MacrosWidget::periodIsChecked() const
{
    return checkBoxPeriod->isChecked();
}

void MacrosWidget::setEnabledInterval(bool enable)
{
    checkBoxInterval->setEnabled(enable);
}

bool MacrosWidget::intervalIsEnabled() const
{
    return checkBoxInterval->isEnabled();
}

void MacrosWidget::setEnabledPeriod(bool enable)
{
    checkBoxPeriod->setEnabled(enable);
}

bool MacrosWidget::periodIsEnabled() const
{
    return checkBoxPeriod->isEnabled();
}

void MacrosWidget::setTime(int time)
{
    spinBoxTime->setValue(time);
}

int MacrosWidget::getTime() const
{
    return spinBoxTime->value();
}

const QByteArray &MacrosWidget::getPackage() const
{
    return macrosEditWidget->getPackage();
}

void MacrosWidget::deleteMacros()
{
    int button = QMessageBox::question(this, tr("Warning"),
    tr("Delete macros ") + buttonSend->text() + " ?",
    QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes) {
        emit deleted();
    }
}

void MacrosWidget::view()
{
    buttonUp->setFixedSize(16, 13);
    buttonDown->setFixedSize(16, 13);
    buttonUp->setStyleSheet("border-image: url(:/Resources/arrow-up.png) stretch;");
    buttonDown->setStyleSheet("border-image: url(:/Resources/arrow-down.png) stretch;");

    QVBoxLayout *upDownButtonsLayout = new QVBoxLayout;
    upDownButtonsLayout->setSpacing(1);
    upDownButtonsLayout->addWidget(buttonUp);
    upDownButtonsLayout->addWidget(buttonDown);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(buttonDelete);
    mainLayout->addWidget(checkBoxInterval);
    mainLayout->addWidget(checkBoxPeriod);
    mainLayout->addWidget(spinBoxTime);
    mainLayout->addWidget(buttonSend);
    mainLayout->addLayout(upDownButtonsLayout);
    mainLayout->setSpacing(5);

    setLayout(mainLayout);

    buttonSend->setStyleSheet("font-weight: bold");
    checkBoxInterval->setFixedWidth(15);
    checkBoxPeriod->setFixedWidth(15);
    buttonSend->setFixedWidth(85);
    buttonDelete->setFixedSize(15, 15);
    buttonDelete->setStyleSheet("border-image: url(:/Resources/del.png) stretch;");
}

void MacrosWidget::connections()
{
    connect(checkBoxInterval, &QCheckBox::toggled, this, &MacrosWidget::intervalChecked);
    connect(checkBoxPeriod, &QCheckBox::toggled, this, &MacrosWidget::periodChecked);
    connect(buttonSend, &RightClickedButton::rightClicked, macrosEditWidget, &MacrosEditWidget::show);
    connect(buttonSend, &RightClickedButton::clicked, macrosEditWidget, &MacrosEditWidget::sendPeriod());
    connect(spinBoxTime, SIGNAL(valueChanged(int)), this, SLOT(timeChanged()));
    connect(buttonDelete, SIGNAL(pressed()), this, SLOT(deleteMacros()));
    connect(macrosEditWidget, SIGNAL(upd(bool, QString, int)), this, SLOT(update(bool, QString, int)));
    connect(macrosEditWidget, SIGNAL(act(bool)), this, SLOT(activate(bool)));
    connect(timerPeriod, SIGNAL(timeout()), this, SLOT(sendPeriod()));
    connect(buttonUp, &QPushButton::clicked, this, &MacrosWidget::movedUp);
    connect(buttonDown, &QPushButton::clicked, this, &MacrosWidget::movedDown);
}
