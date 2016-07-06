#include "Macros.h"
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>

Macros::Macros(QWidget *parent)
    : QMainWindow(parent, Qt::WindowCloseButtonHint)
    , widget(new QWidget(this))
    , toolBar(new QToolBar(this))
    , mainLay(new QGridLayout())
    , lbHEX(new QLineEdit(this))
    , lbDEC(new QLineEdit(this))
    , lbASCII(new QLineEdit(this))
    , package(new QLineEdit(this))
    , rbHEX(new QRadioButton("HEX", this))
    , rbDEC(new QRadioButton("DEC", this))
    , rbASCII(new QRadioButton("ASCII", this))
{
    setWindowTitle("RS232 Terminal - Macros: Empty");
    time = 50;
    isFromFile = false;

    addToolBar(Qt::TopToolBarArea, toolBar);
    toolBar->setMovable(false);
    toolBar->addAction("New", this, SLOT(reset()));
    toolBar->addAction("Load", this, SLOT(openDialog()));
    toolBar->addAction("Save", this, SLOT(save()));
    toolBar->addAction("Save as", this, SLOT(saveAs()));

    connect(package, SIGNAL(textChanged(QString)), this, SLOT(compute(QString)));
    connect(rbHEX, SIGNAL(toggled(bool)), this, SLOT(rbChecked()));
    connect(rbDEC, SIGNAL(toggled(bool)), this, SLOT(rbChecked()));
    connect(rbASCII, SIGNAL(toggled(bool)), this, SLOT(rbChecked()));

    widget->setLayout(mainLay);
    setCentralWidget(widget);
    mainLay->setSpacing(3);
    mainLay->setMargin(3);
    mainLay->addWidget(new QLabel("Formats:", this), 0, 0);
    mainLay->addWidget(package, 0, 1);
    mainLay->addWidget(rbHEX, 1, 0);
    mainLay->addWidget(lbHEX, 1, 1);
    mainLay->addWidget(rbASCII, 2, 0);
    mainLay->addWidget(lbASCII, 2, 1);
    mainLay->addWidget(rbDEC, 3, 0);
    mainLay->addWidget(lbDEC, 3, 1);

    lbHEX->setReadOnly(true);
    lbDEC->setReadOnly(true);
    lbASCII->setReadOnly(true);
    lbHEX->setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 9pt");
    lbDEC->setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 9pt");
    lbASCII->setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 9pt");
    package->setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 9pt");
    setMinimumSize(400, 150);
    rbHEX->setChecked(true);
}

void Macros::compute(QString str)
{
    if (str.isEmpty())
    {
        lbDEC->clear();
        lbASCII->clear();
        lbHEX->clear();
        emit act(false);
    }
    if (rbHEX->isChecked())
    {
        QString outDEC;
        QString outASCII;
        QStringList list = str.split(" ", QString::SkipEmptyParts);
        bool ok;
        foreach (QString s, list) {
            int hex = s.toInt(&ok, 16);
            if (ok)
            {
                outDEC.append(QString::number(hex) + " ");
                lbDEC->setText(outDEC);
                QChar ch(hex);
                outASCII.append(ch);
                lbASCII->setText(outASCII);
                lbHEX->setText(str);
            }
        }
    }
    if (rbASCII->isChecked())
    {
        QString outDEC;
        QString outHEX;
        foreach (QChar ch, str) {
            int ascii = ch.toLatin1();
            outDEC.append(QString::number(ascii) + " ");
            lbDEC->setText(outDEC);
            lbASCII->setText(str);
            outHEX.append(QString::number(ascii, 16).toUpper() + " ");
            lbHEX->setText(outHEX);
        }
    }
    if (rbDEC->isChecked())
    {
        QString outASCII;
        QString outHEX;
        QStringList list = str.split(" ", QString::SkipEmptyParts);
        foreach (QString s, list) {
            int dec = s.toInt();
            lbDEC->setText(str);
            QChar ch(dec);
            outASCII.append(ch);
            lbASCII->setText(outASCII);
            QString hex = QString::number(dec, 16).toUpper();
            if (hex.length() < 2)
                hex.insert(0, "0");
            outHEX.append(hex + " ");
            lbHEX->setText(outHEX);
        }
    }
    emit act(true);
}

