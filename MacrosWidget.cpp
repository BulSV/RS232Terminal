#include "MacrosWidget.h"
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>

MacrosWidget::MacrosWidget(QWidget *parent)
    : QMainWindow(parent, Qt::WindowCloseButtonHint)
    , centralWidget(new QWidget(this))
    , toolBar(new QToolBar(this))
    , mainLayout(new QGridLayout())
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

    view();
    connections();
}

void MacrosWidget::compute(QString str)
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

void MacrosWidget::update(unsigned short int t)
{
    time = t;
}

void MacrosWidget::hexChecked()
{
    package->setText(lbHEX->text());
}

void MacrosWidget::asciiChecked()
{
    package->setText(lbASCII->text());
}

void MacrosWidget::decChecked()
{
    package->setText(lbDEC->text());
}

void MacrosWidget::reset()
{
    package->clear();
    path.clear();

    emit upd(false, "Empty", 50);

    setWindowTitle(tr("Terminal - Macros: Empty"));
}

void MacrosWidget::saveAs()
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

void MacrosWidget::save()
{
    if(path.isEmpty()) {
        saveAs();

        return;
    }

    saveToFile(path);
}

void MacrosWidget::openDialog()
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

bool MacrosWidget::openPath(QString fileName)
{
    if(fileName.isEmpty()) {
        return false;
    }

    openFile(fileName);

    return true;
}

void MacrosWidget::saveToFile(const QString &path)
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

void MacrosWidget::openFile(const QString &path)
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

void MacrosWidget::connections()
{
    connect(package, SIGNAL(textChanged(QString)), this, SLOT(compute(QString)));
    connect(rbHEX, SIGNAL(toggled(bool)), this, SLOT(hexChecked()));
    connect(rbDEC, SIGNAL(toggled(bool)), this, SLOT(decChecked()));
    connect(rbASCII, SIGNAL(toggled(bool)), this, SLOT(asciiChecked()));
}

void MacrosWidget::view()
{
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

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
    mainLayout->setSpacing(5);
    mainLayout->addWidget(new QLabel(tr("Formats:"), this), 0, 0);
    mainLayout->addWidget(package, 0, 1);
    mainLayout->addWidget(rbHEX, 1, 0);
    mainLayout->addWidget(lbHEX, 1, 1);
    mainLayout->addWidget(rbASCII, 2, 0);
    mainLayout->addWidget(lbASCII, 2, 1);
    mainLayout->addWidget(rbDEC, 3, 0);
    mainLayout->addWidget(lbDEC, 3, 1);

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
