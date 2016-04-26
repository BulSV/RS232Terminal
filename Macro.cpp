#include "Macro.h"
#include <QGridLayout>
#include <QCloseEvent>

#define STANDARTINTERVAL 100

Macro::Macro(QString title, QWidget *parent)
    : QWidget(parent, Qt::WindowCloseButtonHint)
    , leMacro1(new QLineEdit(this))
    , leMacro2(new QLineEdit(this))
    , leMacro3(new QLineEdit(this))
    , leMacro4(new QLineEdit(this))
    , leMacro5(new QLineEdit(this))
    , leMacro6(new QLineEdit(this))
    , leMacro7(new QLineEdit(this))
    , leMacro8(new QLineEdit(this))
    , leMacro9(new QLineEdit(this))
    , leMacro10(new QLineEdit(this))
    , bMacroLoad1(new QPushButton("Load", this))
    , bMacroLoad2(new QPushButton("Load", this))
    , bMacroLoad3(new QPushButton("Load", this))
    , bMacroLoad4(new QPushButton("Load", this))
    , bMacroLoad5(new QPushButton("Load", this))
    , bMacroLoad6(new QPushButton("Load", this))
    , bMacroLoad7(new QPushButton("Load", this))
    , bMacroLoad8(new QPushButton("Load", this))
    , bMacroLoad9(new QPushButton("Load", this))
    , bMacroLoad10(new QPushButton("Load", this))
    , bMacroSave1(new QPushButton("Save", this))
    , bMacroSave2(new QPushButton("Save", this))
    , bMacroSave3(new QPushButton("Save", this))
    , bMacroSave4(new QPushButton("Save", this))
    , bMacroSave5(new QPushButton("Save", this))
    , bMacroSave6(new QPushButton("Save", this))
    , bMacroSave7(new QPushButton("Save", this))
    , bMacroSave8(new QPushButton("Save", this))
    , bMacroSave9(new QPushButton("Save", this))
    , bMacroSave10(new QPushButton("Save", this))
    , sbMacroInterval1(new QSpinBox(this))
    , sbMacroInterval2(new QSpinBox(this))
    , sbMacroInterval3(new QSpinBox(this))
    , sbMacroInterval4(new QSpinBox(this))
    , sbMacroInterval5(new QSpinBox(this))
    , sbMacroInterval6(new QSpinBox(this))
    , sbMacroInterval7(new QSpinBox(this))
    , sbMacroInterval8(new QSpinBox(this))
    , sbMacroInterval9(new QSpinBox(this))
    , sbMacroInterval10(new QSpinBox(this))
    , cbMacroActive1(new QCheckBox(this))
    , cbMacroActive2(new QCheckBox(this))
    , cbMacroActive3(new QCheckBox(this))
    , cbMacroActive4(new QCheckBox(this))
    , cbMacroActive5(new QCheckBox(this))
    , cbMacroActive6(new QCheckBox(this))
    , cbMacroActive7(new QCheckBox(this))
    , cbMacroActive8(new QCheckBox(this))
    , cbMacroActive9(new QCheckBox(this))
    , cbMacroActive10(new QCheckBox(this))
    , tMacro(new QTimer(this))
    , settings(new QSettings("settings.ini", QSettings::IniFormat))
{
    setAttribute(Qt::WA_DeleteOnClose);
    resize(650, 300);
    setWindowTitle(title);
    view();
    widgetInit();
    connections();

    settings->beginGroup("macros");
    cbMacroActive1->setChecked(settings->value("m1").toBool());
    cbMacroActive2->setChecked(settings->value("m2").toBool());
    cbMacroActive3->setChecked(settings->value("m3").toBool());
    cbMacroActive4->setChecked(settings->value("m4").toBool());
    cbMacroActive5->setChecked(settings->value("m5").toBool());
    cbMacroActive6->setChecked(settings->value("m6").toBool());
    cbMacroActive7->setChecked(settings->value("m7").toBool());
    cbMacroActive8->setChecked(settings->value("m8").toBool());
    cbMacroActive9->setChecked(settings->value("m9").toBool());
    cbMacroActive10->setChecked(settings->value("m10").toBool());
    settings->endGroup();
}

