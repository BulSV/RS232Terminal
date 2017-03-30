#include <QAction>
#include <QGroupBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QGridLayout>
#include <QIntValidator>
#include <QRegExpValidator>
#include <QtMath>
#include <QFileInfo>

#include "MacrosEditWidget.h"

const int TABLE_WIDTH = 250;
const int GROUP_BYTES_COUNT = 8;
const char CR = 0x0A;
const char LF = 0x0D;

#include <QDebug>

MacrosEditWidget::MacrosEditWidget(QWidget *parent)
    : QMainWindow(parent, Qt::WindowCloseButtonHint)
    , toolBar(new QToolBar(this))
    , actionNew(new QAction(tr("New"), this))
    , actionLoad(new QAction(tr("Load"), this))
    , actionSave(new QAction(tr("Save"), this))
    , actionSaveAs(new QAction(tr("Save as"), this))
    , actionRawEdit(new QAction(tr("Raw Edit"), this))
    , actionClearSelectedGroup(new QAction(tr("Clear Selected Group"), this))
    , actionCR(new QAction(tr("CR"), this))
    , actionLF(new QAction(tr("LF"), this))
    , actionAddGroup(new QAction(tr("Add Group"), this))
    , actionDeleteGroup(new QAction(tr("Delete Group"), this))
    , tableAscii(new DataTable(1, this))
    , tableHex(new DataTable(GROUP_BYTES_COUNT, this))
    , tableDec(new DataTable(GROUP_BYTES_COUNT, this))
    , macrosLayout(new QVBoxLayout)
    , asciiEncoder(new AsciiEncoder)
    , hexEncoder(new HexEncoder)
    , decEncoder(new DecEncoder)
    , CR(false)
    , LF(false)
    , macrosRawEditWidget(new MacrosRawEditWidget(0))
    , macrosesOpenDir(".")
    , macrosesSaveDir(".")
    , fileOpenDialog(new QFileDialog(this, tr("Open Macros File"), macrosesOpenDir, "Terminal Macros File (*.tmf)"))
    , fileSaveAsDialog(new QFileDialog(this, tr("Save Macros File"), macrosesOpenDir, "Terminal Macros File (*.tmf)"))
{
    setWindowTitle(tr("Macros Sander - No Name"));

    actionCR->setCheckable(true);
    actionLF->setCheckable(true);

    QFont font;
    font.setFamily("Lucida Console");
    font.setPixelSize(12);
    tableAscii->setFont(font);
    tableHex->setFont(font);
    tableDec->setFont(font);

    tableAscii->setMouseSelectEnable(false);
    tableAscii->setDataAlignment(Qt::AlignLeft);
    tableAscii->setValidator(new QRegExpValidator(QRegExp(".{0,8}"), this));
    tableHex->setValidator(new QRegExpValidator(QRegExp("([0-9]|[a-f]|[A-F]){1,2}"), this));
    tableDec->setValidator(new QIntValidator(0, 255, this));

    tableAscii->setFixedWidth(TABLE_WIDTH);
    tableHex->setFixedWidth(TABLE_WIDTH);
    tableDec->setFixedWidth(TABLE_WIDTH);

    fileSaveAsDialog->setDefaultSuffix("tmf");
    fileSaveAsDialog->setAcceptMode(QFileDialog::AcceptSave);

    addGroup();

    macrosGroups.last()->setSelected(true);

    currentEditGroup = macrosGroups.last();

    macrosLayout->setSpacing(0);
    macrosLayout->setContentsMargins(0, 0, 0, 0);

    view();
    connections();
}

void MacrosEditWidget::addCR_LF()
{
    if(CR) {
        package.append(::CR);
    }
    if(LF) {
        package.append(::LF);
    }
}

void MacrosEditWidget::newMacrosFile()
{
    int groupsCount = macrosGroups.size();
    for(int i = 0; i < groupsCount - 1; ++i) {
        deleteGroup();
    }
    clearSelectedGroup();
    macrosFileName.clear();
    package.clear();
    setWindowTitle(tr("Macros Sander - No Name"));
}

void MacrosEditWidget::openMacrosFile()
{
    openMacrosFile(fileOpenDialog->selectedFiles().first());
}

