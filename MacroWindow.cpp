#include "MacroWindow.h"
#include "Macros.h"
#include <QDir>
#include <QCloseEvent>
#include <QMapIterator>
#include <QScrollArea>
#include <QScrollBar>

MacroWindow::MacroWindow(QString title, QWidget *parent)
    : QWidget(parent, Qt::WindowCloseButtonHint)
    , bAddMacros(new QPushButton("Add Macros", this))
    , spacer(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding))
    , settings(new QSettings("settings.ini", QSettings::IniFormat))
    , tMacro(new QTimer(this))
{
    setWindowTitle(title);
    connections();
    tMacro->setInterval(10);
    setMinimumWidth(750);
    setMinimumHeight(100);
    resize(settings->value("config/m_width", 750).toInt(), settings->value("config/m_height", 300).toInt());
    QScrollArea *scrollArea = new QScrollArea(this);
    widget = new QWidget(scrollArea);
    mainLayout = new QVBoxLayout(widget);
    mainLayout->addWidget(bAddMacros);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->addSpacerItem(spacer);
    scrollArea->setWidget(widget);
    scrollArea->show();
    scrollArea->setVisible(true);
    scrollArea->setVerticalScrollBar(new QScrollBar(Qt::Vertical, scrollArea));
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);
    QVBoxLayout *scrollAreaLayout = new QVBoxLayout;
    scrollAreaLayout->addWidget(scrollArea);
    setLayout(scrollAreaLayout);
    id = 0;
    activeMacrosCount = 0;
    sendingIndex = -1;

    QDir dir;
    path = dir.currentPath()+"/Macros";
    if (!dir.exists(path))
    {
        dir.mkpath(path);
    }
    loadPrevSession();
}

void MacroWindow::loadPrevSession()
{
    int size = settings->beginReadArray("macros");
    if (!size)
    {
        addMacros();
        return;
    }
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
    mainLayout->removeItem(spacer);
    mainLayout->addWidget(MacrosList.last());
    mainLayout->addSpacerItem(spacer);
    connect(MacrosList.last(), SIGNAL(Sending(bool)), this, SLOT(addDelSending(bool)));
    connect(MacrosList.last(), SIGNAL(DeleteMacros(int)), this, SLOT(delMacros(int)));
    connect(MacrosList.last(), SIGNAL(WriteMacros(const QString)), this, SLOT(macrosRecieved(const QString)));
}

void MacroWindow::addDelSending(bool check)
{
    if (check)
        activeMacrosCount++;
    else
    {
        activeMacrosCount--;
        if (!activeMacrosCount)
        {
            sendingIndex = -1;
            tMacro->setInterval(10);
        }
    }
}

void MacroWindow::delMacros(int index)
{
    macrosCount--;
    delete MacrosList[index];
    MacrosList.remove(index);
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
    if (activeMacrosCount)
    {
        if (sendingIndex != -1 && tMacro->interval() == MacrosList.value(sendingIndex)->sbMacrosInterval->value())
            emit WriteMacros(MacrosList.value(sendingIndex)->leMacros->text());

        setSendingIndex();
        tMacro->setInterval(MacrosList.value(sendingIndex)->sbMacrosInterval->value());
    }
}

void MacroWindow::setSendingIndex()
{
    int startIndex = sendingIndex;
    foreach (Macros *m, MacrosList.values()){
        if (m->cbMacrosActive->checkState() && m->index > sendingIndex)
        {
            sendingIndex = m->index;
            return;
        }
    }
    if (startIndex == sendingIndex)
        foreach (Macros *m, MacrosList.values()) {
            if (m->cbMacrosActive->checkState())
            {
                sendingIndex = m->index;
                return;
            }
        }
}

void MacroWindow::closeEvent(QCloseEvent *e)
{
    QWidget::hide();
    e->ignore();
}
