#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QList>
#include <QString>
#include <QKeySequence>
#include <QShortcut>

#include "MacroRawEditWidget.h"

#include <QDebug>

MacroRawEditWidget::MacroRawEditWidget(QWidget *parent)
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

const QByteArray &MacroRawEditWidget::getData() const
{
    return data;
}

void MacroRawEditWidget::setData(const QByteArray &data)
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

void MacroRawEditWidget::saveSettings(QSettings *settings, int macroIndex)
{
    QString macroIndexString = QString::number(macroIndex);
    settings->setValue("macros/" + macroIndexString + "/raw_edit/mode", mode->currentIndex());
    settings->setValue("macros/" + macroIndexString + "/raw_edit/separator", separator->text());
    settings->setValue("macros/" + macroIndexString + "/raw_edit/position", pos());
    settings->setValue("macros/" + macroIndexString + "/raw_edit/size", size());
}

void MacroRawEditWidget::loadSettings(QSettings *settings, int macroIndex)
{
    QString macroIndexString = QString::number(macroIndex);
    mode->setCurrentIndex(settings->value("macros/" + macroIndexString + "/raw_edit/mode").toInt());
    separator->setText(settings->value("macros/" + macroIndexString + "/raw_edit/separator").toString());
    QPoint pos = settings->value("macros/" + macroIndexString + "/raw_edit/position").toPoint();
    if(!pos.isNull()) {
        move(pos);
    }
    QSize size = settings->value("macros/" + macroIndexString + "/raw_edit/size").toSize();
    if(size.isValid()) {
        resize(size);
    }
}

void MacroRawEditWidget::show()
{
    if(savedWidgetSize.isValid()) {
        resize(savedWidgetSize);
    }
    QWidget::show();
}

void MacroRawEditWidget::view()
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

void MacroRawEditWidget::connections()
{
    connect(mode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MacroRawEditWidget::onSelectMode);
    connect(input, &QPushButton::clicked, this, &MacroRawEditWidget::onRawDataInput);
    connect(input, &QPushButton::clicked, this, &MacroRawEditWidget::close);
    QShortcut *enterSC = new QShortcut(QKeySequence(Qt::Key_Enter), this);
    QShortcut *returnSC = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(enterSC, &QShortcut::activated, this, &MacroRawEditWidget::onRawDataInput);
    connect(returnSC, &QShortcut::activated, this, &MacroRawEditWidget::onRawDataInput);
    connect(enterSC, &QShortcut::activated, this, &MacroRawEditWidget::close);
    connect(returnSC, &QShortcut::activated, this, &MacroRawEditWidget::close);
}

void MacroRawEditWidget::onSelectMode(int mode)
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

void MacroRawEditWidget::onRawDataInput()
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

QString MacroRawEditWidget::fromStringListToString(const QList<QString> &stringList)
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

void MacroRawEditWidget::closeEvent(QCloseEvent *e)
{
    savedWidgetSize = size();
    QWidget::closeEvent(e);
}

