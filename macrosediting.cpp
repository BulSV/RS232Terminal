#include "MacrosEditing.h"
#include "Macros.h"
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

MacrosEditing::MacrosEditing(QString title, QString p, int i, QWidget *parent)
    : QWidget(parent, Qt::WindowCloseButtonHint)
    , load(new QPushButton("Load", this))
    , save(new QPushButton("Save", this))
    , package(new QLineEdit(this))
    , interval(new QSpinBox(this))
    , mainLay(new QHBoxLayout(this))
{
    setWindowTitle("RS232 Terminal - Macro: " + title);
    mainLay->addWidget(load);
    mainLay->addWidget(save);
    mainLay->addWidget(package);
    mainLay->addWidget(interval);
    mainLay->setSpacing(3);
    mainLay->setMargin(3);
    setLayout(mainLay);
    setMaximumHeight(sizeHint().height());
    resize(500, height());

    package->setText(p);
    interval->setValue(i);

    connect(save, SIGNAL(clicked(bool)), this, SLOT(saveDialog()));
}

void MacrosEditing::saveDialog()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), tr("Macro Files (*.rsmc)"));

        if (fileName != "") {
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not save file"));
                return;
            } else {
                QTextStream stream(&file);
                stream << package->text()+"\n";
                stream.flush();
                stream << interval->value();
                stream.flush();
                file.close();
            }
        }
}

