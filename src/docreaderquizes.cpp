#include "docreaderquizes.h"
#include <QFileDialog>
#include <QMessageBox>

DocReaderQuizzes::DocReaderQuizzes(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::DocReaderQuizesClass())
{
	ui->setupUi(this);

	this->m_formsApi->setIncludeFormInResponse(true);

	if (this->m_authenticated == false)
		this->ui->stackedWidget->setCurrentWidget(this->ui->authenticationPage);

	this->_reset_to_new_quiz_state();

	connect(this->m_sso, &GoogleSSO::granted, this, &DocReaderQuizzes::token_granted);
	connect(this->m_formsApi, &GoogleFormsAPI::formCreated, this, &DocReaderQuizzes::form_created);
	connect(this->m_formsApi, &GoogleFormsAPI::formUpdated, this, &DocReaderQuizzes::form_updated);
	connect(this->m_formsApi, &GoogleFormsAPI::errorOccurred, [this](const QString& errorStr) {
		qInfo() << errorStr;
		});

}

DocReaderQuizzes::~DocReaderQuizzes()
{
	delete ui;
}

void DocReaderQuizzes::form_created(std::shared_ptr<Form> form)
{

	using namespace m0st4fa::forms;
	using namespace m0st4fa::forms::update_form;

	this->m_form = form;

	UpdateFormInfoRequest updateInfoReq{
		.info {
			.description = m_temporaryInfo.description
		},
		.updateMask = "description"
	};

	UpdateSettingsRequest updateSettingsReq{
		.settings {
			.quizSettings {
				.isQuiz = true
			}
			},
		.updateMask = "quizSettings.isQuiz"
	};

	UpdateRequestBody updateInfoAndSettingsReq{
		.includeFormInResponse = true,
		.requests {
			UpdateRequest{.request = updateInfoReq},
			UpdateRequest{.request = updateSettingsReq}
		}
	};

	this->m_formsApi->updateForm(this->m_form->formId, updateInfoAndSettingsReq);
}

void DocReaderQuizzes::form_updated(const UpdateResponseBody& response)
{
	using namespace m0st4fa::forms;
	using namespace m0st4fa::forms::update_form;

	if (!m_temporaryInfo.updated) { // if we haven't just updated the info and settings 

		m_temporaryInfo.updated = true; // by this time, they should have been updated, so indicate it
		this->m_formsApi->createItems(this->m_form->formId, m_temporaryInfo.requests);
	}
	else { // if we're here, it means that the items have been created
		
		for (const UpdateResponse& reply : response.replies) {
			qInfo() << reply.toJson();
		}

		this->m_form = response.form;

		QString formId = this->m_form->formId;
		QString formLink = QString{"https://docs.google.com/forms/d/%0"}.arg(formId);

		QMessageBox::information(this, "DocReaderQuizzes", QString{ "Form created successfully.\nYou can access it via this link: %1." }.arg(formLink));

		this->_reset_to_new_quiz_state();
	}

}

void DocReaderQuizzes::on_loginBtn_clicked()
{
	this->m_sso->authenticate();
}

void DocReaderQuizzes::on_chooseFileBtn_clicked()
{

	QString filePath = QFileDialog::getOpenFileName(this, "Choose the file to be converted to Google Forms quiz.", QString{}, "Text Files (*.txt)");

	if (filePath.isEmpty()) {
		QMessageBox::critical(this, "Error while processing chosen file.", "Either you didn't choose a file or the path is incorrect. Try to choose again.");
		return;
	}

	this->ui->createQuizBtn->setEnabled(true);
	this->m_temporaryInfo.filePath = filePath;
}

void DocReaderQuizzes::on_createQuizBtn_clicked()
{
	using namespace m0st4fa::forms;
	using namespace m0st4fa::forms::update_form;

	this->m_parser->setFilePath(m_temporaryInfo.filePath);
	QVector<CreateItemRequest> requests = this->m_parser->parseFile();

	if (requests.isEmpty()) {
		QMessageBox::critical(this, "", "The provided file is either empty or it doesn't adhere to the correct syntax.");
		return;
	}

	QString title = this->ui->formTitleLineEdit->text();
	QString documentTitle = this->ui->formDocumentLineEdit->text();
	QString description = this->ui->formDescriptionLineEdit->text();

	m_temporaryInfo.description = description;
	m_temporaryInfo.requests = requests;
	m_temporaryInfo.updated = false;

	this->m_formsApi->createForm(title, documentTitle);
	this->ui->createQuizBtn->setText("Creating Quiz...");
}

void DocReaderQuizzes::_reset_to_new_quiz_state()
{
	m_temporaryInfo.reset();

	this->ui->createQuizBtn->setDisabled(true);
	this->ui->createQuizBtn->setText("Create Quiz");
	this->m_form = nullptr;

	this->ui->formTitleLineEdit->setText("");
	this->ui->formDocumentLineEdit->setText("");
	this->ui->formDescriptionLineEdit->setText("");
}

void DocReaderQuizzes::token_granted()
{
	qInfo() << this->m_sso->token();
	this->m_authenticated = true;
	this->m_formsApi->setAccessToken(this->m_sso->token());
	this->ui->stackedWidget->setCurrentWidget(this->ui->applicationPage);
}
