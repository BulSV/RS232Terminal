#include "HEXLineEdit.h"
#include <QRegExpValidator>

HEXLineEdit::HEXLineEdit(QWidget *parent) : QLineEdit(parent)
{
    //connect(this, SIGNAL(textChanged(QString)), this, SLOT(changed(QString)));
}

void HEXLineEdit::changed(QString str)
{
    str = str.toUpper();


    blockSignals(true);
    setText(str);
    blockSignals(false);
}

