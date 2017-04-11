#include <QGridLayout>
#include <QScrollBar>
#include <QMessageBox>
#include <QToolBar>
#include <QWidgetAction>
#include <algorithm>

#include "Macros.h"

#include <QDebug>

const int DEFAULT_TIME = 50; // ms
const int DEFAULT_COUNT = 0;

Macros::Macros(QWidget *parent)
    : QMainWindow(parent)
    , actionPause(new QAction(QIcon(":/Resources/Pause.png"), tr("Pause sending macros"), this))
    , actionStartStop(new QAction(QIcon(":/Resources/Play.png"), tr("Start sending macros"), this))
    , actionDelete(new QAction(QIcon(":/Resources/Delete.png"), tr("Delete macros"), this))
    , actionNew(new QAction(QIcon(":/Resources/Add.png"), tr("Add empty macro"), this))
    , actionLoad(new QAction(QIcon(":/Resources/Open.png"), tr("Load macros"), this))
    , time(new QSpinBox(this))
    , selectMacro(new QAction(QIcon(":/Resources/Select.png"), tr("Select macros"), this))
    , deselectMacro(new QAction(QIcon(":/Resources/Deselect.png"), tr("Deselect macros"), this))
    , mainWidget(new QWidget(this))
    , scrollAreaLayout(new QVBoxLayout)
    , scrollArea(new QScrollArea(this))
    , fileDialog(new QFileDialog(this))
    , intervalTimer(new QTimer(this))
    , currentIntervalIndex(0)
{
    actionPause->setCheckable(true);
    actionPause->setEnabled(false);
    time->setRange(0, 60000);
    time->setToolTip(tr("Time, ms"));
    QToolBar *toolBar = new QToolBar(this);
    toolBar->setStyleSheet("spacing:2px");
    QWidgetAction *actionTime = new QWidgetAction(toolBar);
    actionTime->setDefaultWidget(time);
    QList<QAction*> actions;
    actions << actionDelete << selectMacro << deselectMacro << actionTime
            << actionNew << actionLoad << actionStartStop << actionPause;
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
    if(!dir.exists(dir.currentPath() + "/Macros")) {
        dir.mkpath(dir.currentPath() + "/Macros");
    }
    fileDialog->setDirectory(dir.currentPath() + "/Macros");
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setNameFilter(tr("Terminal Macro File (*.tmf)"));

    connect(actionNew, &QAction::triggered, this, &Macros::addMacro);
    connect(actionDelete, &QAction::triggered,
            this, static_cast<void (Macros::*)()>(&Macros::deleteMacros));
    connect(actionLoad, &QAction::triggered, this, &Macros::loadMacros);
    connect(actionStartStop, &QAction::triggered, this, &Macros::startOrStop);
    connect(actionPause, &QAction::triggered, this, &Macros::pause);
    connect(intervalTimer, &QTimer::timeout, this, &Macros::sendNextMacro);
}

void Macros::saveSettings(QSettings *settings)
{
    settings->remove("macros");
    settings->setValue("macros/time", time->value());

    int macroIndex = 1;
    QListIterator<Macro*> it(macros);
    Macro *m = 0;
    while(it.hasNext()) {
        m = it.next();
        m->saveSettings(settings, macroIndex);
        macroIndex++;
    }
    settings->setValue("macros/count", macroIndex - 1);
}

void Macros::loadSettings(QSettings *settings)
{
    time->setValue(settings->value("macros/time", DEFAULT_TIME).toInt());
    selectMacro->setChecked(settings->value("macros/setTime", false).toBool());

    int macrosCount = settings->value("macros/count", DEFAULT_COUNT).toInt();
    for(int macroIndex = 1; macroIndex <= macrosCount; ++macroIndex) {
        addMacro();
        macros.last()->loadSettings(settings, macroIndex);
        if(selectMacro->isChecked()) {
            macros.last()->deselect();
        }
    }
}

void Macros::setWorkState(bool work)
{
}

void Macros::addMacro()
{
    Macro *macro = new Macro(this);
    macros.append(macro);
    scrollAreaLayout->insertWidget(scrollAreaLayout->count() - 1, macro);

    connect(macro, &Macro::deleted, this, static_cast<void (Macros::*)()>(&Macros::deleteMacro));
    connect(macro, &Macro::packetSended, this, &Macros::packetSended);
    connect(macro, &Macro::selected, this, &Macros::updateIntervals);
    connect(macro, &Macro::movedUp, this, &Macros::moveMacroUp);
    connect(macro, &Macro::movedDown, this, &Macros::moveMacroDown);
    connect(selectMacro, &QAction::triggered, macro, &Macro::select);
    connect(deselectMacro, &QAction::triggered, macro, &Macro::deselect);
    connect(time, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), macro, &Macro::setTime);
}

void Macros::deleteMacro()
{
    deleteMacro(qobject_cast<Macro*>(sender()));
}

