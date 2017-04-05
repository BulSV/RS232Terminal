#include <QMessageBox>

#include "MacroWidget.h"

#include <QDebug>

const int MAX_SYMBOLS_COUNT = 20;

MacroWidget::MacroWidget(QWidget *parent)
: QWidget(parent)
, buttonDelete(new ClickableLabel(this))
, checkBoxInterval(new QCheckBox(this))
, checkBoxPeriod(new QCheckBox(this))
, spinBoxTime(new QSpinBox(this))
, buttonSend(new RightClickedButton(tr("Empty"), this))
, buttonUp(new ClickableLabel(this))
, buttonDown(new ClickableLabel(this))
, macroEditWidget(new MacroEditWidget(this))
, timerPeriod(new QTimer(this))
{
    spinBoxTime->setRange(0, 999999);
    spinBoxTime->setValue(50);

    view();
    connections();
}

void MacroWidget::saveSettings(QSettings *settings, int macroIndex)
{
    QString macroIndexString = QString::number(macroIndex);
    settings->setValue("macros/" + macroIndexString + "/interval", intervalIsChecked());
    settings->setValue("macros/" + macroIndexString + "/period", periodIsChecked());
    settings->setValue("macros/" + macroIndexString + "/time", getTime());

    macroEditWidget->saveSettings(settings, macroIndex);
}

void MacroWidget::loadSettings(QSettings *settings, int macroIndex)
{
    QString macroIndexString = QString::number(macroIndex);
    setCheckedInterval(settings->value("macros/" + macroIndexString + "/interval").toBool());
    setCheckedPeriod(settings->value("macros/" + macroIndexString + "/period").toBool());
    setTime(settings->value("macros/" + macroIndexString + "/time").toInt());

    macroEditWidget->loadSettings(settings, macroIndex);
}

void MacroWidget::setCheckedInterval(bool check)
{
    checkBoxInterval->setChecked(check);
}

bool MacroWidget::intervalIsChecked() const
{
    return checkBoxInterval->isChecked();
}

void MacroWidget::setCheckedPeriod(bool check)
{
    checkBoxPeriod->setChecked(check);
}

bool MacroWidget::periodIsChecked() const
{
    return checkBoxPeriod->isChecked();
}

void MacroWidget::setEnabledInterval(bool enable)
{
    checkBoxInterval->setEnabled(enable);
}

bool MacroWidget::intervalIsEnabled() const
{
    return checkBoxInterval->isEnabled();
}

void MacroWidget::setEnabledPeriod(bool enable)
{
    checkBoxPeriod->setEnabled(enable);
}

bool MacroWidget::periodIsEnabled() const
{
    return checkBoxPeriod->isEnabled();
}

void MacroWidget::setTime(int time)
{
    spinBoxTime->setValue(time);
}

int MacroWidget::getTime() const
{
    return spinBoxTime->value();
}

const QByteArray &MacroWidget::getPackage() const
{
    return macroEditWidget->getPackage();
}

void MacroWidget::openMacroFile(const QString &fileName)
{
    macroEditWidget->openMacroFile(fileName);
}

void MacroWidget::deleteMacro()
{
    int button = QMessageBox::question(this, tr("Warning"),
    tr("Delete macro ") + buttonSend->text() + " ?",
    QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes) {
        emit deleted();
    }
}

void MacroWidget::sendPackage()
{
    emit packageSended(macroEditWidget->getPackage());
}

void MacroWidget::titleChanged()
{
    int symbolsCount = buttonSend->text().size();
    if(symbolsCount > MAX_SYMBOLS_COUNT) {
        buttonSend->setText(buttonSend->text().mid(0, MAX_SYMBOLS_COUNT));
    }
}

void MacroWidget::view()
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

void MacroWidget::connections()
{
    connect(checkBoxInterval, &QCheckBox::toggled, this, &MacroWidget::intervalChecked);
    connect(checkBoxPeriod, &QCheckBox::toggled, this, &MacroWidget::periodChecked);
    connect(buttonSend, &RightClickedButton::rightClicked, macroEditWidget, &MacroEditWidget::show);
    connect(buttonSend, &RightClickedButton::clicked, this, &MacroWidget::sendPackage);
    connect(spinBoxTime, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MacroWidget::sendTimeChanged);
    connect(buttonDelete, &ClickableLabel::clicked, this, &MacroWidget::deleteMacro);
    connect(timerPeriod, &QTimer::timeout, this, &MacroWidget::sendPackage);
    connect(buttonUp, &ClickableLabel::clicked, this, &MacroWidget::movedUp);
    connect(buttonDown, &ClickableLabel::clicked, this, &MacroWidget::movedDown);
    connect(macroEditWidget, &MacroEditWidget::titleChanged, buttonSend, &RightClickedButton::setText);
    connect(macroEditWidget, &MacroEditWidget::titleChanged, this, &MacroWidget::titleChanged);
}
