#ifndef MACROS_RAW_EDIT_WIDGET_H
#define MACROS_RAW_EDIT_WIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QByteArray>
#include <QCloseEvent>
#include <QSettings>

#include "AsciiEncoder.h"
#include "HexEncoder.h"
#include "DecEncoder.h"

class MacrosRawEditWidget : public QWidget
{
    Q_OBJECT
    enum Modes
    {
        ASCII = 0,
        HEX = 1,
        DEC = 2
    };

public:
    explicit MacrosRawEditWidget(QWidget *parent = 0);
    const QByteArray &getData() const;
    void setData(const QByteArray &data);
    void saveSettings(QSettings *settings, int macrosIndex);
    void loadSettings(QSettings *settings, int macrosIndex);
    void show();
signals:
    void dataInputted(const QByteArray &data);
private:
    QComboBox *mode;
    QLineEdit *separator;
    QLineEdit *rawData;
    QPushButton *input;

    AsciiEncoder *asciiEncoder;
    HexEncoder *hexEncoder;
    DecEncoder *decEncoder;

    int currentMode;
    QByteArray data;

    QSize savedWidgetSize;

    void view();
    void connections();
    void onSelectMode(int mode);
    void onRawDataInput();
    QString fromStringListToString(const QList<QString> &stringList);
    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *e);
};

#endif // MACROS_RAW_EDIT_WIDGET_H
