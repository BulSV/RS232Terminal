#ifndef MACROSES_H
#define MACROSES_H

#include <QMainWindow>
#include <QAction>
#include <QSpinBox>
#include <QWidgetAction>
#include <QToolBar>
#include <QList>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QSettings>

#include "MacrosWidget.h"

class Macroses : public QMainWindow
{
    Q_OBJECT
    enum MacrosMoveDirection
    {
        MoveUp = 0,
        MoveDown = 1
    };
public:
    explicit Macroses(QWidget *parent = 0);
private:
    QAction *actionPause;
    QAction *actionStartStop;
    QAction *actionDelete;
    QAction *actionNew;
    QAction *actionLoad;
    QSpinBox *time;
    QAction *actionSetTime;
    QToolBar *toolBar;
    QList<MacrosWidget*> macrosWidgets;
    QWidget *mainWidget;
    QVBoxLayout *scrollAreaLayout;
    QScrollArea *scrollArea;

    void addMacros();
    void deleteMacros();
    void deleteMacros(MacrosWidget *macros);
    void deleteAllMacroses();
    void moveMacrosUp();
    void moveMacrosDown();
    void moveMacros(MacrosWidget *macrosWidget, MacrosMoveDirection direction);
};

#endif // MACROSES_H
