#pragma once

#include <QDialog>
#include <QLocale>
#include <QSettings>
#include "ui_settings.h"

class Settings : public QDialog
{
	Q_OBJECT

public:
	Settings(QWidget *parent = nullptr);
	~Settings();

	QLocale locale() const;
	bool requireAllQuestions() const;
	bool includeOptionIndicator() const;
	QString PDFToTextPrompt() const;

protected slots:

	void on_SettingsDialog_accepted();
	void on_languageCombobox_currentIndexChanged(int);
	void on_promptTextEdit_textChanged();
	void on_resetPromptBtn_clicked();

private:
	Ui::SettingsDialog *ui;

	static const QString DEFAULT_PDF_TO_TEXT_PROMPT;

	bool m_localeChanged = false;
	bool m_isSystemLocale = true;
	QString m_PDFToTextPrompt = DEFAULT_PDF_TO_TEXT_PROMPT;
	QLocale m_locale = QLocale::system();

	bool m_requireAllQuestions = false;
	bool m_includeOptionIndicator = true;

	QSettings m_settings;

	void _load_state();
	void _load_language();
	void _set_comboboxes();
	void _store_state();

};