void MacrosEditWidget::saveMacrosFile()
{
    if(macrosFileName.isEmpty()) {
        fileSaveAsDialog->show();

        return;
    }
    formPackage();
    saveMacros.setData(package);
    saveMacros.save(macrosFileName);
}

void MacrosEditWidget::saveAsMacrosFile()
{
    QString fileName = fileSaveAsDialog->selectedFiles().first();
    if(fileName.isEmpty()) {
        return;
    }
    formPackage();
    saveMacros.setData(package);
    if(!saveMacros.save(fileName)) {
        return;
    }
    macrosFileName = fileName;
    QFileInfo fileInfo(fileName);
    fileName = fileInfo.fileName();
    fileName.chop(4);
    setWindowTitle(tr("Macros Sander - ") + fileName);
}

void MacrosEditWidget::formPackage()
{
    QListIterator<DataTable*> itDataTable(macrosGroups);
    QString hexData;
    while(itDataTable.hasNext()) {
        QListIterator<QString> itHexData(itDataTable.next()->getData());
        while(itHexData.hasNext()) {
            QString tempString = itHexData.next();
            if(tempString.isEmpty()) {
                break;
            }
            hexData.append(tempString);
            hexData.append(" ");
        }
    }
    hexData.chop(1);
    hexEncoder->setData(hexData, " ");
    package = hexEncoder->encodedByteArray();
}

const QByteArray &MacrosEditWidget::getPackage()
{
    addCR_LF();

    return package;
}

void MacrosEditWidget::saveSettings(QSettings *settings, int macrosIndex)
{
    QString macrosIndexString = QString::number(macrosIndex);
    if(macrosFileName.isEmpty()) {
        formPackage();
        QString packageString;
        int packageSize = package.size();
        for(int i = 0; i < packageSize; ++i) {
            packageString.append(QString(package.at(i)));
            packageString.append(" ");
        }
        settings->setValue("macroses/" + macrosIndexString + "/package", packageString);
    } else {
        settings->setValue("macroses/" + macrosIndexString + "/path", macrosFileName);
    }
    settings->setValue("macroses/" + macrosIndexString + "/position", pos());
    settings->setValue("macroses/" + macrosIndexString + "/size", size());

    macrosRawEditWidget->saveSettings(settings, macrosIndex);
}

void MacrosEditWidget::loadSettings(QSettings *settings, int macrosIndex)
{
    QString macrosIndexString = QString::number(macrosIndex);
    QString fileName = settings->value("macroses/" + macrosIndexString + "/path").toString();
    if(!fileName.isEmpty()) {
        if(!openMacros.open(fileName)) {
            return;
        }
        macrosFileName = fileName;
        setRawData(openMacros.getData());
        QFileInfo fileInfo(fileName);
        fileName = fileInfo.fileName();
        fileName.chop(4);
        setWindowTitle(tr("Macros Sander - ") + fileName);
    } else {
        QList<QString> packageList = settings->value("macroses/" + macrosIndexString + "/package").toString().split(" ");
        int dataSize = packageList.size();
        bool ok;
        for(int i = 0; i < dataSize; ++i) {
            package.append(static_cast<char>(packageList.at(i).toInt(&ok, 16)));
        }
    }
    actionCR->setChecked(settings->value("macroses/" + macrosIndexString + "/CR").toBool());
    actionLF->setChecked(settings->value("macroses/" + macrosIndexString + "/LF").toBool());
    QPoint pos = settings->value("macroses/" + macrosIndexString + "/position").toPoint();
    if(!pos.isNull()) {
        move(pos);
    }
    QSize size = settings->value("macroses/" + macrosIndexString + "/size").toSize();
    if(size.isValid()) {
        resize(size);
    }

    macrosRawEditWidget->loadSettings(settings, macrosIndex);
}

void MacrosEditWidget::openMacrosFile(const QString &fileName)
{
    if(fileName.isEmpty()) {
        return;
    }
    if(!openMacros.open(fileName)) {
        return;
    }
    macrosFileName = fileName;
    setRawData(openMacros.getData());
    QFileInfo fileInfo(fileName);
    QString title = fileInfo.fileName();
    title.chop(4);
    setWindowTitle(tr("Macros Sander - ") + title);
}

