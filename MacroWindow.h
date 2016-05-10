#ifndef MACROWINDOW_H
#define MACROWINDOW_H

#include "Macros.h"
#include <QWidget>
#include <QList>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>

class MacroWindow : public QWidget
{
    Q_OBJECT
    QPushButton *bAddMacros;
    QVBoxLayout *mainLayout;

    QMap<int, Macros *> MacrosList;
    int id;

    void connections();

protected:
    virtual void closeEvent(QCloseEvent *e);

public slots:
    void addMacros();
    void delMacros(int index);
    void macrosRecieved(const QString &str){ emit WriteMacros(str); }
    void tick();

signals:
        WriteMacros(const QString &str);
public:
    QTimer *tMacro;
    QTimer *tSend;
    QString path;
    MacroWindow(QString title, QWidget *parent = 0);
};

#endif // MACROWINDOW_H
