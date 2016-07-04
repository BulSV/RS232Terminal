#include "Macros.h"
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>

Macros::Macros(QWidget *parent)
    : QMainWindow(parent, Qt::WindowCloseButtonHint)
    , widget(new QWidget(this))
    , toolBar(new QToolBar(this))
    , package(new QLineEdit(this))
    , mainLay(new QGridLayout())
    , rbHEX(new QRadioButton("HEX", this))
    , rbDEC(new QRadioButton("DEC", this))
    , rbASCII(new QRadioButton("ASCII", this))
    , lbHEX(new QLabel(this))
    , lbDEC(new QLabel(this))
    , lbASCII(new QLabel(this))
{
    setWindowTitle("RS232 Terminal - Macros");

    addToolBar(Qt::TopToolBarArea, toolBar);
    toolBar->setMovable(false);
    toolBar->addAction("New");
    toolBar->addAction("Load");
    toolBar->addAction("Save");
    toolBar->addAction("Save as", this, SLOT(saveDialog()));

    widget->setLayout(mainLay);
    setCentralWidget(widget);
    mainLay->setSpacing(3);
    mainLay->setMargin(3);
    mainLay->addWidget(new QLabel("Formats:", this), 0, 0);
    mainLay->addWidget(package, 0, 1);
    mainLay->addWidget(rbHEX, 1, 0);
    mainLay->addWidget(lbHEX, 1, 1);
    mainLay->addWidget(rbDEC, 2, 0);
    mainLay->addWidget(lbDEC, 2, 1);
    mainLay->addWidget(rbASCII, 3, 0);
    mainLay->addWidget(lbASCII, 3, 1);

    setMinimumSize(400, 150);
    rbHEX->setChecked(true);
}


void Macros::saveDialog()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::currentPath()+"/Macros", tr("Macro Files (*.rsmc)"));

        if (fileName != "") {
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not save file"));
                return;
            } else {
                QTextStream stream(&file);
                stream << package->text()+"\n";
                stream.flush();
                file.close();
            }
        }
}

void Macros::openDialog()
{
//    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath()+"/Macros", tr("Macro Files (*.rsmc)"));
//        if (fileName != "") {
//            QFile file(fileName);
//            if (!file.open(QIODevice::ReadOnly)) {
//                QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
//                return;
//            }
//            QTextStream stream(&file);
//            leMacros->setText(stream.readLine(0));
//            sbMacrosInterval->setValue(stream.readLine(0).toInt());
//            path = fileName;
//            QFileInfo fileInfo(file.fileName());
//            bMacros->setText(fileInfo.baseName());
//            emit textChanged(bMacros->text(), index);
//            file.close();
//            cbMacrosActive->setEnabled(true);
//            cbPeriodSending->setEnabled(true);
//            isFromFile = true;
//        }
}

