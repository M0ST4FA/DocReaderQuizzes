#include "updateformdialog.h"

UpdateFormDialog::UpdateFormDialog(std::shared_ptr<Form> form, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::UpdateFormDialogClass())
	, m_form{form}
{
	ui->setupUi(this);

	// Set form info
	this->ui->titleLineEdit->setText(this->m_form->info.title);
	this->ui->documentTitleLineEdit->setText(this->m_form->info.documentTitle);
	this->ui->descriptionLineEdit->setText(this->m_form->info.description);

	// Set form settings
	this->ui->isQuizCheckbox->setChecked(this->m_form->settings.quizSettings.isQuiz);

}

UpdateFormDialog::~UpdateFormDialog()
{
	delete ui;
}

m0st4fa::forms::update_form::UpdateFormInfoRequest UpdateFormDialog::getFormInfo() const
{
	QString title = this->ui->titleLineEdit->text();
	QString documentTitle = this->ui->documentTitleLineEdit->text();
	QString description = this->ui->descriptionLineEdit->text();

	bool titleChanged = title != this->m_form->info.title;
	bool descriptionChanged = description != this->m_form->info.description;

	QStringList mask;

	if (titleChanged)
		mask << "title";

	if (descriptionChanged)
		mask << "description";

	return m0st4fa::forms::update_form::UpdateFormInfoRequest{
		.info {
			.title = title,
			.documentTitle = documentTitle,
			.description = description
		},
		.updateMask = mask.join(",")
	};
}

m0st4fa::forms::update_form::UpdateSettingsRequest UpdateFormDialog::getFormSettings() const
{
	bool isQuiz = this->ui->isQuizCheckbox->isChecked();
	bool isQuizChanged = isQuiz != this->m_form->settings.quizSettings.isQuiz;

	QStringList mask;

	if (isQuizChanged)
		mask << "quizSettings.isQuiz";

	return m0st4fa::forms::update_form::UpdateSettingsRequest{
		.settings {
			.quizSettings {.isQuiz = isQuiz }
		},
		.updateMask = mask.join(",")
	};
}
