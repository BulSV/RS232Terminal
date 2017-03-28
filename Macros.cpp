#include "Macros.h"

Macros::Macros(QObject *parent)
    : QObject(parent)
    , macrosWidget(new MacrosWidget)
    , macrosEditWidget(new MacrosEditWidget(macrosWidget))
{

}

Macros::~Macros()
{
    delete macrosWidget;
}

QWidget *Macros::getMacrosWidget() const
{
    return macrosWidget;
}

void Macros::saveSettings(QSettings *settings, int macrosIndex)
{

}

void Macros::loadSettings(QSettings *settings, int macrosIndex)
{

}

void Macros::setTimeMode(TimeMode timeMode)
{
    this->timeMode = timeMode;
}

Macros::TimeMode Macros::getTimeMode() const
{
    return timeMode;
}

int Macros::getSendTime() const
{
    return macrosWidget->getTime();
}

