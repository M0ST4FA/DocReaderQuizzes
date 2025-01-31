#include "settings.h"
#include <QSettings>
#include <QTranslator>

const QString Settings::DEFAULT_PDF_TO_TEXT_PROMPT = R"(Extract all the text from this PDF file, focusing only on quiz questions and their answer options.
Strictly exclude headers, footers, and any non-question-related text.
Format each question as:
#. Question title  
   a. Option 1  
   b. Option 2  
   c. Option 3  
   d. Option 4
Identify the correct answer by placing an asterisk (*) before the letter of the correct option as:
	*d. Paris
The correct answer is marked with a red indicator in the PDF.
Ensure no extra characters, spaces, or formatting alterations beyond the original structure.)";

Settings::Settings(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::SettingsDialog())
{
	this->_load_state();
	ui->setupUi(this);
	this->_set_comboboxes();

	this->ui->promptTextEdit->setText(this->m_PDFToTextPrompt);
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

QString Settings::PDFToTextPrompt() const
{
	return this->m_PDFToTextPrompt;
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

void Settings::on_promptTextEdit_textChanged()
{
	this->m_PDFToTextPrompt = this->ui->promptTextEdit->toPlainText();
}

void Settings::on_resetPromptBtn_clicked()
{
	this->m_PDFToTextPrompt = DEFAULT_PDF_TO_TEXT_PROMPT;
	this->ui->promptTextEdit->setText(this->m_PDFToTextPrompt);
}

void Settings::_load_state()
{
	m_settings.beginGroup("userPreferences");

	QString localeString = m_settings.value("locale", "").toString();

	if (localeString.isEmpty() || localeString == "system") {
		this->m_locale = QLocale::system();
		this->m_isSystemLocale = true;
	}
	else {
		this->m_locale = QLocale{ localeString };
		this->m_isSystemLocale = false;
	}

	this->_load_language();

	this->m_requireAllQuestions = m_settings.value("requireAllQuestions", true).toBool();
	this->m_includeOptionIndicator = m_settings.value("includeOptionIndicator", true).toBool();

	this->m_PDFToTextPrompt = m_settings.value("PDFToTextPrompt", DEFAULT_PDF_TO_TEXT_PROMPT).toString();

	m_settings.endGroup();
}

void Settings::_load_language()
{
	QTranslator* qtTrans = nullptr;
	QTranslator* appTrans = nullptr;
	if (this->m_locale.language() == QLocale::Arabic) {

		qtTrans = new QTranslator{ QApplication::instance() };
		appTrans = new QTranslator{ QApplication::instance() };

		if (!qtTrans->load(":/i18n/translations/qt_ar.qm"))
			qDebug() << "Couldn't load Qt translations.";
		if (!appTrans->load(":/i18n/translations/Quizzes_ar_EG.qm"))
			qDebug() << "Couldn't load app translations.";

		QApplication::instance()->installTranslator(qtTrans);
		QApplication::instance()->installTranslator(appTrans);
	}
}

void Settings::_set_comboboxes()
{

	if (this->m_isSystemLocale)
		this->ui->languageCombobox->setCurrentIndex(0);
	else if (this->m_locale.language() == QLocale::Arabic)
		this->ui->languageCombobox->setCurrentIndex(1);
	else
		this->ui->languageCombobox->setCurrentIndex(2);

	this->ui->requireQuestionsCheckbox->setChecked(this->m_requireAllQuestions);
	this->ui->indicatorCheckbox->setChecked(this->m_includeOptionIndicator);
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

	m_settings.setValue("requireAllQuestions", this->m_requireAllQuestions);
	m_settings.setValue("includeOptionIndicator", this->m_includeOptionIndicator);

	m_settings.setValue("PDFToTextPrompt", this->m_PDFToTextPrompt);

	m_settings.endGroup();
}

void Settings::on_SettingsDialog_accepted()
{
	this->m_requireAllQuestions = this->ui->requireQuestionsCheckbox->isChecked();
	this->m_includeOptionIndicator = this->ui->indicatorCheckbox->isChecked();
	this->_store_state();
}