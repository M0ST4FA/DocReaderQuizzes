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
	using Grading = m0st4fa::forms::Grading;
	using Image = m0st4fa::forms::Image;
	using Item = m0st4fa::forms::Item;
	using Question = m0st4fa::forms::Question;
	using QuestionItem = m0st4fa::forms::QuestionItem;
	using Item = m0st4fa::forms::Item;

	AddItemDialog(std::shared_ptr<Form>, QWidget *parent = nullptr);
	~AddItemDialog();

	bool isQuestionRequired() const;
	bool isShufflingEnabled() const;

	QVector<Option> getOptions() const;

	Grading getGrading() const;

	Image getImage() const;

	QuestionItem getQuestionItem() const;

	Item getItem() const;

protected slots:

	void on_addOptionBtn_clicked();

private:
	Ui::AddItemDialogClass *ui;
	std::shared_ptr<Form> m_form = nullptr;
};
