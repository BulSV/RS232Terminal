#include <QMessageBox>

#include "Macro.h"

#include <QDebug>

const int MAX_SYMBOLS_COUNT = 20;

Macro::Macro(QWidget *parent)
    : QWidget(parent)
    , buttonDelete(new ClickableLabel(this))
    , checkBoxSelect(new QCheckBox(this))
    , spinBoxTime(new QSpinBox(this))
    , buttonSend(new RightClickedButton(tr("Empty"), this))
    , buttonUp(new ClickableLabel(this))
    , buttonDown(new ClickableLabel(this))
    , macroEdit(new MacroEdit(this))
    , timerPeriod(new QTimer(this))
    , isSelected(false)
{
    spinBoxTime->setRange(0, 999999);
    spinBoxTime->setValue(50);

    buttonSend->setCheckable(true);

    view();
    connections();
}

void Macro::saveSettings(QSettings *settings, int macroIndex)
{
    QString macroIndexString = QString::number(macroIndex);
    settings->setValue("macros/" + macroIndexString + "/selected", selectState());
    settings->setValue("macros/" + macroIndexString + "/time", getTime());

    macroEdit->saveSettings(settings, macroIndex);
}

void Macro::loadSettings(QSettings *settings, int macroIndex)
{
    QString macroIndexString = QString::number(macroIndex);
    settings->value("macros/" + macroIndexString + "/selected", false).toBool() ? select() : deselect();
    setTime(settings->value("macros/" + macroIndexString + "/time").toInt());

    macroEdit->loadSettings(settings, macroIndex);
}

void Macro::select()
{
    isSelected = true;
    checkBoxSelect->setChecked(isSelected);

}

void Macro::deselect()
{
    isSelected = false;
    checkBoxSelect->setChecked(isSelected);
}

void Macro::selectToggle()
{
    isSelected = !isSelected;
    checkBoxSelect->setChecked(isSelected);

    emit toggled();
}

bool Macro::selectState() const
{
    return checkBoxSelect->isChecked();
}

void Macro::enableSelectState(bool enable)
{
    checkBoxSelect->setEnabled(enable);
}

bool Macro::isEnabledSelectState() const
{
    return checkBoxSelect->isEnabled();
}

void Macro::setTime(int time)
{
    if(time == 0) {
        deselect();
        enableSelectState(false);
    } else {
        enableSelectState(true);
    }
    spinBoxTime->setValue(time);

    emit timeChanged(time);
}

int Macro::getTime() const
{
    return spinBoxTime->value();
}

const QByteArray &Macro::getPacket() const
{
    return macroEdit->getPackage();
}

void Macro::openMacroFile(const QString &fileName)
{
    macroEdit->openMacroFile(fileName);
}

void Macro::stopSend()
{
    timerPeriod->stop();
    buttonSend->setChecked(false);
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

void Macro::singleSend()
{
    emit packetSended(macroEdit->getPackage());
    if(isSelected ||spinBoxTime->value() == 0 ) {
        stopSend();

        return;
    }
    timerPeriod->start(spinBoxTime->value());
}

void Macro::sendPacket(bool checked)
{
    if(!checked) {
        timerPeriod->stop();

        return;
    }
    if(spinBoxTime->value() != 0 && !isSelected) {
        timerPeriod->start(spinBoxTime->value());

        return;
    }
    buttonSend->setChecked(false);
    singleSend();
}

void Macro::titleChanged()
{
    int symbolsCount = buttonSend->text().size();
    if(symbolsCount > MAX_SYMBOLS_COUNT) {
        buttonSend->setText(buttonSend->text().mid(0, MAX_SYMBOLS_COUNT));
    }
}

void Macro::selectTrigger()
{
    isSelected = !isSelected;
    checkBoxSelect->setChecked(isSelected);

    emit selected(isSelected);
}

void Macro::view()
{
    buttonDelete->setFixedWidth(25);
    checkBoxSelect->setFixedWidth(15);
    spinBoxTime->setFixedWidth(60);
    buttonSend->setMaximumWidth(150);
    buttonUp->setFixedSize(16, 13);
    buttonDown->setFixedSize(16, 13);

    buttonDelete->setText("<img src=':/Resources/Delete.png' width='20' height='20'/>");
    buttonDelete->setAlignment(Qt::AlignCenter);
    buttonDelete->setToolTip(tr("Delete macro"));

    checkBoxSelect->setToolTip(tr("Select macro"));

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
    mainLayout->addWidget(checkBoxSelect);
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
    connect(checkBoxSelect, &QCheckBox::clicked, this, &Macro::selectTrigger);
    connect(buttonSend, &RightClickedButton::rightClicked, macroEdit, &MacroEdit::show);
    connect(buttonSend, &RightClickedButton::clicked, this, &Macro::sendPacket);
    connect(timerPeriod, &QTimer::timeout, this, &Macro::singleSend);
    connect(spinBoxTime, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Macro::setTime);
    connect(buttonDelete, &ClickableLabel::clicked, this, &Macro::deleteMacro);
    connect(buttonUp, &ClickableLabel::clicked, this, &Macro::movedUp);
    connect(buttonDown, &ClickableLabel::clicked, this, &Macro::movedDown);
    connect(macroEdit, &MacroEdit::titleChanged, buttonSend, &RightClickedButton::setText);
    connect(macroEdit, &MacroEdit::titleChanged, this, &Macro::titleChanged);
}
