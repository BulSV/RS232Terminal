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
#include <QFileDialog>

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
    void saveSettings(QSettings *settings);
    void loadSettings(QSettings *settings);
signals:
    void packageSended(const QByteArray &package);
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
    QFileDialog *fileDialog;

    void addMacros();
    void deleteMacros();
    void deleteMacros(MacrosWidget *macros);
    void deleteAllMacroses();
    void moveMacrosUp();
    void moveMacrosDown();
    void moveMacros(MacrosWidget *macrosWidget, MacrosMoveDirection direction);
    void loadMacroses();
    void startOrStop();
};

#endif // MACROSES_H
