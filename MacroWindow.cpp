#include "MacroWindow.h"
#include "Macros.h"
#include <QDir>
#include <QCloseEvent>
#include <QMapIterator>
#include <QScrollArea>
#include <QScrollBar>

MacroWindow::MacroWindow(QString title, QWidget *parent)
    : QWidget(parent, Qt::WindowCloseButtonHint)
    , scrollAreaLayout  (new QVBoxLayout)
    , scrollArea (new QScrollArea(this))
    , widget (new QWidget(scrollArea))
    , mainLayout (new QVBoxLayout(widget))
    , bAddMacros(new QPushButton("Add Macros", this))
    , bLoadMacroses(new QPushButton("Load Macroses", this))
    , spacer(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding))
    , settings(new QSettings("settings.ini", QSettings::IniFormat))
    , fileDialog(new QFileDialog(this))
    , tMacro(new QTimer(this))
{
    setWindowTitle(title);
    connections();

    QDir dir;
    path = dir.currentPath()+"/Macros";
    if (!dir.exists(path))
    {
        dir.mkpath(path);
    }

    fileDialog->setDirectory(path);
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setNameFilter(trUtf8("Macro Files (*.rsmc)"));

    tMacro->setInterval(10);
    setMinimumWidth(750);
    setMinimumHeight(100);
    resize(settings->value("config/m_width", 750).toInt(), settings->value("config/m_height", 300).toInt());

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(bAddMacros);
    buttonsLayout->addWidget(bLoadMacroses);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->addSpacerItem(spacer);
    scrollArea->setWidget(widget);
    scrollArea->show();
    scrollArea->setVisible(true);
    scrollArea->setVerticalScrollBar(new QScrollBar(Qt::Vertical, scrollArea));
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);
    scrollAreaLayout->addWidget(scrollArea);
    setLayout(scrollAreaLayout);
    id = 0;
    activeMacrosCount = 0;
    sendingIndex = -1;
}

void MacroWindow::loadPrevSession()
{
    const QPoint pos = settings->value ("config/macro_position").toPoint();
        if (!pos.isNull())
            move (pos);

    int size = settings->value("macros/size", 0).toInt();
    if (!size)
    {
        addMacros();
        return;
    }
    for (int i = 1; i <= size; ++i) {
        addMacros();
        if (!MacrosList.last()->openPath(settings->value("macros/"+QString::number(i)+"/path").toString()))
        {
            MacrosList.last()->leMacros->setText(settings->value("macros/"+QString::number(i)+"/packege").toString());
            MacrosList.last()->sbMacrosInterval->setValue(settings->value("macros/"+QString::number(i)+"/interval").toInt());
        }
         MacrosList.last()->cbMacrosActive->setChecked(settings->value("macros/"+QString::number(i)+"/checked_interval").toBool());
         MacrosList.last()->cbPeriodSending->setChecked(settings->value("macros/"+QString::number(i)+"/checked_period").toBool());
    }
}

void MacroWindow::saveSession()
{
    settings->setValue("config/m_height", height());
    settings->setValue("config/m_width", width());
    settings->setValue("config/macro_position", pos());

    settings->remove("macros");
    int i = 1;
    foreach (Macros *m, MacrosList.values()) {
        if (!m->isFromFile)
        {
        settings->setValue("macros/"+QString::number(i)+"/packege", m->leMacros->text());
        settings->setValue("macros/"+QString::number(i)+"/interval", m->sbMacrosInterval->value());
        }
        settings->setValue("macros/"+QString::number(i)+"/checked_interval", m->cbMacrosActive->isChecked());
        settings->setValue("macros/"+QString::number(i)+"/checked_period", m->cbPeriodSending->isChecked());
        settings->setValue("macros/"+QString::number(i)+"/path", m->path);
        i++;
    }
    settings->setValue("macros/size", i-1);
}

void MacroWindow::addMacros()
{
    macrosCount++;
    MacrosList.insert(id, new Macros(id, path, portOpen, this));
    mainLayout->removeItem(spacer);
    mainLayout->addWidget(MacrosList.last());
    mainLayout->addSpacerItem(spacer);
    connect(MacrosList.last(), SIGNAL(Sending(bool)), this, SLOT(addDelSending(bool)));
    connect(MacrosList.last(), SIGNAL(DeleteMacros(int)), this, SLOT(delMacros(int)));
    connect(MacrosList.last(), SIGNAL(WriteMacros(QString)), this, SLOT(macrosRecieved(QString)));
    connect(MacrosList.last(), SIGNAL(textChanged(QString, int)), this, SLOT(textChanged(QString, int)));
    connect(MacrosList.last(), SIGNAL(checked(bool, int)), this, SLOT(macChecked(bool, int)));
    id++;
    emit macrosAdded(MacrosList.last()->index, MacrosList.last()->bMacros->text());
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
    emit macrosDeleted(index);
}

void MacroWindow::connections()
{
    connect(bAddMacros, SIGNAL(clicked(bool)), this, SLOT(addMacros()));
    connect(bLoadMacroses, SIGNAL(clicked(bool)), this, SLOT(loadMacroses()));
    connect(tMacro, SIGNAL(timeout()), this, SLOT(tick()));
}

void MacroWindow::loadMacroses()
{
    QStringList fileNames;
    if (fileDialog->exec())
        fileNames = fileDialog->selectedFiles();

    foreach (QString s, fileNames) {
        addMacros();
        MacrosList.last()->openPath(s);
    }
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
