#include <QMessageBox>

#include "MacrosWidget.h"

const int MAX_SYMBOLS_COUNT = 20;

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
    settings->setValue("macroses/" + macrosIndexString + "/time", getTime());

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

void MacrosWidget::openMacrosFile(const QString &fileName)
{
    macrosEditWidget->openMacrosFile(fileName);
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

void MacrosWidget::sendPackage()
{
    emit packageSended(macrosEditWidget->getPackage());
}

void MacrosWidget::titleChanged()
{
    int symbolsCount = buttonSend->text().size();
    if(symbolsCount > MAX_SYMBOLS_COUNT) {
        buttonSend->setText(buttonSend->text().mid(0, MAX_SYMBOLS_COUNT));
    }
}

void MacrosWidget::view()
{
    buttonUp->setFixedSize(16, 13);
    buttonDown->setFixedSize(16, 13);
    buttonUp->setIcon(QIcon(":/Resources/arrow-up.png"));
    buttonDown->setIcon(QIcon(":/Resources/arrow-down.png"));

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
    layout()->setSpacing(0);
    layout()->setContentsMargins(0, 0, 0, 0);

    buttonDelete->setIcon(QIcon(":/Resources/Delete.png"));
    buttonSend->setStyleSheet("font-weight: bold");

    buttonDelete->setFixedWidth(25);
    checkBoxInterval->setFixedWidth(15);
    checkBoxPeriod->setFixedWidth(15);
    spinBoxTime->setFixedWidth(60);
    buttonSend->setMaximumWidth(150);
}

void MacrosWidget::connections()
{
    connect(checkBoxInterval, &QCheckBox::toggled, this, &MacrosWidget::intervalChecked);
    connect(checkBoxPeriod, &QCheckBox::toggled, this, &MacrosWidget::periodChecked);
    connect(buttonSend, &RightClickedButton::rightClicked, macrosEditWidget, &MacrosEditWidget::show);
    connect(buttonSend, &RightClickedButton::clicked, this, &MacrosWidget::sendPackage);
    connect(spinBoxTime, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MacrosWidget::sendTimeChanged);
    connect(buttonDelete, &QPushButton::pressed, this, &MacrosWidget::deleteMacros);
    connect(timerPeriod, &QTimer::timeout, this, &MacrosWidget::sendPackage);
    connect(buttonUp, &QPushButton::clicked, this, &MacrosWidget::movedUp);
    connect(buttonDown, &QPushButton::clicked, this, &MacrosWidget::movedDown);
    connect(macrosEditWidget, &MacrosEditWidget::titleChanged, buttonSend, &RightClickedButton::setText);
    connect(macrosEditWidget, &MacrosEditWidget::titleChanged, this, &MacrosWidget::titleChanged);
}