void Macros::deleteMacro(Macro *macro)
{
    if(macro == 0) {
        return;
    }
    macro->deselect();
    macros.removeOne(macro);
    scrollAreaLayout->removeWidget(macro);
    disconnect(macro, &Macro::deleted, this, static_cast<void (Macros::*)()>(&Macros::deleteMacro));
    disconnect(macro, &Macro::packetSended, this, &Macros::packetSended);
    disconnect(macro, &Macro::selected, this, &Macros::updateIntervals);
    disconnect(macro, &Macro::movedUp, this, &Macros::moveMacroUp);
    disconnect(macro, &Macro::movedDown, this, &Macros::moveMacroDown);
    disconnect(selectMacro, &QAction::triggered, macro, &Macro::select);
    connect(deselectMacro, &QAction::triggered, macro, &Macro::deselect);
    disconnect(time, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), macro, &Macro::setTime);
    delete macro;
    macro = 0;
}

void Macros::deleteMacros()
{
    int button = QMessageBox::question(this, tr("Warning"),
                                       tr("Delete macros?"),
                                       QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes) {
        QListIterator<Macro*> it(macros);
        while(it.hasNext()) {
            deleteMacro(it.next());
        }
        macros.clear();
    }
}

void Macros::moveMacroUp()
{
    moveMacro(qobject_cast<Macro*>(sender()), MoveUp);
}

void Macros::moveMacroDown()
{
    moveMacro(qobject_cast<Macro*>(sender()), MoveDown);
}

void Macros::moveMacro(Macro *macro, Macros::MacrosMoveDirection direction)
{
    if(!macro) {
        return;
    }

    int macroIndex = macros.indexOf(macro);

    QVBoxLayout* tempLayout = qobject_cast<QVBoxLayout*>(macro->parentWidget()->layout());
    int index = tempLayout->indexOf(macro);

    if(direction == MoveUp) {
        if(macroIndex == 0) {
            return;
        }

        macros.swap(macroIndex, macroIndex - 1);
        --index;
    } else {
        if(macroIndex == macros.size() - 1) {
            return;
        }

        macros.swap(macroIndex, macroIndex + 1);
        ++index;
    }
    tempLayout->removeWidget(macro);
    tempLayout->insertWidget(index, macro);
    std::sort(indexesOfIntervals.begin(), indexesOfIntervals.end(), qLess<int>());
}

void Macros::loadMacros()
{
    QStringList fileNames;
    if(fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
    }

    QListIterator<QString> it(fileNames);
    while(it.hasNext()) {
        addMacro();
        macros.last()->openMacroFile(it.next());
    }
}

void Macros::startOrStop()
{
    if(indexesOfIntervals.isEmpty()) {
        return;
    }
    if(actionStartStop->toolTip() == tr("Start sending macros")) {
        actionStartStop->setIcon(QIcon(":/Resources/Stop.png"));
        actionStartStop->setToolTip(tr("Stop sending macros"));
        actionPause->setEnabled(true);
        blockForMultiSend(true);
        sendNextMacro();
    } else {
        actionStartStop->setIcon(QIcon(":/Resources/Play.png"));
        actionStartStop->setToolTip(tr("Start sending macros"));
        actionPause->setChecked(false);
        actionPause->setToolTip("Pause sending macros");
        actionPause->setEnabled(false);
        intervalTimer->stop();
        currentIntervalIndex = 0;
        blockForMultiSend(false);
    }
}

void Macros::pause(bool check)
{
    if(check) {
        intervalTimer->stop();
        actionPause->setToolTip("Resume sending macros");
    } else {
        intervalTimer->start();
        actionPause->setToolTip("Pause sending macros");
    }
}

void Macros::updateIntervals(bool add)
{
    Macro *macro = qobject_cast<Macro*>(sender());
    if(macro == 0) {
        return;
    }
    int index = macros.indexOf(macro);
    if(add) {
        indexesOfIntervals.append(index);
        std::sort(indexesOfIntervals.begin(), indexesOfIntervals.end(), qLess<int>());

        return;
    }

    indexesOfIntervals.removeOne(index);
}

void Macros::sendNextMacro()
{
    Macro *macro = macros.at(indexesOfIntervals.at(currentIntervalIndex++));
    if(currentIntervalIndex >= indexesOfIntervals.size()) {
        currentIntervalIndex = 0;
    }

    emit packetSended(macro->getPacket());

    intervalTimer->start(macro->getTime());
}

void Macros::blockForMultiSend(bool block)
{
    actionDelete->setDisabled(block);
    selectMacro->setDisabled(block);
    deselectMacro->setDisabled(block);
    time->setDisabled(block);
    actionNew->setDisabled(block);
    actionLoad->setDisabled(block);

    QListIterator<Macro*> it(macros);
    while(it.hasNext()) {
        it.next()->setDisabled(block);
    }
}
