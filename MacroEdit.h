#ifndef MACRO_EDIT_WIDGET_H
#define MACRO_EDIT_WIDGET_H

#include <QMainWindow>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>
#include <QByteArray>
#include <QCheckBox>
#include <QFileDialog>
#include <QSettings>

#include "DataTable.h"
#include "AsciiEncoder.h"
#include "HexEncoder.h"
#include "DecEncoder.h"
#include "MacroRawEdit.h"
#include "OpenMacro.h"
#include "SaveMacro.h"

class MacroEdit : public QMainWindow
{
    Q_OBJECT
public:
    explicit MacroEdit(QWidget *parent = 0);
    virtual ~MacroEdit();
    const QByteArray &getPackage();
    void saveSettings(QSettings *settings, int macroIndex);
    void loadSettings(QSettings *settings, int macroIndex);
    void openMacroFile(const QString &fileName);
signals:
    void titleChanged(const QString &title);
private:
    QToolBar *toolBar;
    QAction *actionNew;
    QAction *actionLoad;
    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionRawEdit;
    QAction *actionClearSelectedGroup;
    QAction *actionCR;
    QAction *actionLF;
    QAction *actionAddGroup;
    QAction *actionDeleteGroup;
    DataTable *tableAscii;
    DataTable *tableHex;
    DataTable *tableDec;
    QList<DataTable*> macroGroups;
    QVBoxLayout *macroGroupsLayout;
    DataTable *currentEditGroup;
    AsciiEncoder *asciiEncoder;
    HexEncoder *hexEncoder;
    DecEncoder *decEncoder;
    QByteArray package;
    QByteArray package_CR_LF;
    MacroRawEdit *macroRawEdit;
    OpenMacro openMacro;
    SaveMacro saveMacro;
    QString macroOpenDir;
    QString macroSaveDir;
    QFileDialog *fileOpenDialog;
    QFileDialog *fileSaveAsDialog;
    QString macroFileName;

    void connections();
    void view();

    void selectGroup();
    void clearSelectedGroup();
    void addGroup();
    void deleteGroup();
    QList<QString> getLabels() const;
    QList<QString> toUpper(const QList<QString> &source) const;
    void fromAsciiInput();
    void fromHexInput();
    void fromDecInput();
    void nonPrintableCharacters();
    QList<QString> getAsciiString();
    void fillEmptyBytes(DataTable *dataTable);
    QString fromStringListToString(DataTable *dataTable);
    void fillEditGroup();
    void setRawData(const QByteArray &rawData);
    void onEditRawData();
    void formPacket();
    void addCR_LF();
    void newMacroFile();
    void openMacroFile();
    void saveMacroFile();
    void saveAsMacroFile();
};

#endif // MACRO_EDIT_WIDGET_H
