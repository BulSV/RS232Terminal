#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QList>
#include <QString>
#include <QKeySequence>
#include <QShortcut>

#include "MacrosRawEditWidget.h"

#include <QDebug>

MacrosRawEditWidget::MacrosRawEditWidget(QWidget *parent)
    : QWidget(parent, Qt::WindowCloseButtonHint)
    , mode(new QComboBox(this))
    , separator(new QLineEdit(this))
    , rawData(new QLineEdit(this))
    , input(new QPushButton(tr("Input"), this))
    , asciiEncoder(new AsciiEncoder)
    , hexEncoder(new HexEncoder)
    , decEncoder(new DecEncoder)
    , currentMode(ASCII)
{
    setWindowModality(Qt::ApplicationModal);
    QList<QString> modes;
    modes << "ASCII" << "HEX" << "DEC";
    mode->addItems(modes);

    separator->setText(" ");
    separator->setEnabled(false);

    view();
    connections();
}

const QByteArray &MacrosRawEditWidget::getData() const
{
    return data;
}

void MacrosRawEditWidget::setData(const QByteArray &data)
{
    this->data = data;
    DataEncoder *dataEncoder = 0;
    switch(currentMode) {
    case ASCII:
        dataEncoder = asciiEncoder;
        break;
    case HEX:
        dataEncoder = hexEncoder;
        break;
    case DEC:
        dataEncoder = decEncoder;
        break;
    }
    dataEncoder->setData(this->data);
    rawData->setText(fromStringListToString(dataEncoder->encodedStringList()));
    rawData->setFocus();
    rawData->selectAll();
}

void MacrosRawEditWidget::saveSettings(QSettings *settings, int macrosIndex)
{
    settings->setValue("macroses/" + QString::number(macrosIndex) + "/raw_edit/mode", mode->currentIndex());
    settings->setValue("macroses/" + QString::number(macrosIndex) + "/raw_edit/separator", separator->text());
    settings->setValue("macroses/" + QString::number(macrosIndex) + "/raw_edit/position", pos());
    settings->setValue("macroses/" + QString::number(macrosIndex) + "/raw_edit/size", size());
}

void MacrosRawEditWidget::loadSettings(QSettings *settings, int macrosIndex)
{
    mode->setCurrentIndex(settings->value("macroses/"+QString::number(macrosIndex)+"/raw_edit/mode").toInt());
    separator->setText(settings->value("macroses/" + QString::number(macrosIndex) + "/raw_edit/separator").toString());
    QPoint pos = settings->value("macroses/" + QString::number(macrosIndex) + "/raw_edit/position").toPoint();
    if(!pos.isNull()) {
        move(pos);
    }
    QSize size = settings->value("macroses/" + QString::number(macrosIndex) + "/raw_edit/size").toSize();
    if(size.isValid()) {
        resize(size);
    }
}

void MacrosRawEditWidget::show()
{
    if(savedWidgetSize.isValid()) {
        resize(savedWidgetSize);
    }
    QWidget::show();
}

void MacrosRawEditWidget::view()
{
    separator->setFixedWidth(50);
    QGridLayout *configLayout = new QGridLayout;
    configLayout->addWidget(new QLabel(tr("Mode"), this), 0, 0);
    configLayout->addWidget(new QLabel(tr("Separator"), this), 0, 1);
    configLayout->addWidget(mode, 1, 0);
    configLayout->addWidget(separator, 1, 1);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addLayout(configLayout, 0, 0, 2, 2);
    mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, 2, 2);
    mainLayout->addWidget(new QLabel(tr("Raw Data"), this), 2, 0, 1, 3);
    mainLayout->addWidget(rawData, 3, 0, 1, 3);
    mainLayout->addWidget(input, 4, 0, 1, 3);
    setLayout(mainLayout);
    setMaximumHeight(minimumHeight());
}

void MacrosRawEditWidget::connections()
{
    connect(mode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MacrosRawEditWidget::onSelectMode);
    connect(input, &QPushButton::clicked, this, &MacrosRawEditWidget::onRawDataInput);
    connect(input, &QPushButton::clicked, this, &MacrosRawEditWidget::close);
    QShortcut *enterSC = new QShortcut(QKeySequence(Qt::Key_Enter), this);
    QShortcut *returnSC = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(enterSC, &QShortcut::activated, this, &MacrosRawEditWidget::onRawDataInput);
    connect(returnSC, &QShortcut::activated, this, &MacrosRawEditWidget::onRawDataInput);
    connect(enterSC, &QShortcut::activated, this, &MacrosRawEditWidget::close);
    connect(returnSC, &QShortcut::activated, this, &MacrosRawEditWidget::close);
}

void MacrosRawEditWidget::onSelectMode(int mode)
{
    switch(mode) {
    case ASCII:
        separator->setEnabled(false);
        break;
    case HEX:
        separator->setEnabled(true);
        break;
    case DEC:
        separator->setEnabled(true);
        break;
    }
    currentMode = mode;
}

void MacrosRawEditWidget::onRawDataInput()
{
    DataEncoder *dataEncoder = 0;
    switch(currentMode) {
    case ASCII:
        dataEncoder = asciiEncoder;
        break;
    case HEX:
        dataEncoder = hexEncoder;
        break;
    case DEC:
        dataEncoder = decEncoder;
        break;
    }
    dataEncoder->setData(rawData->text(), separator->text());
    setData(dataEncoder->encodedByteArray());

    emit dataInputted(getData());
}

QString MacrosRawEditWidget::fromStringListToString(const QList<QString> &stringList)
{
    QString resultString;
    QListIterator<QString> it(stringList);
    while(it.hasNext()) {
        resultString.append(it.next());
        if(currentMode != ASCII) {
            resultString.append(separator->text());
        }
    }
    if(currentMode != ASCII) {
        resultString.chop(separator->text().size());
    }

    return resultString;
}

void MacrosRawEditWidget::closeEvent(QCloseEvent *e)
{
    savedWidgetSize = size();
    QWidget::closeEvent(e);
}

