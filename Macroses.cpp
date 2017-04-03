#include <QGridLayout>
#include <QScrollBar>
#include <QMessageBox>

#include "Macroses.h"

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
{
    actionPause->setCheckable(true);
    actionStartStop->setCheckable(true);
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

    connect(actionNew, &QAction::triggered, this, &Macroses::addMacros);
    connect(actionDelete, &QAction::triggered,
            this, static_cast<void (Macroses::*)()>(&Macroses::deleteAllMacroses));
}

void Macroses::addMacros()
{
    MacrosWidget *macrosWidget = new MacrosWidget(this);
    macrosWidgets.append(macrosWidget);
    scrollAreaLayout->insertWidget(scrollAreaLayout->count() - 1, macrosWidget);

    connect(macrosWidget, &MacrosWidget::deleted, this, static_cast<void (Macroses::*)()>(&Macroses::deleteMacros));
//    connect(macrosWidget, &MacrosWidget::packageSended, this, static_cast<void (Macroses::*)(const QByteArray &)>(&Macroses::sendPackage));
//    connect(macrosWidget, &MacrosWidget::intervalChecked, this, &Macroses::updateIntervalsList);
    connect(macrosWidget, &MacrosWidget::movedUp, this, &Macroses::moveMacrosUp);
    connect(macrosWidget, &MacrosWidget::movedDown, this, &Macroses::moveMacrosDown);
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
//    disconnect(macros, &MacrosWidget::packageSended, this, static_cast<void (Macroses::*)(const QByteArray &)>(&Macroses::sendPackage));
//    disconnect(macros, &MacrosWidget::intervalChecked, this, &Macroses::updateIntervalsList);
    disconnect(macros, &MacrosWidget::movedUp, this, &Macroses::moveMacrosUp);
    disconnect(macros, &MacrosWidget::movedDown, this, &Macroses::moveMacrosDown);
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

