#include "MacroWindow.h"
#include "Macros.h"
#include <QDir>
#include <QCloseEvent>

MacroWindow::MacroWindow(QString title, QWidget *parent)
    : QWidget(parent, Qt::WindowCloseButtonHint)
    , bAddMacros(new QPushButton("Add Macros", this))
    , mainLayout(new QVBoxLayout(this))
    , settings(new QSettings("settings.ini", QSettings::IniFormat))
    , tMacro(new QTimer(this))
{
    setWindowTitle(title);
    connections();
    id = 0;
    setMinimumWidth(750);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    setLayout(mainLayout);
    mainLayout->addWidget(bAddMacros);   
    tMacro->setInterval(10);
    sendingIndex = -1;

    QDir dir;
    path = dir.currentPath()+"/Macros";
    if (!dir.exists(path))
        dir.mkpath(path);

    loadPrevSession();
}

void MacroWindow::loadPrevSession()
{
    int size = settings->beginReadArray("macros");
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        addMacros();
        if (!MacrosList.last()->openPath(settings->value("path").toString()))
        {
            MacrosList.last()->leMacros->setText(settings->value("packege").toString());
            MacrosList.last()->sbMacrosInterval->setValue(settings->value("interval").toInt());
        }
    }
    settings->endArray();
}

void MacroWindow::saveSession()
{
    settings->remove("macros");
    settings->beginWriteArray("macros");
    int i = 0;
    foreach (Macros *m, MacrosList.values()) {
        settings->setArrayIndex(i);
        settings->setValue("packege", m->leMacros->text());
        settings->setValue("interval", m->sbMacrosInterval->value());
        settings->setValue("path", m->path);
        i++;
    }
    settings->endArray();
}

void MacroWindow::addMacros()
{
    macrosCount++;
    MacrosList.insert(id, new Macros(id, path, portOpen, this));
    id++;
    mainLayout->addWidget(MacrosList.last());
    connect(MacrosList.last(), SIGNAL(AddSending(int)), this, SLOT(addSending(Macros)));
    connect(MacrosList.last(), SIGNAL(DeleteMacros(int)), this, SLOT(delMacros(int)));
    connect(MacrosList.last(), SIGNAL(WriteMacros(const QString)), this, SLOT(macrosRecieved(const QString)));
}

void MacroWindow::delMacros(int index)
{
    macrosCount--;
    delete MacrosList[index];
    MacrosList.remove(index);
    adjustSize();
}

void MacroWindow::connections()
{
    connect(bAddMacros, SIGNAL(clicked(bool)), this, SLOT(addMacros()));
    connect(tMacro, SIGNAL(timeout()), this, SLOT(tick()));
}

void MacroWindow::start()
{
    foreach (Macros *m, MacrosList.values()) {
        m->bMacros->setEnabled(true);
        portOpen = true;
    }

    foreach (Macros *m, MacrosList.values()) {
        if (m->checked)
        {
            sendingIndex = m->index;
            break;
        }
    }
    if (sendingIndex != -1)
    {
        tMacro->setInterval(MacrosList.value(sendingIndex)->sbMacrosInterval->value());
        tMacro->start();
    } else
        tMacro->start();
}

void MacroWindow::stop()
{
    foreach (Macros *m, MacrosList.values()) {
        m->bMacros->setEnabled(false);
        portOpen = false;
    }

    tMacro->stop();
}

void MacroWindow::tick()
{
    if (sendingIndex != -1)
    {
        emit WriteMacros(MacrosList.value(sendingIndex)->leMacros->text());
        sendingIndex++;
    }
    for (int i = sendingIndex; i <= MacrosList.last()->index; i++)
    {
        if (MacrosList.value(i)->checked)
        {
            sendingIndex = i;
            tMacro->setInterval(MacrosList.value(i)->sbMacrosInterval->value());
            break;
        }
    }
}

void MacroWindow::closeEvent(QCloseEvent *e)
{
    QWidget::hide();
    e->ignore();
}
