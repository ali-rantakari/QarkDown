#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "defines.h"

#include <QDebug>
#include <QFontDialog>
#include <QColorDialog>

PreferencesDialog::PreferencesDialog(QSettings *appSettings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    settings = appSettings;
    ui->setupUi(this);

#ifdef Q_WS_WIN
    QFont font = ui->infoLabel1->font();
    font.setPointSize(7);
    ui->infoLabel1->setFont(font);
#endif

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
    connect(ui->highightLineColorButton, SIGNAL(clicked()), this, SLOT(lineHighlightColorButtonClicked()));
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
    ui->tabsWithSpacesCheckBox->setChecked(indentWithTabs);

    // highlight interval
    double highlightInterval = settings->value(SETTING_HIGHLIGHT_INTERVAL,
                                               QVariant(DEF_HIGHLIGHT_INTERVAL)).toDouble();
    ui->highlightIntervalSpinBox->setValue(highlightInterval);

    // remember last open file
    bool rememberLastFile = settings->value(SETTING_REMEMBER_LAST_FILE,
                                            QVariant(DEF_REMEMBER_LAST_FILE)).toBool();
    ui->rememberLastFileCheckBox->setChecked(rememberLastFile);

    // make links clickable
    bool clickableLinks = settings->value(SETTING_CLICKABLE_LINKS,
                                          QVariant(DEF_CLICKABLE_LINKS)).toBool();
    ui->linksClickableCheckBox->setChecked(clickableLinks);

    // line highlight color
    QColor lineHighlightColor = settings->value(SETTING_LINE_HIGHLIGHT_COLOR,
                                                QVariant(DEF_LINE_HIGHLIGHT_COLOR)).value<QColor>();
    QPalette palette = ui->highlightLineColorLabel->palette();
    palette.setColor(ui->highlightLineColorLabel->backgroundRole(), lineHighlightColor);
    ui->highlightLineColorLabel->setPalette(palette);
}

void PreferencesDialog::updateSettingsFromUI()
{
    settings->setValue(SETTING_FONT, ui->fontLabel->font().toString());
    settings->setValue(SETTING_TAB_WIDTH, ui->tabWidthSpinBox->value());
    settings->setValue(SETTING_HIGHLIGHT_INTERVAL, ui->highlightIntervalSpinBox->value());
    settings->setValue(SETTING_INDENT_WITH_TABS, ui->tabsWithSpacesCheckBox->isChecked());
    settings->setValue(SETTING_REMEMBER_LAST_FILE, ui->rememberLastFileCheckBox->isChecked());
    settings->setValue(SETTING_CLICKABLE_LINKS, ui->linksClickableCheckBox->isChecked());
    settings->setValue(SETTING_LINE_HIGHLIGHT_COLOR, ui->highlightLineColorLabel->palette().background().color());
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

void PreferencesDialog::lineHighlightColorButtonClicked()
{
    QColor currColor = ui->highlightLineColorLabel->palette().background().color();
    QColor newColor = QColorDialog::getColor(currColor, this);
    QPalette palette = ui->highlightLineColorLabel->palette();
    palette.setColor(ui->highlightLineColorLabel->backgroundRole(), newColor);
    ui->highlightLineColorLabel->setPalette(palette);
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
