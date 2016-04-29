#include "Macro.h"
#include <QGridLayout>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>

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
    , lMacro1(new QLabel(this))
    , lMacro2(new QLabel(this))
    , lMacro3(new QLabel(this))
    , lMacro4(new QLabel(this))
    , lMacro5(new QLabel(this))
    , lMacro6(new QLabel(this))
    , lMacro7(new QLabel(this))
    , lMacro8(new QLabel(this))
    , lMacro9(new QLabel(this))
    , lMacro10(new QLabel(this))
    , tMacro(new QTimer(this))
    , settings(new QSettings("settings.ini", QSettings::IniFormat))
{
    setAttribute(Qt::WA_DeleteOnClose);
    resize(750, 300);
    setWindowTitle(title);
    view();
    Init();
    connections();
    CurrPackegeIndex = 0;

    QDir dir;
    path = dir.currentPath()+"/Macros";
    if (!dir.exists(path))
        dir.mkpath(path);
}

void Macro::Init()
{
    lMacro1->setStyleSheet("font-weight: bold");
    lMacro2->setStyleSheet("font-weight: bold");
    lMacro3->setStyleSheet("font-weight: bold");
    lMacro4->setStyleSheet("font-weight: bold");
    lMacro5->setStyleSheet("font-weight: bold");
    lMacro6->setStyleSheet("font-weight: bold");
    lMacro7->setStyleSheet("font-weight: bold");
    lMacro8->setStyleSheet("font-weight: bold");
    lMacro9->setStyleSheet("font-weight: bold");
    lMacro10->setStyleSheet("font-weight: bold");

    sbMacroInterval1->setRange(1, 100000);
    sbMacroInterval1->setValue(STANDARTINTERVAL);
    sbMacroInterval2->setRange(1, 100000);
    sbMacroInterval2->setValue(STANDARTINTERVAL);
    sbMacroInterval3->setRange(1, 100000);
    sbMacroInterval3->setValue(STANDARTINTERVAL);
    sbMacroInterval4->setRange(1, 100000);
    sbMacroInterval4->setValue(STANDARTINTERVAL);
    sbMacroInterval5->setRange(1, 100000);
    sbMacroInterval5->setValue(STANDARTINTERVAL);
    sbMacroInterval6->setRange(1, 100000);
    sbMacroInterval6->setValue(STANDARTINTERVAL);
    sbMacroInterval7->setRange(1, 100000);
    sbMacroInterval7->setValue(STANDARTINTERVAL);
    sbMacroInterval8->setRange(1, 100000);
    sbMacroInterval8->setValue(STANDARTINTERVAL);
    sbMacroInterval9->setRange(1, 100000);
    sbMacroInterval9->setValue(STANDARTINTERVAL);
    sbMacroInterval10->setRange(1, 100000);
    sbMacroInterval10->setValue(STANDARTINTERVAL);

    if ( !settings->value("macros/FilePath1").toString().isEmpty() ) pathLoad(leMacro1, sbMacroInterval1, lMacro1, 1, settings->value("macros/FilePath1").toString());
    if ( !settings->value("macros/FilePath2").toString().isEmpty() ) pathLoad(leMacro2, sbMacroInterval2, lMacro2, 2, settings->value("macros/FilePath2").toString());
    if ( !settings->value("macros/FilePath3").toString().isEmpty() ) pathLoad(leMacro3, sbMacroInterval3, lMacro3, 3, settings->value("macros/FilePath3").toString());
    if ( !settings->value("macros/FilePath4").toString().isEmpty() ) pathLoad(leMacro4, sbMacroInterval4, lMacro4, 4, settings->value("macros/FilePath4").toString());
    if ( !settings->value("macros/FilePath5").toString().isEmpty() ) pathLoad(leMacro5, sbMacroInterval5, lMacro5, 5, settings->value("macros/FilePath5").toString());
    if ( !settings->value("macros/FilePath6").toString().isEmpty() ) pathLoad(leMacro6, sbMacroInterval6, lMacro6, 6, settings->value("macros/FilePath6").toString());
    if ( !settings->value("macros/FilePath7").toString().isEmpty() ) pathLoad(leMacro7, sbMacroInterval7, lMacro7, 7, settings->value("macros/FilePath7").toString());
    if ( !settings->value("macros/FilePath8").toString().isEmpty() ) pathLoad(leMacro8, sbMacroInterval8, lMacro8, 8, settings->value("macros/FilePath8").toString());
    if ( !settings->value("macros/FilePath9").toString().isEmpty() ) pathLoad(leMacro9, sbMacroInterval9, lMacro9, 9, settings->value("macros/FilePath9").toString());
    if ( !settings->value("macros/FilePath10").toString().isEmpty() ) pathLoad(leMacro10, sbMacroInterval10, lMacro10, 10, settings->value("macros/FilePath10").toString());
}

