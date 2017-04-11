#ifndef MACROS_H
#define MACROS_H

#include <QMainWindow>
#include <QAction>
#include <QSpinBox>
#include <QList>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QSettings>
#include <QFileDialog>

#include "Macro.h"

class Macros : public QMainWindow
{
    Q_OBJECT
    enum MacrosMoveDirection
    {
        MoveUp = 0,
        MoveDown = 1
    };
public:
    explicit Macros(QWidget *parent = 0);
    void saveSettings(QSettings *settings);
    void loadSettings(QSettings *settings);
    void setWorkState(bool work);
signals:
    void packetSended(const QByteArray &package);
private:
    QAction *actionPause;
    QAction *actionStartStop;
    QAction *actionDelete;
    QAction *actionNew;
    QAction *actionLoad;
    QSpinBox *time;
    QAction *selectMacro;
    QAction *deselectMacro;
    QList<Macro*> macros;
    QWidget *mainWidget;
    QVBoxLayout *scrollAreaLayout;
    QScrollArea *scrollArea;
    QFileDialog *fileDialog;

    QTimer *intervalTimer;
    QList<int> indexesOfIntervals;
    int currentIntervalIndex;

    void addMacro();
    void deleteMacro();
    void deleteMacro(Macro *macro);
    void deleteMacros();
    void moveMacroUp();
    void moveMacroDown();
    void moveMacro(Macro *macro, MacrosMoveDirection direction);
    void loadMacros();
    void startOrStop();
    void pause(bool check);
    void updateIntervals(bool add);
    void sendNextMacro();
    void blockForMultiSend(bool block);
};

#endif // MACROS_H
