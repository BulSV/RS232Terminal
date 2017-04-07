#ifndef MACRO_WIDGET_H
#define MACRO_WIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QTimer>
#include <QSettings>
#include <QByteArray>

#include "RightClickedButton.h"
#include "MacroEdit.h"
#include "ClickableLabel.h"

class Macro : public QWidget
{
    Q_OBJECT
public:
    explicit Macro(QWidget *parent = 0);

    void saveSettings(QSettings *settings, int macroIndex);
    void loadSettings(QSettings *settings, int macroIndex);

    void select();
    void deselect();
    void selectToggle();
    bool selectState() const;

    void enableSelectState(bool enable);
    bool isEnabledSelectState() const;

    void setTime(int time);
    int getTime() const;

    const QByteArray &getPacket() const;
    void openMacroFile(const QString &fileName);
    void stopSend();
signals:
    void deleted();
    void selected(bool select);
    void toggled();
    void packetSended(const QByteArray &packet);
    void timeChanged(int time);
    void movedUp();
    void movedDown();
private:
    ClickableLabel *buttonDelete;
    QCheckBox *checkBoxSelect;
    QSpinBox *spinBoxTime;
    RightClickedButton *buttonSend;
    ClickableLabel *buttonUp;
    ClickableLabel *buttonDown;
    MacroEdit *macroEdit;
    QTimer *timerPeriod;
    bool isSelected;

    void view();
    void connections();
    void deleteMacro();
    void singleSend();
    void sendPacket(bool checked);
    void titleChanged();
    void selectTrigger();
};

#endif // MACRO_WIDGET_H
