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

protected slots:

	void on_SettingsDialog_accepted();
	void on_languageCombobox_currentIndexChanged(int);

private:
	Ui::SettingsDialog *ui;

	bool m_localeChanged = false;
	bool m_isSystemLocale = true;
	QLocale m_locale = QLocale::system();

	bool m_requireAllQuestions = false;
	bool m_includeOptionIndicator = true;

	QSettings m_settings;

	void _load_state();
	void _load_language();
	void _set_comboboxes();
	void _store_state();

};
