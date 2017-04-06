#include <QMessageBox>

#include "Macro.h"

#include <QDebug>

const int MAX_SYMBOLS_COUNT = 20;

Macro::Macro(QWidget *parent)
    : QWidget(parent)
    , buttonDelete(new ClickableLabel(this))
    , checkBoxInterval(new QCheckBox(this))
    , checkBoxPeriod(new QCheckBox(this))
    , spinBoxTime(new QSpinBox(this))
    , buttonSend(new RightClickedButton(tr("Empty"), this))
    , buttonUp(new ClickableLabel(this))
    , buttonDown(new ClickableLabel(this))
    , macroEdit(new MacroEdit(this))
    , timerPeriod(new QTimer(this))
{
    spinBoxTime->setRange(0, 999999);
    spinBoxTime->setValue(50);

    view();
    connections();
}

void Macro::saveSettings(QSettings *settings, int macroIndex)
{
    QString macroIndexString = QString::number(macroIndex);
    settings->setValue("macros/" + macroIndexString + "/interval", intervalIsChecked());
    settings->setValue("macros/" + macroIndexString + "/period", periodIsChecked());
    settings->setValue("macros/" + macroIndexString + "/time", getTime());

    macroEdit->saveSettings(settings, macroIndex);
}

void Macro::loadSettings(QSettings *settings, int macroIndex)
{
    QString macroIndexString = QString::number(macroIndex);
    setCheckedInterval(settings->value("macros/" + macroIndexString + "/interval").toBool());
    setCheckedPeriod(settings->value("macros/" + macroIndexString + "/period").toBool());
    setTime(settings->value("macros/" + macroIndexString + "/time").toInt());

    macroEdit->loadSettings(settings, macroIndex);
}

void Macro::setCheckedInterval(bool check)
{
    checkBoxInterval->setChecked(check);
}

bool Macro::intervalIsChecked() const
{
    return checkBoxInterval->isChecked();
}

void Macro::setCheckedPeriod(bool check)
{
    checkBoxPeriod->setChecked(check);
}

bool Macro::periodIsChecked() const
{
    return checkBoxPeriod->isChecked();
}

void Macro::setEnabledInterval(bool enable)
{
    checkBoxInterval->setEnabled(enable);
}

bool Macro::intervalIsEnabled() const
{
    return checkBoxInterval->isEnabled();
}

void Macro::setEnabledPeriod(bool enable)
{
    checkBoxPeriod->setEnabled(enable);
}

bool Macro::periodIsEnabled() const
{
    return checkBoxPeriod->isEnabled();
}

void Macro::setTime(int time)
{
    spinBoxTime->setValue(time);
}

int Macro::getTime() const
{
    return spinBoxTime->value();
}

const QByteArray &Macro::getPackage() const
{
    return macroEdit->getPackage();
}

void Macro::openMacroFile(const QString &fileName)
{
    macroEdit->openMacroFile(fileName);
}

void Macro::deleteMacro()
{
    int button = QMessageBox::question(this, tr("Warning"),
                                       tr("Delete macro ") + buttonSend->text() + " ?",
                                       QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes) {
        emit deleted();
    }
}

void Macro::sendPackage()
{
    emit packageSended(macroEdit->getPackage());
}

void Macro::titleChanged()
{
    int symbolsCount = buttonSend->text().size();
    if(symbolsCount > MAX_SYMBOLS_COUNT) {
        buttonSend->setText(buttonSend->text().mid(0, MAX_SYMBOLS_COUNT));
    }
}

void Macro::intervalToggled(bool toggled)
{
    if(toggled) {
        checkBoxPeriod->setChecked(false);
    }
    checkBoxInterval->setChecked(toggled);

    emit intervalChecked(toggled);
}

void Macro::periodToggled(bool toggled)
{
    if(toggled) {
        checkBoxInterval->setChecked(false);
    }
    checkBoxPeriod->setChecked(toggled);

    emit periodChecked(toggled);
}

void Macro::view()
{
    buttonDelete->setFixedWidth(25);
    checkBoxInterval->setFixedWidth(15);
    checkBoxPeriod->setFixedWidth(15);
    spinBoxTime->setFixedWidth(60);
    buttonSend->setMaximumWidth(150);
    buttonUp->setFixedSize(16, 13);
    buttonDown->setFixedSize(16, 13);

    buttonDelete->setText("<img src=':/Resources/Delete.png' width='20' height='20'/>");
    buttonDelete->setAlignment(Qt::AlignCenter);
    buttonDelete->setToolTip(tr("Delete macro"));

    checkBoxInterval->setToolTip(tr("Interval"));
    checkBoxPeriod->setToolTip(tr("Period"));

    spinBoxTime->setToolTip(tr("Time, ms"));

    buttonSend->setStyleSheet("font-weight: bold");

    buttonUp->setText("<img src=':/Resources/arrow-up.png' width='14' height='11'/>");
    buttonUp->setAlignment(Qt::AlignHCenter);
    buttonUp->setToolTip(tr("Move up"));

    buttonDown->setText("<img src=':/Resources/arrow-down.png' width='14' height='11'/>");
    buttonDown->setAlignment(Qt::AlignHCenter);
    buttonDown->setToolTip(tr("Move down"));

    QVBoxLayout *upDownButtonsLayout = new QVBoxLayout;
    upDownButtonsLayout->setSpacing(0);
    upDownButtonsLayout->setContentsMargins(0, 0, 0, 0);
    upDownButtonsLayout->addWidget(buttonUp, 0, Qt::AlignVCenter);
    upDownButtonsLayout->addWidget(buttonDown, 0, Qt::AlignVCenter);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(buttonDelete);
    mainLayout->addWidget(checkBoxInterval);
    mainLayout->addWidget(checkBoxPeriod);
    mainLayout->addWidget(spinBoxTime);
    mainLayout->addWidget(buttonSend);
    mainLayout->addLayout(upDownButtonsLayout);
    mainLayout->setSpacing(5);

    setLayout(mainLayout);
    layout()->setSpacing(2);
    layout()->setContentsMargins(0, 0, 0, 0);
}

void Macro::connections()
{
    connect(checkBoxInterval, &QCheckBox::toggled, this, &Macro::intervalToggled);
    connect(checkBoxPeriod, &QCheckBox::toggled, this, &Macro::periodToggled);
    connect(buttonSend, &RightClickedButton::rightClicked, macroEdit, &MacroEdit::show);
    connect(buttonSend, &RightClickedButton::clicked, this, &Macro::sendPackage);
    connect(spinBoxTime, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Macro::sendTimeChanged);
    connect(buttonDelete, &ClickableLabel::clicked, this, &Macro::deleteMacro);
    connect(timerPeriod, &QTimer::timeout, this, &Macro::sendPackage);
    connect(buttonUp, &ClickableLabel::clicked, this, &Macro::movedUp);
    connect(buttonDown, &ClickableLabel::clicked, this, &Macro::movedDown);
    connect(macroEdit, &MacroEdit::titleChanged, buttonSend, &RightClickedButton::setText);
    connect(macroEdit, &MacroEdit::titleChanged, this, &Macro::titleChanged);
}
