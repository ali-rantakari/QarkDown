#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "defines.h"
#include "qarkdownapplication.h"

#include <QDebug>
#include <QFontDialog>
#include <QColorDialog>
#include <QDesktopServices>
#include <QMessageBox>

PreferencesDialog::PreferencesDialog(QSettings *appSettings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    settings = appSettings;
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    stylesModel = new QStandardItemModel();
    ui->stylesComboBox->setModel(stylesModel);

    ui->openStylesFolderButton->setToolTip(userStylesDir().absolutePath());

#ifdef Q_WS_WIN
    QFont font = ui->infoLabel1->font();
    font.setPointSize(8);
    ui->infoLabel1->setFont(font);
    ui->infoLabel2->setFont(font);
    ui->infoLabel3->setFont(font);
    ui->linkInfoLabel->setFont(font);
#endif

#ifdef Q_WS_MACX
    ui->linkInfoLabel->setText("If enabled, you can click on links while holding "
                               "the Command key.");
#else
    ui->linkInfoLabel->setText("If enabled, you can click on links while holding "
                               "the Ctrl key.");
#endif

    setupConnections();
    updateUIFromSettings();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
    delete stylesModel;
}

void PreferencesDialog::setupConnections()
{
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accepted()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(rejected()));
    connect(ui->fontButton, SIGNAL(clicked()), this, SLOT(fontButtonClicked()));
    connect(ui->openStylesFolderButton, SIGNAL(clicked()),
            this, SLOT(openStylesFolderButtonClicked()));
}

void PreferencesDialog::setFontToLabel(QFont font)
{
    ui->fontLabel->setFont(font);
    QString sizeStr;
    if (font.pixelSize() > -1)
        sizeStr = QVariant(font.pixelSize()).toString()+" px";
    else
        sizeStr = QVariant(font.pointSize()).toString()+" pt";
    ui->fontLabel->setText(font.family()+" "+sizeStr);
}


QDir PreferencesDialog::userStylesDir()
{
    return QDir(((QarkdownApplication *)qApp)->applicationStoragePath() + "/styles/");
}

QStringList PreferencesDialog::userStyleFiles()
{
    QStringList nameFilters;
    nameFilters << "*.style";
    return userStylesDir().entryList(nameFilters);
}



void PreferencesDialog::updateStylesCheckBoxFromSettings()
{
# define STYLABEL(varname, name) \
    QStandardItem *varname = new QStandardItem(name);\
    varname->setSelectable(false);\
    varname->setEnabled(false);\
    rootItem->appendRow(varname)
# define STYITEM(name, data) \
    QStandardItem *item = new QStandardItem(name);\
    item->setData(QVariant(data), Qt::UserRole);\
    rootItem->appendRow(item)

    QString selectedStylePath = settings->value(SETTING_STYLE,
                                                QVariant(DEF_STYLE)).toString();

    stylesModel->clear();
    QStandardItem *rootItem = stylesModel->invisibleRootItem();

    int indexToSelect = 1;
    int i = 0;

    STYLABEL(builtinStylesLabel, "Built-in Styles:");
    i++;

    foreach (QString builtInStyleName, QDir(":/styles/").entryList())
    {
        QString builtinStyleFullPath = QDir(":/styles/" + builtInStyleName).absolutePath();
        STYITEM(builtInStyleName, builtinStyleFullPath);
        if (builtinStyleFullPath == selectedStylePath)
            indexToSelect = i;
        i++;
    }

    QString userStylesDirPath = userStylesDir().absolutePath();
    QStringList userStyles = userStyleFiles();
    if (userStyles.length() > 0)
    {
        STYLABEL(userStylesLabel, "User Styles:");
        i++;

        foreach (QString userStyleFile, userStyles)
        {
            QString userStyleFullPath = QDir(userStylesDirPath + "/" + userStyleFile).absolutePath();
            STYITEM(QFileInfo(userStyleFile).baseName(), userStyleFullPath);
            if (userStyleFullPath == selectedStylePath)
                indexToSelect = i;
            i++;
        }
    }
    ui->stylesComboBox->setCurrentIndex(indexToSelect);
}


