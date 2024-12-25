#include "settings.h"
#include <QSettings>

Settings::Settings(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::SettingsDialog())
{
	ui->setupUi(this);
	this->_load_state();
}

Settings::~Settings()
{
	delete ui;
}

QLocale Settings::locale() const
{
	return this->m_locale;
}

bool Settings::requireAllQuestions() const
{
	return this->m_requireAllQuestions;
}

bool Settings::includeOptionIndicator() const
{
	return this->m_includeOptionIndicator;
}

void Settings::on_languageCombobox_currentIndexChanged(int index)
{
	this->m_localeChanged = true;

	switch (index)
	{
	case 0:
		this->m_locale = QLocale::system();
		this->m_isSystemLocale = true;
		break;

	case 1:
		this->m_locale = QLocale{ QLocale::Arabic, QLocale::Egypt };
		this->m_isSystemLocale = false;
		break;

	case 2:
		this->m_locale = QLocale{ QLocale::English, QLocale::UnitedStates };
		this->m_isSystemLocale = false;
		break;

	default:
		qCritical() << "Unknown locale chosen in language check box.";
	}

}

void Settings::_load_state()
{
	m_settings.beginGroup("userPreferences");

	QString localeString = m_settings.value("locale", "").toString();

	if (localeString.isEmpty() || localeString == "system") {
		this->m_locale = QLocale::system();
		this->m_isSystemLocale = true;
		this->ui->languageCombobox->setCurrentIndex(0);
	}
	else {
		this->m_locale = QLocale{ localeString };
		this->m_isSystemLocale = false;

		if (this->m_locale.language() == QLocale::Arabic)
			this->ui->languageCombobox->setCurrentIndex(1);
		else
			this->ui->languageCombobox->setCurrentIndex(2);

	}

	this->m_requireAllQuestions = m_settings.value("requireAllQuestions", this->ui->requireQuestionsCheckbox->isChecked()).toBool();
	this->m_includeOptionIndicator = m_settings.value("includeOptionIndicator", this->ui->indicatorCheckbox->isChecked()).toBool();

	this->ui->requireQuestionsCheckbox->setChecked(this->m_requireAllQuestions);
	this->ui->indicatorCheckbox->setChecked(this->m_includeOptionIndicator);

	m_settings.endGroup();
}

void Settings::_store_state()
{
	m_settings.beginGroup("userPreferences");

	if (this->m_localeChanged) {
		if (m_isSystemLocale)
			m_settings.setValue("locale", "system");
		else
			m_settings.setValue("locale", this->m_locale.name());
		this->m_localeChanged = false;
	}

	m_settings.setValue("requireAllQuestions", this->ui->requireQuestionsCheckbox->isChecked());
	m_settings.setValue("includeOptionIndicator", this->ui->indicatorCheckbox->isChecked());

	m_settings.endGroup();
}

void Settings::on_SettingsDialog_accepted()
{
	this->_store_state();
}
 