void Macro::view()
{
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(lMacro1, 0, 6);
    mainLayout->addWidget(bMacroLoad1, 0, 1);
    mainLayout->addWidget(bMacroSave1, 0, 2);
    mainLayout->addWidget(leMacro1, 0, 3);
    mainLayout->addWidget(sbMacroInterval1, 0, 4);
    mainLayout->addWidget(cbMacroActive1, 0, 5);

    mainLayout->addWidget(lMacro2, 1, 6);
    mainLayout->addWidget(bMacroLoad2, 1, 1);
    mainLayout->addWidget(bMacroSave2, 1, 2);
    mainLayout->addWidget(leMacro2, 1, 3);
    mainLayout->addWidget(sbMacroInterval2, 1, 4);
    mainLayout->addWidget(cbMacroActive2, 1, 5);

    mainLayout->addWidget(lMacro3, 2, 6);
    mainLayout->addWidget(bMacroLoad3, 2, 1);
    mainLayout->addWidget(bMacroSave3, 2, 2);
    mainLayout->addWidget(leMacro3, 2, 3);
    mainLayout->addWidget(sbMacroInterval3, 2, 4);
    mainLayout->addWidget(cbMacroActive3, 2, 5);

    mainLayout->addWidget(lMacro4, 3, 6);
    mainLayout->addWidget(bMacroLoad4, 3, 1);
    mainLayout->addWidget(bMacroSave4, 3, 2);
    mainLayout->addWidget(leMacro4, 3, 3);
    mainLayout->addWidget(sbMacroInterval4, 3, 4);
    mainLayout->addWidget(cbMacroActive4, 3, 5);

    mainLayout->addWidget(lMacro5, 4, 6);
    mainLayout->addWidget(bMacroLoad5, 4, 1);
    mainLayout->addWidget(bMacroSave5, 4, 2);
    mainLayout->addWidget(leMacro5, 4, 3);
    mainLayout->addWidget(sbMacroInterval5, 4, 4);
    mainLayout->addWidget(cbMacroActive5, 4, 5);

    mainLayout->addWidget(lMacro6, 5, 6);
    mainLayout->addWidget(bMacroLoad6, 5, 1);
    mainLayout->addWidget(bMacroSave6, 5, 2);
    mainLayout->addWidget(leMacro6, 5, 3);
    mainLayout->addWidget(sbMacroInterval6, 5, 4);
    mainLayout->addWidget(cbMacroActive6, 5, 5);

    mainLayout->addWidget(lMacro7, 6, 6);
    mainLayout->addWidget(bMacroLoad7, 6, 1);
    mainLayout->addWidget(bMacroSave7, 6, 2);
    mainLayout->addWidget(leMacro7, 6, 3);
    mainLayout->addWidget(sbMacroInterval7, 6, 4);
    mainLayout->addWidget(cbMacroActive7, 6, 5);

    mainLayout->addWidget(lMacro8, 7, 6);
    mainLayout->addWidget(bMacroLoad8, 7, 1);
    mainLayout->addWidget(bMacroSave8, 7, 2);
    mainLayout->addWidget(leMacro8, 7, 3);
    mainLayout->addWidget(sbMacroInterval8, 7, 4);
    mainLayout->addWidget(cbMacroActive8, 7, 5);

    mainLayout->addWidget(lMacro9, 8, 6);
    mainLayout->addWidget(bMacroLoad9, 8, 1);
    mainLayout->addWidget(bMacroSave9, 8, 2);
    mainLayout->addWidget(leMacro9, 8, 3);
    mainLayout->addWidget(sbMacroInterval9, 8, 4);
    mainLayout->addWidget(cbMacroActive9, 8, 5);

    mainLayout->addWidget(lMacro10, 9, 6);
    mainLayout->addWidget(bMacroLoad10, 9, 1);
    mainLayout->addWidget(bMacroSave10, 9, 2);
    mainLayout->addWidget(leMacro10, 9, 3);
    mainLayout->addWidget(sbMacroInterval10, 9, 4);
    mainLayout->addWidget(cbMacroActive10, 9, 5);

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

    connect(bMacroLoad1, SIGNAL(clicked(bool)), this, SLOT(load1()));
    connect(bMacroLoad2, SIGNAL(clicked(bool)), this, SLOT(load2()));
    connect(bMacroLoad3, SIGNAL(clicked(bool)), this, SLOT(load3()));
    connect(bMacroLoad4, SIGNAL(clicked(bool)), this, SLOT(load4()));
    connect(bMacroLoad5, SIGNAL(clicked(bool)), this, SLOT(load5()));
    connect(bMacroLoad6, SIGNAL(clicked(bool)), this, SLOT(load6()));
    connect(bMacroLoad7, SIGNAL(clicked(bool)), this, SLOT(load7()));
    connect(bMacroLoad8, SIGNAL(clicked(bool)), this, SLOT(load8()));
    connect(bMacroLoad9, SIGNAL(clicked(bool)), this, SLOT(load9()));
    connect(bMacroLoad10, SIGNAL(clicked(bool)), this, SLOT(load10()));

    connect(bMacroSave1, SIGNAL(clicked(bool)), this, SLOT(save1()));
    connect(bMacroSave2, SIGNAL(clicked(bool)), this, SLOT(save2()));
    connect(bMacroSave3, SIGNAL(clicked(bool)), this, SLOT(save3()));
    connect(bMacroSave4, SIGNAL(clicked(bool)), this, SLOT(save4()));
    connect(bMacroSave5, SIGNAL(clicked(bool)), this, SLOT(save5()));
    connect(bMacroSave6, SIGNAL(clicked(bool)), this, SLOT(save6()));
    connect(bMacroSave7, SIGNAL(clicked(bool)), this, SLOT(save7()));
    connect(bMacroSave8, SIGNAL(clicked(bool)), this, SLOT(save8()));
    connect(bMacroSave9, SIGNAL(clicked(bool)), this, SLOT(save9()));
    connect(bMacroSave10, SIGNAL(clicked(bool)), this, SLOT(save10()));

    connect(sbMacroInterval1, SIGNAL(valueChanged(int)), this, SLOT(setInt1()));
    connect(sbMacroInterval2, SIGNAL(valueChanged(int)), this, SLOT(setInt2()));
    connect(sbMacroInterval3, SIGNAL(valueChanged(int)), this, SLOT(setInt3()));
    connect(sbMacroInterval4, SIGNAL(valueChanged(int)), this, SLOT(setInt4()));
    connect(sbMacroInterval5, SIGNAL(valueChanged(int)), this, SLOT(setInt5()));
    connect(sbMacroInterval6, SIGNAL(valueChanged(int)), this, SLOT(setInt6()));
    connect(sbMacroInterval7, SIGNAL(valueChanged(int)), this, SLOT(setInt7()));
    connect(sbMacroInterval8, SIGNAL(valueChanged(int)), this, SLOT(setInt8()));
    connect(sbMacroInterval9, SIGNAL(valueChanged(int)), this, SLOT(setInt9()));
    connect(sbMacroInterval10, SIGNAL(valueChanged(int)), this, SLOT(setInt10()));

    connect(this, SIGNAL(added()), this, SLOT(startSending()));
    connect(tMacro, SIGNAL(timeout()), this, SLOT(setPackege()));
    connect(this, SIGNAL(deleted()), this, SLOT(checkForEmpty()));
}

