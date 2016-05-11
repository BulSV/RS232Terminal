#include "Macros.h"
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

Macros::Macros(int id, QString p, bool buttonActive, QWidget *parent)
    : QWidget(parent)
    , leMacros(new QLineEdit(this))
    , bMacrosLoad(new QPushButton("Load", this))
    , bMacrosSave(new QPushButton("Save", this))
    , sbMacrosInterval(new QSpinBox(this))
    , cbMacrosActive(new QCheckBox(this))
    , bMacros(new QPushButton("Not saved", this))
    , bMacrosDel(new QPushButton(this))
{
    connections();
    index = id;
    path = p;
    bMacros->setStyleSheet("font-weight: bold");
    bMacros->setEnabled(buttonActive);
    bMacrosDel->resize(5, 5);
    bMacrosDel->setStyleSheet("border-image: url(:/Resources/del.png) stretch;");
    sbMacrosInterval->setRange(1, 1000000);
    sbMacrosInterval->setValue(500);
    cbMacrosActive->setEnabled(false);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(bMacrosDel, 0, 0);
    mainLayout->addWidget(bMacrosLoad, 0, 1);
    mainLayout->addWidget(bMacrosSave, 0, 2);
    mainLayout->addWidget(leMacros, 0, 3);
    mainLayout->addWidget(sbMacrosInterval, 0, 4);
    mainLayout->addWidget(cbMacrosActive, 0, 5);
    mainLayout->addWidget(bMacros, 0, 6);
    setLayout(mainLayout);
}

void Macros::connections()
{
    connect(leMacros, SIGNAL(textEdited(QString)), this, SLOT(textEdited(QString)));
    connect(bMacros, SIGNAL(clicked(bool)), this, SLOT(singleSend()));
    connect(bMacrosDel, SIGNAL(clicked(bool)), this, SLOT(Delete()));
    connect(bMacrosLoad, SIGNAL(clicked(bool)), this, SLOT(openLoad()));
    connect(bMacrosSave, SIGNAL(clicked(bool)), this, SLOT(save()));
    connect(cbMacrosActive, SIGNAL(toggled(bool)), this, SLOT(stateChange(bool)));
}

void Macros::textEdited(QString text)
{
    if (text.isEmpty())
        cbMacrosActive->setEnabled(false);
    else
        cbMacrosActive->setEnabled(true);
}

void Macros::singleSend()
{
    emit WriteMacros(leMacros->text());
}

void Macros::stateChange(bool check)
{
    checked = check;
    if (check)
    {
        bMacros->setStyleSheet("font-weight: bold; color: red;");
    } else {
        bMacros->setStyleSheet("font-weight: bold;");
    }
}

void Macros::Delete()
{
    emit DeleteMacros(index);
}

void Macros::openLoad()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), path, tr("Macro Files (*.rsmc)"));
        if (fileName != "") {
            QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
                return;
            }
            QTextStream stream(&file);
            leMacros->setText(stream.readLine(0));
            sbMacrosInterval->setValue(stream.readLine(0).toInt());
            path = fileName;
            QFileInfo fileInfo(file.fileName());
            bMacros->setText(fileInfo.baseName());
            file.close();
            cbMacrosActive->setEnabled(true);
        }
}

bool Macros::openPath(QString fileName)
{
    if (fileName != "") {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) return false;
        QTextStream stream(&file);
        leMacros->setText(stream.readLine(0));
        sbMacrosInterval->setValue(stream.readLine(0).toInt());
        path = fileName;
        QFileInfo fileInfo(file.fileName());
        bMacros->setText(fileInfo.baseName());
        file.close();
        cbMacrosActive->setEnabled(true);
        return true;
    } else
        return false;
}

void Macros::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), path, tr("Macro Files (*.rsmc)"));

        if (fileName != "") {
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not save file"));
                return;
            } else {
                QTextStream stream(&file);
                stream << leMacros->text()+"\n";
                stream.flush();
                stream << sbMacrosInterval->value();
                stream.flush();
                QFileInfo fileInfo(file.fileName());
                bMacros->setText(fileInfo.baseName());
                file.close();
            }
        }
}
