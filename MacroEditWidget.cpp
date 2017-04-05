#include <QAction>
#include <QGroupBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QGridLayout>
#include <QIntValidator>
#include <QRegExpValidator>
#include <QtMath>
#include <QFileInfo>

#include "MacroEditWidget.h"

const int TABLE_WIDTH = 250;
const int GROUP_BYTES_COUNT = 8;
const char CR = 0x0A;
const char LF = 0x0D;

#include <QDebug>

MacroEditWidget::MacroEditWidget(QWidget *parent)
    : QMainWindow(parent, Qt::WindowCloseButtonHint)
    , toolBar(new QToolBar(this))
    , actionNew(new QAction(QIcon(":/Resources/New.png"), tr("New"), this))
    , actionLoad(new QAction(QIcon(":/Resources/Open.png"), tr("Load"), this))
    , actionSave(new QAction(QIcon(":/Resources/Save.png"), tr("Save"), this))
    , actionSaveAs(new QAction(QIcon(":/Resources/SaveAs.png"), tr("Save as"), this))
    , actionRawEdit(new QAction(QIcon(":/Resources/RawEdit.png"), tr("Raw Edit"), this))
    , actionClearSelectedGroup(new QAction(QIcon(":/Resources/Clear.png"), tr("Clear Selected Group"), this))
    , actionCR(new QAction(QIcon(":/Resources/CR.png"), tr("CR"), this))
    , actionLF(new QAction(QIcon(":/Resources/LF.png"), tr("LF"), this))
    , actionAddGroup(new QAction(QIcon(":/Resources/Add.png"), tr("Add Group"), this))
    , actionDeleteGroup(new QAction(QIcon(":/Resources/Delete.png"), tr("Delete Group"), this))
    , tableAscii(new DataTable(1, this))
    , tableHex(new DataTable(GROUP_BYTES_COUNT, this))
    , tableDec(new DataTable(GROUP_BYTES_COUNT, this))
    , macroGroupsLayout(new QVBoxLayout)
    , asciiEncoder(new AsciiEncoder)
    , hexEncoder(new HexEncoder)
    , decEncoder(new DecEncoder)
    , macroRawEditWidget(new MacroRawEditWidget(0))
    , macroOpenDir(".")
    , macroSaveDir(".")
    , fileOpenDialog(new QFileDialog(this, tr("Open Macro File"), macroOpenDir, "Terminal Macro File (*.tmf)"))
    , fileSaveAsDialog(new QFileDialog(this, tr("Save Macro File"), macroSaveDir, "Terminal Macro File (*.tmf)"))
{
    setWindowTitle(tr("Macro Edit - No Name"));

    emit titleChanged(QString(tr("No Name")));

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

    macroGroups.last()->setSelected(true);

    currentEditGroup = macroGroups.last();

    macroGroupsLayout->setSpacing(0);
    macroGroupsLayout->setContentsMargins(0, 0, 0, 0);

    view();
    connections();
}

MacroEditWidget::~MacroEditWidget()
{
    delete macroRawEditWidget;
    macroRawEditWidget = 0;
}

void MacroEditWidget::addCR_LF()
{
    package_CR_LF = package;
    if(actionCR->isChecked()) {
        package_CR_LF.append(CR);
    }
    if(actionLF->isChecked()) {
        package_CR_LF.append(LF);
    }
}

void MacroEditWidget::newMacroFile()
{
    int groupsCount = macroGroups.size();
    for(int i = 0; i < groupsCount - 1; ++i) {
        deleteGroup();
    }
    clearSelectedGroup();
    macroFileName.clear();
    package.clear();
    setWindowTitle(tr("Macro Edit - No Name"));

    emit titleChanged(QString(tr("No Name")));
}

void MacroEditWidget::openMacroFile()
{
    openMacroFile(fileOpenDialog->selectedFiles().first());
}

void MacroEditWidget::saveMacroFile()
{
    if(macroFileName.isEmpty()) {
        fileSaveAsDialog->show();

        return;
    }
    formPackage();
    saveMacro.setData(package);
    saveMacro.save(macroFileName);
}

void MacroEditWidget::saveAsMacroFile()
{
    QString fileName = fileSaveAsDialog->selectedFiles().first();
    if(fileName.isEmpty()) {
        return;
    }
    formPackage();
    saveMacro.setData(package);
    if(!saveMacro.save(fileName)) {
        return;
    }
    macroFileName = fileName;
    QFileInfo fileInfo(fileName);
    fileName = fileInfo.fileName();
    fileName.chop(4);
    setWindowTitle(tr("Macro Edit - ") + fileName);

    emit titleChanged(fileName);
}

