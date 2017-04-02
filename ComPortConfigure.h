#ifndef COM_PORT_CONFIGURE_H
#define COM_PORT_CONFIGURE_H

#include <QDialog>
#include <QComboBox>
#include <QSerialPort>
#include <QSettings>

class ComPortConfigure : public QDialog
{
    Q_OBJECT
public:
    ComPortConfigure(QSerialPort *port, QWidget *parent = 0);
    void saveSettings(QSettings *settings);
    void loadSettings(QSettings *settings);
private:
    QSerialPort *port;
    QComboBox *m_cbPort;
    QComboBox *m_cbBaud;
    QComboBox *m_cbBits;
    QComboBox *m_cbParity;
    QComboBox *m_cbStopBits;

    void view();
    void connections();
    void portNameSetting();
    void portBaudSetting();
    void portDataBitsSetting();
    void portParitySetting();
    void portStopBitsSetting();
};

#endif // COM_PORT_CONFIGURE_H
