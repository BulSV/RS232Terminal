#ifndef MACROS_WIDGET_H
#define MACROS_WIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QTimer>
#include <QSettings>
#include <QByteArray>

#include "RightClickedButton.h"
#include "MacrosEditWidget.h"

class MacrosWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MacrosWidget(QWidget *parent = 0);

    void saveSettings(QSettings *settings, int macrosIndex);
    void loadSettings(QSettings *settings, int macrosIndex);

    void setCheckedInterval(bool check);
    bool intervalIsChecked() const;
    void setCheckedPeriod(bool check);
    bool periodIsChecked() const;

    void setEnabledInterval(bool enable);
    bool intervalIsEnabled() const;
    void setEnabledPeriod(bool enable);
    bool periodIsEnabled() const;

    void setTime(int time);
    int getTime() const;

    const QByteArray &getPackage() const;
    void openMacrosFile(const QString &fileName);
signals:
    void deleted();
    void intervalChecked(bool checked);
    void periodChecked(bool checked);
    void packageSended(const QByteArray &package);
    void sendTimeChanged(int time);
    void movedUp();
    void movedDown();
private:
    QPushButton *buttonDelete;
    QCheckBox *checkBoxInterval;
    QCheckBox *checkBoxPeriod;
    QSpinBox *spinBoxTime;
    RightClickedButton *buttonSend;
    QPushButton *buttonUp;
    QPushButton *buttonDown;
    MacrosEditWidget *macrosEditWidget;
    QTimer *timerPeriod;

    void view();
    void connections();
    void deleteMacros();
};

#endif // MACROS_WIDGET_H
