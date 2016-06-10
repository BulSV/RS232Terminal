#ifndef MACROS_H
#define MACROS_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QGridLayout>
#include <QTimer>

class Macros : public QWidget
{
    Q_OBJECT

    void connections();

private slots:
    void singleSend();
    void Delete();
    void save();
    void stateChange(bool check);
    void textChanged(QString text);
    void openLoad();
    void startStopPeriodSending(bool check);
    void tick();

signals:
    void DeleteMacros(int);
    void WriteMacros(const QString &str);
    void Sending(bool);
    void PeriodSending(const QString &str);
    void textChanged(QString str, int index);
    void checkedInterval(bool, int);
    void checkedPeriod(bool, int);

public:
    QLineEdit *leMacros;
    QPushButton *bMacrosLoad;
    QPushButton *bMacrosSave;
    QSpinBox *sbMacrosInterval;
    QCheckBox *cbMacrosActive;
    QCheckBox *cbPeriodSending;
    QPushButton *bMacros;
    QPushButton *bMacrosDel;
    QTimer *tPeriodSending;

    int index;
    QString path;
    bool isFromFile;

    bool openPath(QString);
    explicit Macros(int id, QString p, bool buttonActive, QWidget *parent = 0);
};

#endif // MACROS_H
