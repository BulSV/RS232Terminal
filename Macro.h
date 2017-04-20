#ifndef MACRO_H
#define MACRO_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QTimer>
#include <QSettings>
#include <QByteArray>
#include <QSerialPort>

#include "MacroEdit.h"
#include "ClickableLabel.h"

class Macro : public QWidget
{
    Q_OBJECT
public:
    explicit Macro(QWidget *parent = 0);

    void saveSettings(QSettings *settings, int macroIndex);
    void loadSettings(QSettings *settings, int macroIndex);

    void setPort(const QSerialPort *port);

    void select();
    void deselect();
    bool selectState() const;

    void enableSelectState(bool enable);
    bool isEnabledSelectState() const;

    void setTime(int time);
    int getTime() const;

    const QByteArray &getPacket() const;
    void openMacroFile(const QString &fileName);
signals:
    void deleted();
    void selected(bool select);
    void packetSended(const QByteArray &packet);
    void timeChanged(int time);
    void movedUp();
    void movedDown();
private:
    ClickableLabel *buttonDelete;
    QCheckBox *checkBoxSelect;
    QSpinBox *spinBoxTime;
    ClickableLabel *buttonSend;
    ClickableLabel *buttonUp;
    ClickableLabel *buttonDown;
    MacroEdit *macroEdit;
    const QSerialPort *port;

    void view();
    void connections();
    void deleteMacro();
    void sendPacket();
    void titleChanged();
    void selectTrigger();
    double packetSendTime();
};

#endif // MACRO_H
