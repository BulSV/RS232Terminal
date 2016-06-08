#include "MacrosEditing.h"
#include "Macros.h"

MacrosEditing::MacrosEditing(Macros *m, QWidget *parent)
    : QWidget(parent, Qt::WindowCloseButtonHint)
{
    setWindowTitle("RS232 Terminal - Macro: " + m->bMacros->text());
    QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->addWidget(m);
}