// Some helper macros
#define PREF_TO_UI_INT(pref, def, elem) elem->setValue(settings->value(pref, QVariant(def)).toInt())
#define PREF_TO_UI_DOUBLE(pref, def, elem) elem->setValue(settings->value(pref, QVariant(def)).toDouble())
#define PREF_TO_UI_BOOL_CHECKBOX(pref, def, elem) elem->setChecked(settings->value(pref, QVariant(def)).toBool())
#define PREF_TO_UI_BOOL_INVERT_CHECKBOX(pref, def, elem) elem->setChecked(!settings->value(pref, QVariant(def)).toBool())

void PreferencesDialog::updateUIFromSettings()
{
    // font
    QFont font;
    if (settings->contains(SETTING_FONT))
        font.fromString(settings->value(SETTING_FONT).toString());
    else {
        font.setFamily(DEF_FONT_FAMILY);
        font.setPointSize(DEF_FONT_SIZE);
        font.setFixedPitch(true);
    }
    setFontToLabel(font);

    // styles
    updateStylesCheckBoxFromSettings();

    // others
    PREF_TO_UI_INT(SETTING_TAB_WIDTH, DEF_TAB_WIDTH, ui->tabWidthSpinBox);
    PREF_TO_UI_BOOL_INVERT_CHECKBOX(SETTING_INDENT_WITH_TABS, DEF_INDENT_WITH_TABS, ui->tabsWithSpacesCheckBox);
    PREF_TO_UI_DOUBLE(SETTING_HIGHLIGHT_INTERVAL, DEF_HIGHLIGHT_INTERVAL, ui->highlightIntervalSpinBox);
    PREF_TO_UI_BOOL_CHECKBOX(SETTING_REMEMBER_LAST_FILE, DEF_REMEMBER_LAST_FILE, ui->rememberLastFileCheckBox);
    PREF_TO_UI_BOOL_CHECKBOX(SETTING_CLICKABLE_LINKS, DEF_CLICKABLE_LINKS, ui->linksClickableCheckBox);
    PREF_TO_UI_BOOL_CHECKBOX(SETTING_HIGHLIGHT_CURRENT_LINE, DEF_HIGHLIGHT_CURRENT_LINE, ui->highlightLineCheckBox);
}

void PreferencesDialog::updateSettingsFromUI()
{
    settings->setValue(SETTING_FONT, ui->fontLabel->font().toString());
    settings->setValue(SETTING_TAB_WIDTH, ui->tabWidthSpinBox->value());
    settings->setValue(SETTING_HIGHLIGHT_INTERVAL, ui->highlightIntervalSpinBox->value());
    settings->setValue(SETTING_INDENT_WITH_TABS, !ui->tabsWithSpacesCheckBox->isChecked());
    settings->setValue(SETTING_REMEMBER_LAST_FILE, ui->rememberLastFileCheckBox->isChecked());
    settings->setValue(SETTING_CLICKABLE_LINKS, ui->linksClickableCheckBox->isChecked());
    settings->setValue(SETTING_HIGHLIGHT_CURRENT_LINE, ui->highlightLineCheckBox->isChecked());
    settings->setValue(SETTING_STYLE, ui->stylesComboBox->itemData(ui->stylesComboBox->currentIndex()).toString());
    settings->sync();
}

void PreferencesDialog::showEvent(QShowEvent *event)
{
    updateUIFromSettings();
    QDialog::showEvent(event);
}

void PreferencesDialog::fontButtonClicked()
{
    bool ok;
    QFont newFont = QFontDialog::getFont(&ok, ui->fontLabel->font(),
                                         this, tr("Select New Font"));
    if (!ok)
        return;
    setFontToLabel(newFont);
}

void PreferencesDialog::openStylesFolderButtonClicked()
{
    QString stylesDirPath = userStylesDir().absolutePath();

    // Let's make sure the path exists
    if (!QFile::exists(stylesDirPath)) {
        QDir dir;
        dir.mkpath(stylesDirPath);
    }

    bool couldOpen = QDesktopServices::openUrl(QUrl("file:///" + stylesDirPath));
    if (!couldOpen)
        QMessageBox::information(this, "Could not open folder",
                                 "For some reason " + QCoreApplication::applicationName()
                                 + " could not open the folder. You'll have to do it "
                                 + "manually. The path is:\n\n"
                                 + stylesDirPath);
}

void PreferencesDialog::accepted()
{
    updateSettingsFromUI();
    emit updated();
}

void PreferencesDialog::rejected()
{
    updateUIFromSettings();
}