void MacroEditWidget::formPackage()
{
    QListIterator<DataTable*> itMacroGroups(macroGroups);
    QString hexData;
    while(itMacroGroups.hasNext()) {
        QListIterator<QString> itHexData(itMacroGroups.next()->getData());
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

const QByteArray &MacroEditWidget::getPackage()
{
    addCR_LF();

    return package_CR_LF;
}

void MacroEditWidget::saveSettings(QSettings *settings, int macroIndex)
{
    QString macroIndexString = QString::number(macroIndex);
    if(macroFileName.isEmpty()) {
        formPackage();
        QString packageString;
        int packageSize = package.size();
        for(int i = 0; i < packageSize; ++i) {
            packageString.append(QString::number((unsigned char)package.at(i), 16).toUpper());
            packageString.append(" ");
        }
        packageString.chop(1);
        settings->setValue("macros/" + macroIndexString + "/package", packageString);
    } else {
        settings->setValue("macros/" + macroIndexString + "/path", macroFileName);
    }
    settings->setValue("macros/" + macroIndexString + "/CR", actionCR->isChecked());
    settings->setValue("macros/" + macroIndexString + "/LF", actionLF->isChecked());
    settings->setValue("macros/" + macroIndexString + "/position", pos());
    settings->setValue("macros/" + macroIndexString + "/size", size());

    macroRawEditWidget->saveSettings(settings, macroIndex);
}

void MacroEditWidget::loadSettings(QSettings *settings, int macroIndex)
{
    QString macroIndexString = QString::number(macroIndex);
    QString fileName = settings->value("macros/" + macroIndexString + "/path").toString();
    if(!fileName.isEmpty()) {
        if(!openMacro.open(fileName)) {
            return;
        }
        macroFileName = fileName;
        setRawData(openMacro.getData());
        QFileInfo fileInfo(fileName);
        fileName = fileInfo.fileName();
        fileName.chop(4);
        setWindowTitle(tr("Macro Edit - ") + fileName);

        emit titleChanged(fileName);
    } else {
        QList<QString> packageList = settings->value("macros/" + macroIndexString + "/package").toString().split(" ");
        int dataSize = packageList.size();
        bool ok;
        for(int i = 0; i < dataSize; ++i) {
            package.append(static_cast<char>(packageList.at(i).toInt(&ok, 16)));
        }
        setRawData(package);
    }
    actionCR->setChecked(settings->value("macros/" + macroIndexString + "/CR").toBool());
    actionLF->setChecked(settings->value("macros/" + macroIndexString + "/LF").toBool());
    QPoint pos = settings->value("macros/" + macroIndexString + "/position").toPoint();
    if(!pos.isNull()) {
        move(pos);
    }
    QSize size = settings->value("macros/" + macroIndexString + "/size").toSize();
    if(size.isValid()) {
        resize(size);
    }

    macroRawEditWidget->loadSettings(settings, macroIndex);
}

void MacroEditWidget::openMacroFile(const QString &fileName)
{
    if(fileName.isEmpty()) {
        return;
    }
    if(!openMacro.open(fileName)) {
        return;
    }
    macroFileName = fileName;
    setRawData(openMacro.getData());
    QFileInfo fileInfo(fileName);
    QString title = fileInfo.fileName();
    title.chop(4);
    setWindowTitle(tr("Macro Edit - ") + title);

    emit titleChanged(title);
}

void MacroEditWidget::connections()
{
    connect(actionNew, &QAction::triggered, this, &MacroEditWidget::newMacroFile);

    connect(actionLoad, &QAction::triggered, fileOpenDialog, &QFileDialog::show);
    connect(fileOpenDialog, &QFileDialog::accepted, [this](){openMacroFile();});

    connect(actionSave, &QAction::triggered, this, &MacroEditWidget::saveMacroFile);

    connect(actionSaveAs, &QAction::triggered, fileSaveAsDialog, &MacroEditWidget::show);
    connect(fileSaveAsDialog, &QFileDialog::accepted, this, &MacroEditWidget::saveAsMacroFile);

    connect(actionRawEdit, &QAction::triggered, this, &MacroEditWidget::onEditRawData);
    connect(actionClearSelectedGroup, &QAction::triggered, this, &MacroEditWidget::clearSelectedGroup);
    connect(actionAddGroup, &QAction::triggered, this, &MacroEditWidget::addGroup);
    connect(actionDeleteGroup, &QAction::triggered, this, &MacroEditWidget::deleteGroup);

    connect(tableAscii, &DataTable::editingFinished, this, &MacroEditWidget::fromAsciiInput);
    connect(tableHex, &DataTable::editingFinished, this, &MacroEditWidget::fromHexInput);
    connect(tableDec, &DataTable::editingFinished, this, &MacroEditWidget::fromDecInput);

    connect(macroRawEditWidget, &MacroRawEditWidget::dataInputted, this, &MacroEditWidget::setRawData);
}

void MacroEditWidget::view()
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

    macroGroupsLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QWidget *scrollWidget = new QWidget(this);
    scrollWidget->setLayout(macroGroupsLayout);

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

void MacroEditWidget::selectGroup()
{
    DataTable *group = qobject_cast<DataTable*>(sender());
    if(group == 0) {
        return;
    }
    currentEditGroup = group;
    fillEditGroup();
}

void MacroEditWidget::clearSelectedGroup()
{
    tableAscii->clearData();
    tableHex->clearData();
    tableDec->clearData();
    currentEditGroup->clearData();
}

void MacroEditWidget::addGroup()
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
    connect(group, &DataTable::tableSelected, this, &MacroEditWidget::selectGroup);

    macroGroups.append(group);
    macroGroupsLayout->insertWidget(macroGroupsLayout->count() - 1, group, 0, Qt::AlignCenter);
}

void MacroEditWidget::deleteGroup()
{
    if(macroGroups.size() < 2) {
        return;
    }
    if(currentEditGroup == macroGroups.last()) {
        currentEditGroup = macroGroups[macroGroups.size() - 2];
        fillEditGroup();

    }
    disconnect(macroGroups.last(), &DataTable::tableSelected, this, &MacroEditWidget::selectGroup);
    macroGroupsLayout->removeWidget(macroGroups.last());
    delete macroGroups.takeLast();
}

QList<QString> MacroEditWidget::getLabels() const
{
    QList<QString> labels;
    int index = macroGroups.size() * GROUP_BYTES_COUNT + 1;
    for(int i = index; i < index + GROUP_BYTES_COUNT; ++i) {
        labels.append(QString::number(i));
    }

    return labels;
}

QList<QString> MacroEditWidget::toUpper(const QList<QString> &source) const
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

void MacroEditWidget::fromAsciiInput()
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

void MacroEditWidget::fromHexInput()
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

void MacroEditWidget::fromDecInput()
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

void MacroEditWidget::nonPrintableCharacters()
{
    bool wasNonPrintingChar = false;
    connect(tableAscii, &DataTable::editingFinished, this, &MacroEditWidget::fromAsciiInput);
    QString asciiString = tableAscii->getData().first();
    for(int i = 0; i < asciiString.size(); ++i) {
        if(!asciiString.at(i).isPrint()) {
            wasNonPrintingChar = true;
        }
    }
    tableAscii->setReadOnly(wasNonPrintingChar);
    if(wasNonPrintingChar) {
        disconnect(tableAscii, &DataTable::editingFinished, this, &MacroEditWidget::fromAsciiInput);
    }
}

QList<QString> MacroEditWidget::getAsciiString()
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

void MacroEditWidget::fillEmptyBytes(DataTable *dataTable)
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

QString MacroEditWidget::fromStringListToString(DataTable *dataTable)
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

void MacroEditWidget::fillEditGroup()
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

void MacroEditWidget::setRawData(const QByteArray &rawData)
{
    currentEditGroup->setFocus();

    int rawDataGroupsCount = qCeil(static_cast<qreal>(rawData.size()) / GROUP_BYTES_COUNT);
    int dataGroupsCount = macroGroups.size();
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
    QListIterator<DataTable*> it(macroGroups);
    hexEncoder->setData(rawData);
    QList<QString> dataList = hexEncoder->encodedStringList();
    int i = 0;
    while(it.hasNext()) {
        it.next()->setData(dataList.mid(i, GROUP_BYTES_COUNT));
        i += 8;
    }
    currentEditGroup = macroGroups.first();
    fillEditGroup();
}

void MacroEditWidget::onEditRawData()
{
    formPackage();
    macroRawEditWidget->setData(package);
    macroRawEditWidget->setWindowTitle(windowTitle());
    macroRawEditWidget->show();
}
