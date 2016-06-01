#ifndef MACROWINDOW_H
#define MACROWINDOW_H

#include "Macros.h"
#include <QWidget>
#include <QList>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QSettings>
#include <QScrollArea>
#include <QSpacerItem>
#include <QFileDialog>

class MacroWindow : public QWidget
{
    Q_OBJECT
    QVBoxLayout *scrollAreaLayout;
    QScrollArea *scrollArea;
    QWidget *widget;
    QVBoxLayout *mainLayout;
    QPushButton *bAddMacros;
    QPushButton *bLoadMacroses;
    QSpacerItem *spacer;
    QSettings *settings;
    QFileDialog *fileDialog;
    int id;

    void connections();
    void setSendingIndex();

protected:
    virtual void closeEvent(QCloseEvent *e);

public slots:
    void cbCheck(int index, bool check) {MacrosList[index]->cbMacrosActive->setChecked(check);}
    void bPress(int index) {MacrosList[index]->bMacros->click();}
    void addMacros();
    void delMacros(int index);
    void macrosRecieved(const QString &str){ emit WriteMacros(str); }
    void tick();
    void addDelSending(bool check);
    void loadMacroses();
    void textChanged(QString str, int index) { emit textChange(str, index); }

signals:
    WriteMacros(const QString &str);
    macrosAdded(int id, const QString &str);
    macrosDeleted(int id);
    textChange(const QString str, int index);

public:
    QTimer *tMacro;
    QString path;
    int macrosCount;
    int activeMacrosCount;
    bool portOpen;
    QMap<int, Macros *> MacrosList;
    int sendingIndex;

    void start();
    void stop();
    void loadPrevSession();
    void saveSession();
    MacroWindow(QString title, QWidget *parent = 0);
};

#endif // MACROWINDOW_H
