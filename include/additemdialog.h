#pragma once

#include <QDialog>
#include "ui_additemdialog.h"
#include "googleformsapihelperclasses.h"
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class AddItemDialogClass; };
QT_END_NAMESPACE

class AddItemDialog : public QDialog
{
	Q_OBJECT

public:
	using Form = m0st4fa::forms::Form;
	using Option = m0st4fa::forms::Option;

	AddItemDialog(std::shared_ptr<Form>, QWidget *parent = nullptr);
	~AddItemDialog();

	bool isQuestionRequired() const;
	bool isShufflingEnabled() const;

	QVector<Option> getOptions() const;

protected slots:

	void on_addOptionBtn_clicked();

private:
	Ui::AddItemDialogClass *ui;
	std::shared_ptr<Form> m_form = nullptr;
};
