#pragma once

#include <QDialog>
#include "ui_updateformdialog.h"
#include "googleformsapihelperclasses.h"
#include <memory>

class UpdateFormDialog : public QDialog
{
	Q_OBJECT

public:
	using Form = m0st4fa::forms::Form;

	UpdateFormDialog(std::shared_ptr<Form>, QWidget* = nullptr);
	~UpdateFormDialog();

	m0st4fa::forms::update_form::UpdateFormInfoRequest getFormInfo() const;
	m0st4fa::forms::update_form::UpdateSettingsRequest getFormSettings() const;

private:
	Ui::UpdateFormDialogClass *ui;
	std::shared_ptr<Form> m_form = nullptr;
};