void Macro::widgetInit()
{
    sbMacroInterval1->setRange(0, 100000);
    sbMacroInterval1->setValue(STANDARTINTERVAL);
    sbMacroInterval2->setRange(0, 100000);
    sbMacroInterval2->setValue(STANDARTINTERVAL);
    sbMacroInterval3->setRange(0, 100000);
    sbMacroInterval3->setValue(STANDARTINTERVAL);
    sbMacroInterval4->setRange(0, 100000);
    sbMacroInterval4->setValue(STANDARTINTERVAL);
    sbMacroInterval5->setRange(0, 100000);
    sbMacroInterval5->setValue(STANDARTINTERVAL);
    sbMacroInterval6->setRange(0, 100000);
    sbMacroInterval6->setValue(STANDARTINTERVAL);
    sbMacroInterval7->setRange(0, 100000);
    sbMacroInterval7->setValue(STANDARTINTERVAL);
    sbMacroInterval8->setRange(0, 100000);
    sbMacroInterval8->setValue(STANDARTINTERVAL);
    sbMacroInterval9->setRange(0, 100000);
    sbMacroInterval9->setValue(STANDARTINTERVAL);
    sbMacroInterval10->setRange(0, 100000);
    sbMacroInterval10->setValue(STANDARTINTERVAL);
}

void Macro::view()
{
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(bMacroLoad1, 0, 0);
    mainLayout->addWidget(bMacroSave1, 0, 1);
    mainLayout->addWidget(leMacro1, 0, 2);
    mainLayout->addWidget(sbMacroInterval1, 0, 3);
    mainLayout->addWidget(cbMacroActive1, 0, 4);

    mainLayout->addWidget(bMacroLoad2, 1, 0);
    mainLayout->addWidget(bMacroSave2, 1, 1);
    mainLayout->addWidget(leMacro2, 1, 2);
    mainLayout->addWidget(sbMacroInterval2, 1, 3);
    mainLayout->addWidget(cbMacroActive2, 1, 4);

    mainLayout->addWidget(bMacroLoad3, 2, 0);
    mainLayout->addWidget(bMacroSave3, 2, 1);
    mainLayout->addWidget(leMacro3, 2, 2);
    mainLayout->addWidget(sbMacroInterval3, 2, 3);
    mainLayout->addWidget(cbMacroActive3, 2, 4);

    mainLayout->addWidget(bMacroLoad4, 3, 0);
    mainLayout->addWidget(bMacroSave4, 3, 1);
    mainLayout->addWidget(leMacro4, 3, 2);
    mainLayout->addWidget(sbMacroInterval4, 3, 3);
    mainLayout->addWidget(cbMacroActive4, 3, 4);

    mainLayout->addWidget(bMacroLoad5, 4, 0);
    mainLayout->addWidget(bMacroSave5, 4, 1);
    mainLayout->addWidget(leMacro5, 4, 2);
    mainLayout->addWidget(sbMacroInterval5, 4, 3);
    mainLayout->addWidget(cbMacroActive5, 4, 4);

    mainLayout->addWidget(bMacroLoad6, 5, 0);
    mainLayout->addWidget(bMacroSave6, 5, 1);
    mainLayout->addWidget(leMacro6, 5, 2);
    mainLayout->addWidget(sbMacroInterval6, 5, 3);
    mainLayout->addWidget(cbMacroActive6, 5, 4);

    mainLayout->addWidget(bMacroLoad7, 6, 0);
    mainLayout->addWidget(bMacroSave7, 6, 1);
    mainLayout->addWidget(leMacro7, 6, 2);
    mainLayout->addWidget(sbMacroInterval7, 6, 3);
    mainLayout->addWidget(cbMacroActive7, 6, 4);

    mainLayout->addWidget(bMacroLoad8, 7, 0);
    mainLayout->addWidget(bMacroSave8, 7, 1);
    mainLayout->addWidget(leMacro8, 7, 2);
    mainLayout->addWidget(sbMacroInterval8, 7, 3);
    mainLayout->addWidget(cbMacroActive8, 7, 4);

    mainLayout->addWidget(bMacroLoad9, 8, 0);
    mainLayout->addWidget(bMacroSave9, 8, 1);
    mainLayout->addWidget(leMacro9, 8, 2);
    mainLayout->addWidget(sbMacroInterval9, 8, 3);
    mainLayout->addWidget(cbMacroActive9, 8, 4);

    mainLayout->addWidget(bMacroLoad10, 9, 0);
    mainLayout->addWidget(bMacroSave10, 9, 1);
    mainLayout->addWidget(leMacro10, 9, 2);
    mainLayout->addWidget(sbMacroInterval10, 9, 3);
    mainLayout->addWidget(cbMacroActive10, 9, 4);

    setLayout(mainLayout);
}