void Macros::update(int t)
{
    time = t;
}

void Macros::reset()
{
    package->clear();
    path.clear();
    emit upd(false, "Empty", 50);
    setWindowTitle("RS232 Terminal - Macros: Empty");
}

void Macros::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::currentPath()+"/Macros", tr("Macro Files (*.rsmc)"));
        if (fileName != "") {
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not save file"));
                return;
            } else {
                QTextStream stream(&file);
                QString mode;
                if (rbHEX->isChecked())
                    mode = "HEX";
                if (rbASCII->isChecked())
                    mode = "ASCII";
                if (rbDEC->isChecked())
                    mode = "DEC";
                stream << mode + "\n";
                stream.flush();
                stream << package->text() + "\n";
                stream.flush();
                stream << QString::number(time) + "\n";
                stream.flush();
                stream << QString::number(height()) + "\n";
                stream.flush();
                stream << QString::number(width());
                QFileInfo fileInfo(file.fileName());
                emit upd(true, fileInfo.baseName(), time);
                setWindowTitle("RS232 Terminal - Macros: " + fileInfo.baseName());
                file.close();
                path = fileName;
                isFromFile = true;
            }
        }
}

void Macros::save()
{
    if (path.isEmpty())
    {
        saveAs();
    }
    else
    {
            if (path != "") {
                QFile file(path);
                if (!file.open(QIODevice::WriteOnly)) {
                    QMessageBox::critical(this, tr("Error"), tr("Could not save file"));
                    return;
                } else {
                    QTextStream stream(&file);
                    QString mode;
                    if (rbHEX->isChecked())
                        mode = "HEX";
                    if (rbASCII->isChecked())
                        mode = "ASCII";
                    if (rbDEC->isChecked())
                        mode = "DEC";
                    stream << mode + "\n";
                    stream.flush();
                    stream << package->text() + "\n";
                    stream.flush();
                    stream << QString::number(time) + "\n";
                    stream.flush();
                    stream << QString::number(height()) + "\n";
                    stream.flush();
                    stream << QString::number(width());
                    QFileInfo fileInfo(file.fileName());
                    emit upd(true, fileInfo.baseName(), time);
                    setWindowTitle("RS232 Terminal - Macros: " + fileInfo.baseName());
                    file.close();
                    isFromFile = true;
                }
            }
    }
}

void Macros::openDialog()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath()+"/Macros", tr("Macro Files (*.rsmc)"));
        if (fileName != "") {
            QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
                return;
            }
            QTextStream stream(&file);
            QString mode = stream.readLine(0);
            if (mode == "HEX")
                rbHEX->setChecked(true);
            if (mode == "ASCII")
                rbASCII->setChecked(true);
            if (mode == "DEC")
                rbDEC->setChecked(true);
            package->setText(stream.readLine(0));
            time = stream.readLine(0).toInt();
            resize(stream.readLine(0).toInt(), stream.readLine(0).toInt());
            QFileInfo fileInfo(file.fileName());
            emit upd(true, fileInfo.baseName(), time);
            setWindowTitle("RS232 Terminal - Macros: " + fileInfo.baseName());
            file.close();
            path = fileName;
            isFromFile = true;
        }
}

bool Macros::openPath(QString fileName)
{
        if (fileName != "") {
            QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly))
                return false;
            QTextStream stream(&file);
            QString mode = stream.readLine(0);
            if (mode == "HEX")
                rbHEX->setChecked(true);
            if (mode == "ASCII")
                rbASCII->setChecked(true);
            if (mode == "DEC")
                rbDEC->setChecked(true);
            package->setText(stream.readLine(0));
            time = stream.readLine(0).toInt();
            resize(stream.readLine(0).toInt(), stream.readLine(0).toInt());
            QFileInfo fileInfo(file.fileName());
            emit upd(true, fileInfo.baseName(), time);
            setWindowTitle("RS232 Terminal - Macros: " + fileInfo.baseName());
            file.close();
            path = fileName;
            isFromFile = true;
            return true;
        } else
            return false;
}
