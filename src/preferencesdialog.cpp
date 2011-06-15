#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "defines.h"

#include <QDebug>
#include <QFontDialog>

PreferencesDialog::PreferencesDialog(QSettings *appSettings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    settings = appSettings;
    ui->setupUi(this);
    setupConnections();
    updateUIFromSettings();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::setupConnections()
{
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accepted()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(rejected()));
    connect(ui->fontButton, SIGNAL(clicked()), this, SLOT(fontButtonClicked()));
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

    // tab width
    int tabWidthInChars = settings->value(SETTING_TAB_WIDTH,
                                          QVariant(DEF_TAB_WIDTH)).toInt();
    ui->tabWidthSpinBox->setValue(tabWidthInChars);

    // indent with tabs
    bool indentWithTabs = settings->value(SETTING_INDENT_WITH_TABS,
                                          QVariant(DEF_INDENT_WITH_TABS)).toBool();
    (indentWithTabs ? ui->indentRadioTabs : ui->indentRadioSpaces)->setChecked(true);

    // highlight interval
    double highlightInterval = settings->value(SETTING_HIGHLIGHT_INTERVAL,
                                               QVariant(DEF_HIGHLIGHT_INTERVAL)).toDouble();
    ui->highlightIntervalSpinBox->setValue(highlightInterval);
}

void PreferencesDialog::updateSettingsFromUI()
{
    settings->setValue(SETTING_FONT, ui->fontLabel->font().toString());
    settings->setValue(SETTING_TAB_WIDTH, ui->tabWidthSpinBox->value());
    settings->setValue(SETTING_HIGHLIGHT_INTERVAL, ui->highlightIntervalSpinBox->value());
    settings->setValue(SETTING_INDENT_WITH_TABS, ui->indentRadioTabs->isChecked());
    settings->sync();
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

void PreferencesDialog::accepted()
{
    updateSettingsFromUI();
    emit updated();
}

void PreferencesDialog::rejected()
{
    updateUIFromSettings();
}
