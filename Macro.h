#ifndef MACRO_H
#define MACRO_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QTimer>
#include <QSettings>
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
    QSettings *settings;
    QString path;

    int CurrPackegeIndex;
    QMap<int, QString> MacroValue;
    QMap<int, int> MacroInterval;
    QMap<int, bool> MacroChecked;
    QMap<int, QString> MacroPaths;

    void view();
    void openLoad(QLineEdit *le, QSpinBox *sb, int i);
    void pathLoad(QLineEdit *le, QSpinBox *sb, int i, QString fileName);
    void save(QLineEdit *le, QSpinBox *sb);
    void addPackege(int index, QLineEdit *le, QSpinBox *sb, QCheckBox *cb);
    void delPackege(int index);
    void Init();
    void connections();
    void setInterval(QSpinBox *sb, int i);

protected:
    virtual void closeEvent(QCloseEvent *e);

signals:
    void WriteMacros(bool);
    void added();
    void deleted();

private slots:
    void startSending();
    void setPackege();
    void send();
    void checkForEmpty();
    void checked1(bool check)
    {
        if (check) addPackege(1, leMacro1, sbMacroInterval1, cbMacroActive1);
        else delPackege(1);
    }
    void checked2(bool check)
    {
        if (check) addPackege(2, leMacro2, sbMacroInterval2, cbMacroActive2);
        else delPackege(2);
    }
    void checked3(bool check)
    {
        if (check) addPackege(3, leMacro3, sbMacroInterval3, cbMacroActive3);
        else delPackege(3);
    }
    void checked4(bool check)
    {
        if (check) addPackege(4, leMacro4, sbMacroInterval4, cbMacroActive4);
        else delPackege(4);
    }
    void checked5(bool check)
    {
        if (check) addPackege(5, leMacro5, sbMacroInterval5, cbMacroActive5);
        else delPackege(5);
    }
    void checked6(bool check)
    {
        if (check) addPackege(6, leMacro6, sbMacroInterval6, cbMacroActive6);
        else delPackege(6);
    }
    void checked7(bool check)
    {
        if (check) addPackege(7, leMacro7, sbMacroInterval7, cbMacroActive7);
        else delPackege(7);
    }
    void checked8(bool check)
    {
        if (check) addPackege(8, leMacro8, sbMacroInterval8, cbMacroActive8);
        else delPackege(8);
    }
    void checked9(bool check)
    {
        if (check) addPackege(9, leMacro9, sbMacroInterval9, cbMacroActive9);
        else delPackege(9);
    }
    void checked10(bool check)
    {
        if (check) addPackege(10, leMacro10, sbMacroInterval10, cbMacroActive10);
        else delPackege(10);
    }

    void load1(){openLoad(leMacro1, sbMacroInterval1, 1);}
    void load2(){openLoad(leMacro2, sbMacroInterval2, 2);}
    void load3(){openLoad(leMacro3, sbMacroInterval3, 3);}
    void load4(){openLoad(leMacro4, sbMacroInterval4, 4);}
    void load5(){openLoad(leMacro5, sbMacroInterval5, 5);}
    void load6(){openLoad(leMacro6, sbMacroInterval6, 6);}
    void load7(){openLoad(leMacro7, sbMacroInterval7, 7);}
    void load8(){openLoad(leMacro8, sbMacroInterval8, 8);}
    void load9(){openLoad(leMacro9, sbMacroInterval9, 9);}
    void load10(){openLoad(leMacro10, sbMacroInterval10, 10);}

    void save1(){save(leMacro1, sbMacroInterval1);}
    void save2(){save(leMacro2, sbMacroInterval2);}
    void save3(){save(leMacro3, sbMacroInterval3);}
    void save4(){save(leMacro4, sbMacroInterval4);}
    void save5(){save(leMacro5, sbMacroInterval5);}
    void save6(){save(leMacro6, sbMacroInterval6);}
    void save7(){save(leMacro7, sbMacroInterval7);}
    void save8(){save(leMacro8, sbMacroInterval8);}
    void save9(){save(leMacro9, sbMacroInterval9);}
    void save10(){save(leMacro10, sbMacroInterval10);}

    void setInt1(){setInterval(sbMacroInterval1, 1);}
    void setInt2(){setInterval(sbMacroInterval2, 2);}
    void setInt3(){setInterval(sbMacroInterval3, 3);}
    void setInt4(){setInterval(sbMacroInterval4, 4);}
    void setInt5(){setInterval(sbMacroInterval5, 5);}
    void setInt6(){setInterval(sbMacroInterval6, 6);}
    void setInt7(){setInterval(sbMacroInterval7, 7);}
    void setInt8(){setInterval(sbMacroInterval8, 8);}
    void setInt9(){setInterval(sbMacroInterval9, 9);}
    void setInt10(){setInterval(sbMacroInterval10, 10);}

public:
    void stop();
    QString MacroData;
    explicit Macro(QString title, QWidget *parent = 0);
};

#endif // MACRO_H
