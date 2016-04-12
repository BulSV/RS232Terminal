#include "Dialog.h"
#include <QGridLayout>
#include <QApplication>

Dialog::Dialog(QString title, QWidget *parent)
    : QWidget(parent, Qt::WindowCloseButtonHint)
{
}

Dialog::~Dialog()
{

}