void Macro::setInterval(QSpinBox *sb, int i)
{
    MacroInterval[i] = sb->value();
}

void Macro::addPackege(int index, QLineEdit *le, QSpinBox *sb, QCheckBox *cb)
{
    if (!le->text().isEmpty())
    {
        if (!MacroPaths[index].isEmpty())
            settings->setValue("macros/FilePath" + QString::number(index), MacroPaths[index]);

        MacroValue.insert(index, le->text());
        MacroInterval.insert(index, sb->value());
        MacroChecked[index] = true;
        emit added();
    } else
        cb->setChecked(false);
}

void Macro::delPackege(int index)
{
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

void Macro::openLoad(QLineEdit *le, QSpinBox *sb, QLabel *l, int i)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), path, tr("Macro Files (*.rsmc)"));
        if (fileName != "") {
            QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
                return;
            }
            QTextStream stream(&file);
            le->setText(stream.readLine(0));
            sb->setValue(stream.readLine(6).toInt());
            file.close();
            MacroPaths.insert(i, fileName);
            QFileInfo fileInfo(file.fileName());
            l->setText(fileInfo.baseName());
        }
}

void Macro::pathLoad(QLineEdit *le, QSpinBox *sb, QLabel *l, int i, QString fileName)
{
        if (fileName != "") {
            QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
                return;
            }
            QTextStream stream(&file);
            le->setText(stream.readLine(0));
            sb->setValue(stream.readLine(6).toInt());
            file.close();
            MacroPaths.insert(i, fileName);
            QFileInfo fileInfo(file.fileName());
            l->setText(fileInfo.baseName());
        }
}

void Macro::save(QLineEdit *le, QSpinBox *sb)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), path, tr("Macro Files (*.rsmc)"));

        if (fileName != "") {
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not save file"));
                return;
            } else {
                QTextStream stream(&file);
                stream << le->text()+"\n";
                stream.flush();
                stream << sb->value();
                stream.flush();
                file.close();
            }
        }
}

void Macro::closeEvent(QCloseEvent *e)
{
    QWidget::hide();
    e->ignore();
}
