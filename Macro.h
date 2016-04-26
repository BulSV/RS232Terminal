#ifndef MACRO_H
#define MACRO_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QTimer>
#include <QSignalMapper>

class Macro : public QWidget
{
    Q_OBJECT
    QLineEdit *leMacro1;
    QLineEdit *leMacro2;
    QLineEdit *leMacro3;
    QLineEdit *leMacro4;
    QLineEdit *leMacro5;
    QLineEdit *leMacro6;
    QLineEdit *leMacro7;
    QLineEdit *leMacro8;
    QLineEdit *leMacro9;
    QLineEdit *leMacro10;

    QPushButton *bMacroLoad1;
    QPushButton *bMacroLoad2;
    QPushButton *bMacroLoad3;
    QPushButton *bMacroLoad4;
    QPushButton *bMacroLoad5;
    QPushButton *bMacroLoad6;
    QPushButton *bMacroLoad7;
    QPushButton *bMacroLoad8;
    QPushButton *bMacroLoad9;
    QPushButton *bMacroLoad10;

    QPushButton *bMacroSave1;
    QPushButton *bMacroSave2;
    QPushButton *bMacroSave3;
    QPushButton *bMacroSave4;
    QPushButton *bMacroSave5;
    QPushButton *bMacroSave6;
    QPushButton *bMacroSave7;
    QPushButton *bMacroSave8;
    QPushButton *bMacroSave9;
    QPushButton *bMacroSave10;

    QSpinBox *sbMacroInterval1;
    QSpinBox *sbMacroInterval2;
    QSpinBox *sbMacroInterval3;
    QSpinBox *sbMacroInterval4;
    QSpinBox *sbMacroInterval5;
    QSpinBox *sbMacroInterval6;
    QSpinBox *sbMacroInterval7;
    QSpinBox *sbMacroInterval8;
    QSpinBox *sbMacroInterval9;
    QSpinBox *sbMacroInterval10;

    QCheckBox *cbMacroActive1;
    QCheckBox *cbMacroActive2;
    QCheckBox *cbMacroActive3;
    QCheckBox *cbMacroActive4;
    QCheckBox *cbMacroActive5;
    QCheckBox *cbMacroActive6;
    QCheckBox *cbMacroActive7;
    QCheckBox *cbMacroActive8;
    QCheckBox *cbMacroActive9;
    QCheckBox *cbMacroActive10;

    QTimer *tMacro;

    int CurrPackegeIndex = 1;
    QMap<int, QString> MacroValue;
    QMap<int, int> MacroInterval;
    QMap<int, bool> MacroChecked;

    void view();
    void addPackege(int index, QLineEdit *le, QSpinBox *sb);
    void delPackege(int index);
    void widgetInit();
    void connections();

protected:
    virtual void closeEvent(QCloseEvent *e);

signals:
    void WriteMacros(bool);

private slots:
    void setPackege();
    void send();
    void checked1(bool check)
    {
        if (check) addPackege(1, leMacro1, sbMacroInterval1);
        else delPackege(1);
    }
    void checked2(bool check)
    {
        if (check) addPackege(2, leMacro2, sbMacroInterval2);
        else delPackege(2);
    }
    void checked3(bool check)
    {
        if (check) addPackege(3, leMacro3, sbMacroInterval3);
        else delPackege(3);
    }
    void checked4(bool check)
    {
        if (check) addPackege(4, leMacro4, sbMacroInterval4);
        else delPackege(4);
    }
    void checked5(bool check)
    {
        if (check) addPackege(5, leMacro5, sbMacroInterval5);
        else delPackege(5);
    }
    void checked6(bool check)
    {
        if (check) addPackege(6, leMacro6, sbMacroInterval6);
        else delPackege(6);
    }
    void checked7(bool check)
    {
        if (check) addPackege(7, leMacro7, sbMacroInterval7);
        else delPackege(7);
    }
    void checked8(bool check)
    {
        if (check) addPackege(8, leMacro8, sbMacroInterval8);
        else delPackege(8);
    }
    void checked9(bool check)
    {
        if (check) addPackege(9, leMacro9, sbMacroInterval9);
        else delPackege(9);
    }
    void checked10(bool check)
    {
        if (check) addPackege(10, leMacro10, sbMacroInterval10);
        else delPackege(10);
    }

public:
    QString MacroData;
    explicit Macro(QString title, QWidget *parent = 0);
};

#endif // MACRO_H