void MacrosEditWidget::connections()
{
    connect(actionNew, &QAction::triggered, this, &MacrosEditWidget::newMacrosFile);

    connect(actionLoad, &QAction::triggered, fileOpenDialog, &QFileDialog::show);
    connect(fileOpenDialog, &QFileDialog::accepted, [this](){openMacrosFile();});

    connect(actionSave, &QAction::triggered, this, &MacrosEditWidget::saveMacrosFile);

    connect(actionSaveAs, &QAction::triggered, fileSaveAsDialog, &MacrosEditWidget::show);
    connect(fileSaveAsDialog, &QFileDialog::accepted, this, &MacrosEditWidget::saveAsMacrosFile);

    connect(actionRawEdit, &QAction::triggered, this, &MacrosEditWidget::onEditRawData);
    connect(actionClearSelectedGroup, &QAction::triggered, this, &MacrosEditWidget::clearSelectedGroup);
    connect(actionCR, &QAction::triggered, this, &MacrosEditWidget::addCR);
    connect(actionLF, &QAction::triggered, this, &MacrosEditWidget::addLF);
    connect(actionAddGroup, &QAction::triggered, this, &MacrosEditWidget::addGroup);
    connect(actionDeleteGroup, &QAction::triggered, this, &MacrosEditWidget::deleteGroup);

    connect(tableAscii, &DataTable::editingFinished, this, &MacrosEditWidget::fromAsciiInput);
    connect(tableHex, &DataTable::editingFinished, this, &MacrosEditWidget::fromHexInput);
    connect(tableDec, &DataTable::editingFinished, this, &MacrosEditWidget::fromDecInput);

    connect(macrosRawEditWidget, &MacrosRawEditWidget::dataInputted, this, &MacrosEditWidget::setRawData);
}

void MacrosEditWidget::view()
{
    QList<QAction*> actions;
    actions << actionNew
            << actionLoad
            << actionSave
            << actionSaveAs
            << actionRawEdit
            << actionClearSelectedGroup
            << actionCR
            << actionLF
            << actionAddGroup
            << actionDeleteGroup;
    addToolBar(Qt::TopToolBarArea, toolBar);
    toolBar->setMovable(false);
    toolBar->addActions(actions);

    QGridLayout *editGroupLayout = new QGridLayout;
    editGroupLayout->setContentsMargins(5, 5, 5, 5);
    editGroupLayout->addWidget(new QLabel("ASCII", this), 1, 0, Qt::AlignTop);
    editGroupLayout->addWidget(tableAscii, 0, 1, 2, 8, Qt::AlignVCenter);
    editGroupLayout->addWidget(new QLabel("HEX", this), 3, 0, Qt::AlignTop);
    editGroupLayout->addWidget(tableHex, 2, 1, 2, 8, Qt::AlignVCenter);
    editGroupLayout->addWidget(new QLabel("DEC", this), 5, 0, Qt::AlignTop);
    editGroupLayout->addWidget(tableDec, 4, 1, 2, 8, Qt::AlignVCenter);

    QGroupBox *editGroup = new QGroupBox(tr("Edit selected group"), this);
    editGroup->setLayout(editGroupLayout);

    macrosLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QWidget *scrollWidget = new QWidget(this);
    scrollWidget->setLayout(macrosLayout);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(scrollWidget);
    scrollArea->setVerticalScrollBar(new QScrollBar(Qt::Vertical, scrollArea));
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setFixedWidth(TABLE_WIDTH + 30);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout *scrollLayout = new QVBoxLayout;
    scrollLayout->setContentsMargins(5, 5, 5, 5);
    scrollLayout->addWidget(scrollArea);

    QGroupBox *packageGroups = new QGroupBox(tr("Package groups"), this);
    packageGroups->setLayout(scrollLayout);

    QGridLayout *editLayout = new QGridLayout;
    editLayout->setSpacing(5);
    editLayout->addWidget(editGroup, 0, 0, 6, 9, Qt::AlignCenter);
    editLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 6, 0);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addItem(editLayout, 0, 0, 8, 9);
    mainLayout->addWidget(packageGroups, 0, 9, 8, 9);
    mainLayout->setSpacing(5);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    setMaximumWidth(minimumWidth());
}

