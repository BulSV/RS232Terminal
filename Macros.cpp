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
    , aCR(new QAction("CR", this))
    , aLF(new QAction("LF", this))
{
    setWindowTitle(tr("Terminal - Macros: Empty"));
    time = 50;
    isFromFile = false;

    addToolBar(Qt::TopToolBarArea, toolBar);
    toolBar->setMovable(false);
    toolBar->addAction(tr("New"), this, SLOT(reset()));
    toolBar->addAction(tr("Load"), this, SLOT(openDialog()));
    toolBar->addAction(tr("Save"), this, SLOT(save()));
    toolBar->addAction(tr("Save as"), this, SLOT(saveAs()));
    aCR->setCheckable(true);
    aLF->setCheckable(true);
    QList<QAction*> actionList;
    actionList << aCR << aLF;
    toolBar->addActions(actionList);

    connect(package, SIGNAL(textChanged(QString)), this, SLOT(compute(QString)));
    connect(rbHEX, SIGNAL(toggled(bool)), this, SLOT(hexChecked()));
    connect(rbDEC, SIGNAL(toggled(bool)), this, SLOT(decChecked()));
    connect(rbASCII, SIGNAL(toggled(bool)), this, SLOT(asciiChecked()));

    widget->setLayout(mainLay);
    setCentralWidget(widget);
    mainLay->setSpacing(5);
    mainLay->addWidget(new QLabel(tr("Formats:"), this), 0, 0);
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
    if(str.isEmpty()) {
        lbDEC->clear();
        lbASCII->clear();
        lbHEX->clear();

        emit act(false);
    }
    if(rbHEX->isChecked()) {
        QString outDEC;
        QString outASCII;
        QStringList list = str.split(" ", QString::SkipEmptyParts);
        bool ok;
        unsigned short int count = list.count();
        for(int i = 0; i < count; i++) {
            int hex = list[i].toInt(&ok, 16);
            if(ok) {
                outDEC.append(QString::number(hex) + " ");
                lbDEC->setText(outDEC);
                QChar ch(hex);
                outASCII.append(ch);
                lbASCII->setText(outASCII);
                lbHEX->setText(str);
            }
        }
    }
    if(rbASCII->isChecked()) {
        QString outDEC;
        QString outHEX;
        unsigned short int count = str.length();
        for(int i = 0; i < count; i++) {
            int ascii = str[i].toLatin1();
            outDEC.append(QString::number(ascii) + " ");
            lbDEC->setText(outDEC);
            lbASCII->setText(str);
            outHEX.append(QString::number(ascii, 16).toUpper() + " ");
            lbHEX->setText(outHEX);
        }
    }
    if(rbDEC->isChecked()) {
        QString outASCII;
        QString outHEX;
        QStringList list = str.split(" ", QString::SkipEmptyParts);
        unsigned short int count = list.count();
        for(int i = 0; i < count; i++) {
            int dec = list[i].toInt();
            lbDEC->setText(str);
            QChar ch(dec);
            outASCII.append(ch);
            lbASCII->setText(outASCII);
            QString hex = QString::number(dec, 16).toUpper();
            if(hex.length() < 2) {
                hex.insert(0, "0");
            }
            outHEX.append(hex + " ");
            lbHEX->setText(outHEX);
        }
    }

    emit act(true);
}

void Macros::update(unsigned short int t)
{
    time = t;
}

void Macros::hexChecked()
{
    package->setText(lbHEX->text());
}

void Macros::asciiChecked()
{
    package->setText(lbASCII->text());
}

void Macros::decChecked()
{
    package->setText(lbDEC->text());
}

void Macros::reset()
{
    package->clear();
    path.clear();

    emit upd(false, "Empty", 50);

    setWindowTitle(tr("Terminal - Macros: Empty"));
}

void Macros::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save File"),
                                                    QDir::currentPath()+"/Macros",
                                                    tr("Macro Files (*.rsmc)"));

    if(fileName.isEmpty()) {
        return;
    }

    saveToFile(fileName);
}

void Macros::save()
{
    if(path.isEmpty()) {
        saveAs();

        return;
    }

    saveToFile(path);
}

void Macros::openDialog()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"),
                                                    QDir::currentPath()+"/Macros",
                                                    tr("Macro Files (*.rsmc)"));
    if(fileName.isEmpty()) {
        return;
    }

    openFile(fileName);
}

bool Macros::openPath(QString fileName)
{
    if(fileName.isEmpty()) {
        return false;
    }

    saveToFile(fileName);

    return true;
}

void Macros::saveToFile(const QString &path)
{
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not save file"));

        return;
    }

    QTextStream stream(&file);
    QString mode;
    if(rbHEX->isChecked()) {
        mode = "HEX";
    }
    if(rbASCII->isChecked()) {
        mode = "ASCII";
    }
    if(rbDEC->isChecked()) {
        mode = "DEC";
    }
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

    setWindowTitle(tr("Terminal - Macros: ") + fileInfo.baseName());
    file.close();
    isFromFile = true;
}

void Macros::openFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file"));

        return;
    }

    QTextStream stream(&file);
    QString mode = stream.readLine(0);
    if(mode == "HEX") {
        rbHEX->setChecked(true);
    }
    if(mode == "ASCII") {
        rbASCII->setChecked(true);
    }
    if(mode == "DEC") {
        rbDEC->setChecked(true);
    }
    package->setText(stream.readLine(0));
    time = stream.readLine(0).toInt();
    resize(stream.readLine(0).toInt(), stream.readLine(0).toInt());
    QFileInfo fileInfo(file.fileName());

    emit upd(true, fileInfo.baseName(), time);

    setWindowTitle(tr("Terminal - Macros: ") + fileInfo.baseName());
    file.close();
    this->path = path;
    isFromFile = true;
}
