#include <QMessageBox>
#include <QtMath>

#include "Macro.h"

#include <QDebug>

const int MAX_SYMBOLS_COUNT = 20;

Macro::Macro(QWidget *parent)
    : QWidget(parent)
    , buttonDelete(new ClickableLabel(this))
    , checkBoxSelect(new QCheckBox(this))
    , spinBoxTime(new QSpinBox(this))
    , buttonSend(new ClickableLabel(tr("No name"), this))
    , buttonUp(new ClickableLabel(this))
    , buttonDown(new ClickableLabel(this))
    , macroEdit(new MacroEdit(this))
{
    spinBoxTime->setRange(1, 60000);
    spinBoxTime->setValue(50);

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

void Macro::setPacketTimeCalculator(PacketTimeCalculator *packetTimeCalculator)
{
    this->packetTimeCalculator = packetTimeCalculator;
}

void Macro::select()
{
    checkBoxSelect->setChecked(true);

    emit selected(true);

}

void Macro::deselect()
{
    checkBoxSelect->setChecked(false);

    emit selected(false);
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
    int minimumTime = 1;
    if(packetTimeCalculator != 0 && packetTimeCalculator->isValid()) {
        minimumTime = qCeil(packetTimeCalculator->calculateTime(getPacket().size()));
        if(time < minimumTime) {
            time = minimumTime;
        }
    }
    spinBoxTime->setMinimum(minimumTime);
    spinBoxTime->setValue(time);

    emit timeChanged(time);
}

int Macro::getTime() const
{
    return spinBoxTime->value();
}

const QByteArray &Macro::getPacket() const
{
    return macroEdit->getPacket();
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

void Macro::sendPacket()
{
    emit packetSended(macroEdit->getPacket());
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
    emit selected(selectState());
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
    connect(buttonSend, &ClickableLabel::rightClicked, macroEdit, &MacroEdit::show);
    connect(buttonSend, &ClickableLabel::clicked, this, &Macro::sendPacket);
    connect(spinBoxTime, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Macro::setTime);
    connect(buttonDelete, &ClickableLabel::clicked, this, &Macro::deleteMacro);
    connect(buttonUp, &ClickableLabel::clicked, this, &Macro::movedUp);
    connect(buttonDown, &ClickableLabel::clicked, this, &Macro::movedDown);
    connect(macroEdit, &MacroEdit::titleChanged, buttonSend, &ClickableLabel::setText);
    connect(macroEdit, &MacroEdit::titleChanged, this, &Macro::titleChanged);
}
