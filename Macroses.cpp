#include <QGridLayout>
#include <QScrollBar>
#include <QMessageBox>

#include "Macroses.h"

#include <QDebug>

Macroses::Macroses(QWidget *parent)
    : QMainWindow(parent)
    , actionPause(new QAction(QIcon(":/Resources/Pause.png"), tr("Pause"), this))
    , actionStartStop(new QAction(QIcon(":/Resources/Play.png"), tr("Start"), this))
    , actionDelete(new QAction(QIcon(":/Resources/Delete.png"), tr("Delete all"), this))
    , actionNew(new QAction(QIcon(":/Resources/Add.png"), tr("Add macros"), this))
    , actionLoad(new QAction(QIcon(":/Resources/Open.png"), tr("Load macroses"), this))
    , time(new QSpinBox(this))
    , actionSetTime(new QAction(QIcon(":/Resources/Time.png"), tr("Set time between macroses"), this))
    , toolBar(new QToolBar(this))
    , mainWidget(new QWidget(this))
    , scrollAreaLayout(new QVBoxLayout)
    , scrollArea(new QScrollArea(this))
    , fileDialog(new QFileDialog(this))
{
    actionPause->setCheckable(true);
    time->setRange(1, 60000);
    QWidgetAction *actionTime = new QWidgetAction(toolBar);
    actionTime->setDefaultWidget(time);
    actionTime->setWhatsThis(tr("Macroses time"));
    actionSetTime->setCheckable(true);
    QList<QAction*> actions;
    actions << actionDelete << actionSetTime << actionTime << actionNew
            << actionLoad << actionStartStop << actionPause;
    toolBar->addActions(actions);
    toolBar->setMovable(false);
    addToolBar(Qt::TopToolBarArea, toolBar);

    setCentralWidget(scrollArea);
    scrollArea->setWidget(mainWidget);
    scrollArea->setVerticalScrollBar(new QScrollBar(Qt::Vertical, scrollArea));
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);

    scrollAreaLayout->setSpacing(0);
    scrollAreaLayout->setContentsMargins(0, 0, 0, 0);
    scrollAreaLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    mainWidget->setLayout(scrollAreaLayout);

    QDir dir;
    if(!dir.exists(dir.currentPath() + "/Macroses")) {
        dir.mkpath(dir.currentPath() + "/Macroses");
    }
    fileDialog->setDirectory(dir.currentPath() + "/Macroses");
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setNameFilter(tr("Terminal Macros File (*.tmf)"));

    connect(actionNew, &QAction::triggered, this, &Macroses::addMacros);
    connect(actionDelete, &QAction::triggered,
            this, static_cast<void (Macroses::*)()>(&Macroses::deleteAllMacroses));
    connect(actionLoad, &QAction::triggered, this, &Macroses::loadMacroses);
    connect(actionStartStop, &QAction::triggered, this, &Macroses::startOrStop);
    connect(actionPause, &QAction::triggered, this, &Macroses::pause);
}

void Macroses::saveSettings(QSettings *settings)
{
    settings->remove("macroses");
    settings->setValue("macroses/time", time->value());
    settings->setValue("macroses/setTime", actionSetTime->isChecked());

    int macrosIndex = 1;
    QListIterator<MacrosWidget*> it(macrosWidgets);
    MacrosWidget *m = 0;
    while(it.hasNext()) {
        m = it.next();
        m->saveSettings(settings, macrosIndex);
        macrosIndex++;
    }
    settings->setValue("macroses/count", macrosIndex - 1);
}

void Macroses::loadSettings(QSettings *settings)
{
    time->setValue(settings->value("macroses/time", 50).toInt());
    actionSetTime->setChecked(settings->value("macroses/setTime", false).toBool());

    int macrosesCount = settings->value("macroses/count", 0).toInt();
    for(int macrosIndex = 1; macrosIndex <= macrosesCount; ++macrosIndex) {
        addMacros();
        macrosWidgets.last()->loadSettings(settings, macrosIndex);
        if(actionSetTime->isChecked()) {
            macrosWidgets.last()->setCheckedInterval(true);
        }
    }
}