void MacrosEditWidget::selectGroup()
{
    DataTable *group = qobject_cast<DataTable*>(sender());
    if(group == 0) {
        return;
    }
    currentEditGroup = group;
    fillEditGroup();
}

void MacrosEditWidget::clearSelectedGroup()
{
    tableAscii->clearData();
    tableHex->clearData();
    tableDec->clearData();
    currentEditGroup->clearData();
}

void MacrosEditWidget::addGroup()
{
    DataTable *group = new DataTable(GROUP_BYTES_COUNT, this);
    group->setFixedWidth(TABLE_WIDTH);
    group->setReadOnly(true);
    group->setMouseSelectEnable(false);
    group->setLabels(getLabels());
    QFont font;
    font.setFamily("Lucida Console");
    font.setPixelSize(12);
    group->setFont(font);
    connect(group, &DataTable::tableSelected, this, &MacrosEditWidget::selectGroup);

    macrosGroups.append(group);
    macrosLayout->insertWidget(macrosLayout->count() - 1, group, 0, Qt::AlignCenter);
}

void MacrosEditWidget::deleteGroup()
{
    if(macrosGroups.size() < 2) {
        return;
    }
    if(currentEditGroup == macrosGroups.last()) {
        currentEditGroup = macrosGroups[macrosGroups.size() - 2];
        fillEditGroup();

    }
    disconnect(macrosGroups.last(), &DataTable::tableSelected, this, &MacrosEditWidget::selectGroup);
    macrosLayout->removeWidget(macrosGroups.last());
    delete macrosGroups.takeLast();
}

QList<QString> MacrosEditWidget::getLabels() const
{
    QList<QString> labels;
    int index = macrosGroups.size() * GROUP_BYTES_COUNT + 1;
    for(int i = index; i < index + GROUP_BYTES_COUNT; ++i) {
        labels.append(QString::number(i));
    }

    return labels;
}

QList<QString> MacrosEditWidget::toUpper(const QList<QString> &source) const
{
    QList<QString> result;
    QListIterator<QString> it(source);
    QString tempString;
    while(it.hasNext()) {
        tempString = it.next();
        if(tempString.toInt() < 0x10 && tempString.size() == 1) {
            result.append("0" + tempString.toUpper());

            continue;
        }
        result.append(tempString.toUpper());
    }

    return result;
}

void MacrosEditWidget::fromAsciiInput()
{
    QListIterator<QString> it(tableAscii->getData());
    QString asciiData;
    while(it.hasNext()) {
        asciiData.append(it.next());
    }
    asciiEncoder->setData(asciiData);
    hexEncoder->setData(asciiEncoder->encodedByteArray());
    decEncoder->setData(asciiEncoder->encodedByteArray());
    tableHex->setData(hexEncoder->encodedStringList());
    tableDec->setData(decEncoder->encodedStringList());
    tableHex->setData(toUpper(tableHex->getData()));
    currentEditGroup->setData(tableHex->getData());

    formPackage();
}

void MacrosEditWidget::fromHexInput()
{
    fillEmptyBytes(tableHex);
    hexEncoder->setData(fromStringListToString(tableHex), " ");
    asciiEncoder->setData(hexEncoder->encodedByteArray());
    decEncoder->setData(hexEncoder->encodedByteArray());
    tableAscii->setData(getAsciiString());
    nonPrintableCharacters();
    tableDec->setData(decEncoder->encodedStringList());
    tableHex->setData(toUpper(tableHex->getData()));
    currentEditGroup->setData(tableHex->getData());

    formPackage();
}

void MacrosEditWidget::fromDecInput()
{
    fillEmptyBytes(tableDec);
    decEncoder->setData(fromStringListToString(tableDec), " ");
    asciiEncoder->setData(decEncoder->encodedByteArray());
    hexEncoder->setData(decEncoder->encodedByteArray());
    tableAscii->setData(getAsciiString());
    nonPrintableCharacters();
    tableHex->setData(hexEncoder->encodedStringList());
    tableHex->setData(toUpper(tableHex->getData()));
    currentEditGroup->setData(tableHex->getData());

    formPackage();
}