void Macro::connections()
{
    connect(cbMacroActive1, SIGNAL(toggled(bool)), this, SLOT(checked1(bool)));
    connect(cbMacroActive2, SIGNAL(toggled(bool)), this, SLOT(checked2(bool)));
    connect(cbMacroActive3, SIGNAL(toggled(bool)), this, SLOT(checked3(bool)));
    connect(cbMacroActive4, SIGNAL(toggled(bool)), this, SLOT(checked4(bool)));
    connect(cbMacroActive5, SIGNAL(toggled(bool)), this, SLOT(checked5(bool)));
    connect(cbMacroActive6, SIGNAL(toggled(bool)), this, SLOT(checked6(bool)));
    connect(cbMacroActive7, SIGNAL(toggled(bool)), this, SLOT(checked7(bool)));
    connect(cbMacroActive8, SIGNAL(toggled(bool)), this, SLOT(checked8(bool)));
    connect(cbMacroActive9, SIGNAL(toggled(bool)), this, SLOT(checked9(bool)));
    connect(cbMacroActive10, SIGNAL(toggled(bool)), this, SLOT(checked10(bool)));

    connect(bMacroLoad1, SIGNAL(clicked(bool)), this, SLOT(load));

    connect(this, SIGNAL(added()), this, SLOT(startSending()));
    connect(tMacro, SIGNAL(timeout()), this, SLOT(setPackege()));
    connect(this, SIGNAL(deleted()), this, SLOT(checkForEmpty()));
}

void Macro::addPackege(int index, QLineEdit *le, QSpinBox *sb)
{
    settings->setValue("macros/m"+index, true);
    MacroValue.insert(index, le->text());
    MacroInterval.insert(index, sb->value());
    MacroChecked[index] = true;
    emit added();
}

void Macro::delPackege(int index)
{
    settings->setValue("macros/m"+index, false);
    MacroValue.remove(index);
    MacroInterval.remove(index);
    MacroChecked[index] = false;
    emit deleted();
}

void Macro::stop()
{
    tMacro->stop();
    CurrPackegeIndex = 0;
    cbMacroActive1->setChecked(false);
    cbMacroActive2->setChecked(false);
    cbMacroActive3->setChecked(false);
    cbMacroActive4->setChecked(false);
    cbMacroActive5->setChecked(false);
    cbMacroActive6->setChecked(false);
    cbMacroActive7->setChecked(false);
    cbMacroActive8->setChecked(false);
    cbMacroActive9->setChecked(false);
    cbMacroActive10->setChecked(false);
}

void Macro::checkForEmpty()
{
    int empty = 0;
    foreach (bool is, MacroChecked.values())
       if (!is)
           empty++;

    if (empty == 10)
    {
        tMacro->stop();
        CurrPackegeIndex = 0;
    }
}

void Macro::startSending()
{
    if (CurrPackegeIndex == 0)
    {
        for (int i = 1; i <= 10; i++)
        {
            if (MacroChecked[i])
            {
                CurrPackegeIndex = i;
                tMacro->setInterval(MacroInterval[i]);
                tMacro->start();
                return;
            }
        }
    }
}

void Macro::send()
{
    MacroData = MacroValue[CurrPackegeIndex];
    WriteMacros(true);
    MacroData.clear();
}

void Macro::setPackege()
{
    if (MacroChecked[CurrPackegeIndex])
    {
        MacroData = MacroValue[CurrPackegeIndex];
        WriteMacros(true);
        MacroData.clear();
    }

    if (CurrPackegeIndex == 10)
        CurrPackegeIndex = 0;
    CurrPackegeIndex++;

    if (MacroChecked[CurrPackegeIndex])
        tMacro->setInterval(MacroInterval[CurrPackegeIndex]);
    else
        tMacro->setInterval(1);
}

void Macro::closeEvent(QCloseEvent *e)
{
    QWidget::hide();
    e->ignore();
}