void Macroses::addMacros()
{
    MacrosWidget *macros = new MacrosWidget(this);
    macrosWidgets.append(macros);
    scrollAreaLayout->insertWidget(scrollAreaLayout->count() - 1, macros);

    connect(macros, &MacrosWidget::deleted, this, static_cast<void (Macroses::*)()>(&Macroses::deleteMacros));
    connect(macros, &MacrosWidget::packageSended, this, &Macroses::packageSended);
//    connect(macros, &MacrosWidget::intervalChecked, this, &Macroses::updateIntervalsList);
    connect(macros, &MacrosWidget::movedUp, this, &Macroses::moveMacrosUp);
    connect(macros, &MacrosWidget::movedDown, this, &Macroses::moveMacrosDown);
    connect(actionSetTime, &QAction::triggered, macros, &MacrosWidget::setCheckedInterval);
    connect(time, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), macros, &MacrosWidget::setTime);
}

void Macroses::deleteMacros()
{
    deleteMacros(qobject_cast<MacrosWidget*>(sender()));
}

void Macroses::deleteMacros(MacrosWidget *macros)
{
    if(macros == 0) {
        return;
    }
    macros->setCheckedInterval(false);
    macros->setCheckedPeriod(false);
    macrosWidgets.removeOne(macros);
    scrollAreaLayout->removeWidget(macros);
    disconnect(macros, &MacrosWidget::deleted, this, static_cast<void (Macroses::*)()>(&Macroses::deleteMacros));
    disconnect(macros, &MacrosWidget::packageSended, this, &Macroses::packageSended);
//    disconnect(macros, &MacrosWidget::intervalChecked, this, &Macroses::updateIntervalsList);
    disconnect(macros, &MacrosWidget::movedUp, this, &Macroses::moveMacrosUp);
    disconnect(macros, &MacrosWidget::movedDown, this, &Macroses::moveMacrosDown);
    disconnect(actionSetTime, &QAction::triggered, macros, &MacrosWidget::setCheckedInterval);
    disconnect(time, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), macros, &MacrosWidget::setTime);
    delete macros;
    macros = 0;
}

void Macroses::deleteAllMacroses()
{
    int button = QMessageBox::question(this, tr("Warning"),
                                       tr("Delete ALL macroses?"),
                                       QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes) {
        QListIterator<MacrosWidget*> it(macrosWidgets);
        while(it.hasNext()) {
            deleteMacros(it.next());
        }
        macrosWidgets.clear();
    }
}

void Macroses::moveMacrosUp()
{
    moveMacros(qobject_cast<MacrosWidget*>(sender()), MoveUp);
}

void Macroses::moveMacrosDown()
{
    moveMacros(qobject_cast<MacrosWidget*>(sender()), MoveDown);
}

void Macroses::moveMacros(MacrosWidget *macrosWidget, Macroses::MacrosMoveDirection direction)
{
    if(!macrosWidget) {
        return;
    }

    int macrosIndex = macrosWidgets.indexOf(macrosWidget);

    QVBoxLayout* tempLayout = qobject_cast<QVBoxLayout*>(macrosWidget->parentWidget()->layout());
    int index = tempLayout->indexOf(macrosWidget);

    if(direction == MoveUp) {
        if(macrosIndex == 0) {
            return;
        }

        macrosWidgets.swap(macrosIndex, macrosIndex - 1);
        --index;
    } else {
        if(macrosIndex == macrosWidgets.size() - 1) {
            return;
        }

        macrosWidgets.swap(macrosIndex, macrosIndex + 1);
        ++index;
    }
    tempLayout->removeWidget(macrosWidget);
    tempLayout->insertWidget(index, macrosWidget);
}

void Macroses::loadMacroses()
{
    QStringList fileNames;
    if(fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
    }

    QListIterator<QString> it(fileNames);
    while(it.hasNext()) {
        addMacros();
        macrosWidgets.last()->openMacrosFile(it.next());
    }
}

void Macroses::startOrStop()
{
    if(actionStartStop->toolTip() == tr("Start")) {
        actionStartStop->setIcon(QIcon(":/Resources/Stop.png"));
        actionStartStop->setToolTip(tr("Stop"));
    } else {
        actionStartStop->setIcon(QIcon(":/Resources/Play.png"));
        actionStartStop->setToolTip(tr("Start"));
    }
}

void Macroses::pause(bool check)
{
    if(check) {
//        m_tIntervalSending->stop();
        qDebug() << "paused";
    } else /*if(sendCount != 0)*/ {
//        m_tIntervalSending->start();
        qDebug() << "resumed";
    }
}
