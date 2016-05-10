#include "MacroWindow.h"
#include "Macros.h"
#include <QDir>
#include <QCloseEvent>

MacroWindow::MacroWindow(QString title, QWidget *parent)
    : QWidget(parent, Qt::WindowCloseButtonHint)
    , bAddMacros(new QPushButton("Add Macros", this))
    , mainLayout(new QVBoxLayout(this))
    , tMacro(new QTimer(this))
    , tSend(new QTimer(this))
{
    setWindowTitle(title);
    connections();
    id = 0;
    resize(750, 50);
    setLayout(mainLayout);
    mainLayout->addWidget(bAddMacros);
    tMacro->setInterval(10);
    tMacro->start();

    QDir dir;
    path = dir.currentPath()+"/Macros";
    if (!dir.exists(path))
        dir.mkpath(path);
}

void MacroWindow::addMacros()
{
    MacrosList.insert(id, new Macros(id, path, this));
    id++;
    mainLayout->addWidget(MacrosList.last());
    connect(MacrosList.last(), SIGNAL(DeleteMacros(int)), this, SLOT(delMacros(int)));
    connect(MacrosList.last(), SIGNAL(WriteMacros(const QString)), this, SLOT(macrosRecieved(const QString)));
}

void MacroWindow::delMacros(int index)
{
    delete MacrosList[index];
    MacrosList.remove(index);
    resize(width(), 5);
}

void MacroWindow::connections()
{
    connect(bAddMacros, SIGNAL(clicked(bool)), this, SLOT(addMacros()));
    connect(tMacro, SIGNAL(timeout()), this, SLOT(tick()));
}

void MacroWindow::tick()
{
    foreach (Macros m, MacrosList.values()) {
        if (m.checked)

    }
}

void MacroWindow::closeEvent(QCloseEvent *e)
{
    QWidget::hide();
    e->ignore();
}
