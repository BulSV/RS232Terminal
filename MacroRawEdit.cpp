#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QList>
#include <QString>
#include <QKeySequence>
#include <QShortcut>

#include "MacroRawEdit.h"

#include <QDebug>

MacroRawEdit::MacroRawEdit(QWidget *parent)
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

const QByteArray &MacroRawEdit::getData() const
{
    return data;
}

void MacroRawEdit::setData(const QByteArray &data)
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

void MacroRawEdit::saveSettings(QSettings *settings, int macroIndex)
{
    QString macroIndexString = QString::number(macroIndex);
    settings->setValue("macros/" + macroIndexString + "/raw_edit/mode", mode->currentIndex());
    settings->setValue("macros/" + macroIndexString + "/raw_edit/separator", separator->text());
    settings->setValue("macros/" + macroIndexString + "/raw_edit/position", pos());
    settings->setValue("macros/" + macroIndexString + "/raw_edit/size", size());
}

void MacroRawEdit::loadSettings(QSettings *settings, int macroIndex)
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

void MacroRawEdit::show()
{
    if(savedWidgetSize.isValid()) {
        resize(savedWidgetSize);
    }
    QWidget::show();
}

void MacroRawEdit::view()
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

void MacroRawEdit::connections()
{
    connect(mode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MacroRawEdit::onSelectMode);
    connect(input, &QPushButton::clicked, this, &MacroRawEdit::onRawDataInput);
    connect(input, &QPushButton::clicked, this, &MacroRawEdit::close);
    QShortcut *enterSC = new QShortcut(QKeySequence(Qt::Key_Enter), this);
    QShortcut *returnSC = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(enterSC, &QShortcut::activated, this, &MacroRawEdit::onRawDataInput);
    connect(returnSC, &QShortcut::activated, this, &MacroRawEdit::onRawDataInput);
    connect(enterSC, &QShortcut::activated, this, &MacroRawEdit::close);
    connect(returnSC, &QShortcut::activated, this, &MacroRawEdit::close);
}

void MacroRawEdit::onSelectMode(int mode)
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

void MacroRawEdit::onRawDataInput()
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

QString MacroRawEdit::fromStringListToString(const QList<QString> &stringList)
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

void MacroRawEdit::closeEvent(QCloseEvent *e)
{
    savedWidgetSize = size();
    QWidget::closeEvent(e);
}