void MacrosEditWidget::nonPrintableCharacters()
{
    bool wasNonPrintingChar = false;
    connect(tableAscii, &DataTable::editingFinished, this, &MacrosEditWidget::fromAsciiInput);
    QString asciiString = tableAscii->getData().first();
    for(int i = 0; i < asciiString.size(); ++i) {
        if(!asciiString.at(i).isPrint()) {
            wasNonPrintingChar = true;
        }
    }
    tableAscii->setReadOnly(wasNonPrintingChar);
    if(wasNonPrintingChar) {
        disconnect(tableAscii, &DataTable::editingFinished, this, &MacrosEditWidget::fromAsciiInput);
    }
}

QList<QString> MacrosEditWidget::getAsciiString()
{
    QString asciiString;
    QListIterator<QString> itAscii(asciiEncoder->encodedStringList());
    while(itAscii.hasNext()) {
        asciiString.append(itAscii.next());
    }
    QList<QString> asciiList;
    asciiList.append(asciiString);

    return asciiList;
}

void MacrosEditWidget::fillEmptyBytes(DataTable *dataTable)
{
    QListIterator<QString> it(dataTable->getData());
    QString dataString;
    QList<QString> result;
    bool wasNotEmpty = false;
    it.toBack();
    while(it.hasPrevious()) {
        dataString = it.previous();
        if(dataString.isEmpty() && !wasNotEmpty) {
            continue;
        }
        wasNotEmpty = true;
        if(dataString.isEmpty()) {
            result.prepend(QString::number(0));

            continue;
        }
        result.prepend(dataString);
    }
    dataTable->setData(result);
}

void MacrosEditWidget::addCR(bool CR)
{
    this->CR = CR;
}

void MacrosEditWidget::addLF(bool LF)
{
    this->LF = LF;
}

QString MacrosEditWidget::fromStringListToString(DataTable *dataTable)
{
    QListIterator<QString> it(dataTable->getData());
    QString data;
    while(it.hasNext()) {
        data.append(it.next());
        data.append(" ");
    }
    data.chop(1);

    return data;
}

void MacrosEditWidget::fillEditGroup()
{
    QList<QString> labels;
    labels.append(currentEditGroup->getLabels().first() + "-" + currentEditGroup->getLabels().last());
    tableAscii->setLabels(labels);
    tableAscii->clearData();
    tableHex->setLabels(currentEditGroup->getLabels());
    tableHex->clearData();
    tableHex->setData(currentEditGroup->getData());
    tableDec->setLabels(currentEditGroup->getLabels());
    tableDec->clearData();
    fromHexInput();
}

void MacrosEditWidget::setRawData(const QByteArray &rawData)
{
    currentEditGroup->setFocus();

    int rawDataGroupsCount = qCeil(static_cast<qreal>(rawData.size()) / GROUP_BYTES_COUNT);
    int dataGroupsCount = macrosGroups.size();
    if(dataGroupsCount < rawDataGroupsCount) {
        for(int i = dataGroupsCount; i < rawDataGroupsCount; ++i) {
            addGroup();
        }
    }
    if(dataGroupsCount > rawDataGroupsCount) {
        for(int i = rawDataGroupsCount; i < dataGroupsCount; ++i) {
            deleteGroup();
        }
    }
    QListIterator<DataTable*> it(macrosGroups);
    hexEncoder->setData(rawData);
    QList<QString> dataList = hexEncoder->encodedStringList();
    int i = 0;
    while(it.hasNext()) {
        it.next()->setData(dataList.mid(i, GROUP_BYTES_COUNT));
        i += 8;
    }
}

void MacrosEditWidget::onEditRawData()
{
    formPackage();
    macrosRawEditWidget->setData(package);
    macrosRawEditWidget->setWindowTitle(windowTitle());
    macrosRawEditWidget->show();
}

void MacrosEditWidget::closeEvent(QCloseEvent *e)
{
    delete macrosRawEditWidget;
    macrosRawEditWidget = 0;

    QMainWindow::closeEvent(e);
}
