#include <QGridLayout>
#include <QScrollBar>
#include <QMessageBox>

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
    , actionSetTime(new QAction(QIcon(":/Resources/Time.png"), tr("Set time between macros"), this))
    , toolBar(new QToolBar(this))
    , mainWidget(new QWidget(this))
    , scrollAreaLayout(new QVBoxLayout)
    , scrollArea(new QScrollArea(this))
    , fileDialog(new QFileDialog(this))
{
    actionPause->setCheckable(true);
    time->setRange(1, 60000);
    time->setToolTip(tr("Time, ms"));
    QWidgetAction *actionTime = new QWidgetAction(toolBar);
    actionTime->setDefaultWidget(time);
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
}

void Macros::saveSettings(QSettings *settings)
{
    settings->remove("macros");
    settings->setValue("macros/time", time->value());
    settings->setValue("macros/setTime", actionSetTime->isChecked());

    int macroIndex = 1;
    QListIterator<MacroWidget*> it(macrosWidgets);
    MacroWidget *m = 0;
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
    actionSetTime->setChecked(settings->value("macros/setTime", false).toBool());

    int macrosCount = settings->value("macros/count", DEFAULT_COUNT).toInt();
    for(int macroIndex = 1; macroIndex <= macrosCount; ++macroIndex) {
        addMacro();
        macrosWidgets.last()->loadSettings(settings, macroIndex);
        if(actionSetTime->isChecked()) {
            macrosWidgets.last()->setCheckedInterval(true);
        }
    }
}

void Macros::addMacro()
{
    MacroWidget *macro = new MacroWidget(this);
    macrosWidgets.append(macro);
    scrollAreaLayout->insertWidget(scrollAreaLayout->count() - 1, macro);

    connect(macro, &MacroWidget::deleted, this, static_cast<void (Macros::*)()>(&Macros::deleteMacro));
    connect(macro, &MacroWidget::packageSended, this, &Macros::packageSended);
//    connect(macro, &MacroWidget::intervalChecked, this, &Macros::updateIntervalsList);
    connect(macro, &MacroWidget::movedUp, this, &Macros::moveMacroUp);
    connect(macro, &MacroWidget::movedDown, this, &Macros::moveMacroDown);
    connect(actionSetTime, &QAction::triggered, macro, &MacroWidget::setCheckedInterval);
    connect(time, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), macro, &MacroWidget::setTime);
}

void Macros::deleteMacro()
{
    deleteMacro(qobject_cast<MacroWidget*>(sender()));
}

void Macros::deleteMacro(MacroWidget *macro)
{
    if(macro == 0) {
        return;
    }
    macro->setCheckedInterval(false);
    macro->setCheckedPeriod(false);
    macrosWidgets.removeOne(macro);
    scrollAreaLayout->removeWidget(macro);
    disconnect(macro, &MacroWidget::deleted, this, static_cast<void (Macros::*)()>(&Macros::deleteMacro));
    disconnect(macro, &MacroWidget::packageSended, this, &Macros::packageSended);
//    disconnect(macro, &MacroWidget::intervalChecked, this, &Macros::updateIntervalsList);
    disconnect(macro, &MacroWidget::movedUp, this, &Macros::moveMacroUp);
    disconnect(macro, &MacroWidget::movedDown, this, &Macros::moveMacroDown);
    disconnect(actionSetTime, &QAction::triggered, macro, &MacroWidget::setCheckedInterval);
    disconnect(time, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), macro, &MacroWidget::setTime);
    delete macro;
    macro = 0;
}

void Macros::deleteMacros()
{
    int button = QMessageBox::question(this, tr("Warning"),
                                       tr("Delete ALL macros?"),
                                       QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes) {
        QListIterator<MacroWidget*> it(macrosWidgets);
        while(it.hasNext()) {
            deleteMacro(it.next());
        }
        macrosWidgets.clear();
    }
}

void Macros::moveMacroUp()
{
    moveMacro(qobject_cast<MacroWidget*>(sender()), MoveUp);
}

void Macros::moveMacroDown()
{
    moveMacro(qobject_cast<MacroWidget*>(sender()), MoveDown);
}

void Macros::moveMacro(MacroWidget *macroWidget, Macros::MacrosMoveDirection direction)
{
    if(!macroWidget) {
        return;
    }

    int macroIndex = macrosWidgets.indexOf(macroWidget);

    QVBoxLayout* tempLayout = qobject_cast<QVBoxLayout*>(macroWidget->parentWidget()->layout());
    int index = tempLayout->indexOf(macroWidget);

    if(direction == MoveUp) {
        if(macroIndex == 0) {
            return;
        }

        macrosWidgets.swap(macroIndex, macroIndex - 1);
        --index;
    } else {
        if(macroIndex == macrosWidgets.size() - 1) {
            return;
        }

        macrosWidgets.swap(macroIndex, macroIndex + 1);
        ++index;
    }
    tempLayout->removeWidget(macroWidget);
    tempLayout->insertWidget(index, macroWidget);
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
        macrosWidgets.last()->openMacroFile(it.next());
    }
}

void Macros::startOrStop()
{
    if(actionStartStop->toolTip() == tr("Start sending macros")) {
        actionStartStop->setIcon(QIcon(":/Resources/Stop.png"));
        actionStartStop->setToolTip(tr("Stop sending macros"));
    } else {
        actionStartStop->setIcon(QIcon(":/Resources/Play.png"));
        actionStartStop->setToolTip(tr("Start sending macros"));
    }
}

void Macros::pause(bool check)
{
    if(check) {
//        m_tIntervalSending->stop();
        actionPause->setToolTip("Resume sending macros");
        qDebug() << "paused";
    } else /*if(sendCount != 0)*/ {
//        m_tIntervalSending->start();
        actionPause->setToolTip("Pause sending macros");
        qDebug() << "resumed";
    }
}
