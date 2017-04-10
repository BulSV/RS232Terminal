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
    QList<Macro*> macrosWidgets;
    QWidget *mainWidget;
    QVBoxLayout *scrollAreaLayout;
    QScrollArea *scrollArea;
    QFileDialog *fileDialog;

    void addMacro();
    void deleteMacro();
    void deleteMacro(Macro *macro);
    void deleteMacros();
    void moveMacroUp();
    void moveMacroDown();
    void moveMacro(Macro *macroWidget, MacrosMoveDirection direction);
    void loadMacros();
    void startOrStop();
    void pause(bool check);
};

#endif // MACROS_H
