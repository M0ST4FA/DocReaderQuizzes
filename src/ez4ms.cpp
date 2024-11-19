#include "stdafx.h"
#include "ez4ms.h"

#include <QNetworkAccessManager>

#include <QNetworkReply>
#include <memory>
#include "fileparser.h"


DocReaderQuizzesApp::DocReaderQuizzesApp(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::ez4ms())
	, m_sso{ new GoogleSSO{this->m_scope, this} }
	, m_api {new GoogleFormsAPI{this} }
{
	ui->setupUi(this);

	ui->updateFormBtn->setDisabled(true);
	ui->addMCQBtn->setDisabled(true);

	this->m_api->setIncludeFormInResponse(true);

	QSettings presets{ DEVELOPMENT_PATH"/presets.ini", QSettings::Format::IniFormat };
	QString formId = presets.value("formId").toString();

	this->ui->formIdLineEdit->setText(formId);

	connect(this->m_sso, &GoogleSSO::tokenChanged, this, &DocReaderQuizzesApp::tokenChanged);
	connect(this->m_api, &GoogleFormsAPI::formFetched, [this](std::shared_ptr<Form> form) {

		m_form = form;

		this->ui->updateFormBtn->setEnabled(true);
		this->ui->addMCQBtn->setEnabled(true);

		QByteArray str = QJsonDocument(m_form->toJson()).toJson();

		this->ui->receivedContentTextEdit->setText(str);

			});
	connect(this->m_api, &GoogleFormsAPI::responseFetched, [this](QJsonDocument form) {

		QString str = form.toJson();

		this->ui->receivedContentTextEdit->setText(str);

		});
	connect(this->m_api, &GoogleFormsAPI::responsesFetched, [this](QJsonDocument form) {

		QString str = form.toJson();

		this->ui->receivedContentTextEdit->setText(str);

		});

	connect(this->m_api, &GoogleFormsAPI::formCreated, [this](QString formId) {

		this->ui->formIdLineEdit->setText(formId);

		});

	connect(this->m_api, &GoogleFormsAPI::formUpdated, [this](const m0st4fa::forms::update_form::UpdateResponseBody& response) {

		// Leave the old one just in case it is used else where.
		this->m_form = response.form;

		this->ui->receivedContentTextEdit->setText(QJsonDocument(this->m_form->toJson()).toJson());

		});

	connect(this->m_api, &GoogleFormsAPI::errorOccurred, [this](const QString& errorStr) {

		qInfo() << errorStr;

		});
}

DocReaderQuizzesApp::~DocReaderQuizzesApp()
{
	delete ui;
}

void DocReaderQuizzesApp::on_authenticateBtn_clicked()
{
	m_sso->authenticate();
}

void DocReaderQuizzesApp::on_saveTokensBtn_clicked()
{
	this->m_sso->saveTokens();
}

void DocReaderQuizzesApp::on_requestContentBtn_clicked()
{
	QString formId = this->ui->formIdLineEdit->text();
	QString responseId = this->ui->responseIdLineEdit->text();

	if (formId.isEmpty()) {
		QMessageBox::warning(this, "Error while fetching form.", "Form ID field is empty. You must enter a valid ID.");
		return;
	}

	if (responseId.isEmpty())
		return m_api->getForm(formId);

	if (responseId == "ALL")
		return this->m_api->getResponses(formId);

	return m_api->getResponse(formId, responseId);
}

void DocReaderQuizzesApp::on_deleteContentBtn_clicked()
{
	this->ui->receivedContentTextEdit->clear();
}

void DocReaderQuizzesApp::on_createFormBtn_clicked()
{
	this->m_api->createForm("Test Form", "Test Form");
}

void DocReaderQuizzesApp::on_updateFormBtn_clicked()
{
	UpdateFormDialog dialog{ this->m_form, this };
	int btn = dialog.exec();

	using UpdateFormInfoRequest = m0st4fa::forms::update_form::UpdateFormInfoRequest;
	using UpdateSettingsRequest = m0st4fa::forms::update_form::UpdateSettingsRequest;

	// Cancel pressed. Abort the update.
	if (btn == 0)
		return;

	UpdateFormInfoRequest newFormInfo = dialog.getFormInfo();
	UpdateSettingsRequest newSettings = dialog.getFormSettings();

	this->m_api->updateFormInfo(this->m_form->formId, newFormInfo);
	this->m_api->updateFormSettings(this->m_form->formId, newSettings);
}

void DocReaderQuizzesApp::on_addMCQBtn_clicked()
{
	AddItemDialog* dialog = new AddItemDialog{ this->m_form, this };
	int result = dialog->exec();

	using CreateItemRequest = m0st4fa::forms::update_form::CreateItemRequest;

	unsigned int index = static_cast<unsigned int>(this->m_form->items.size());

	qInfo() << dialog->getImage().toJson();

	CreateItemRequest request {
		.item = dialog->getItem(),
		.location = { .index = index }
	};

	this->m_api->createItems(this->m_form->formId, {request});

}

void DocReaderQuizzesApp::on_parseFileBtn_clicked()
{
	FileParser parser{ this->m_form, DEVELOPMENT_PATH"/resources/MCQs.txt" };
	using CreateItemRequest = m0st4fa::forms::update_form::CreateItemRequest;

	QVector<CreateItemRequest> requests = parser.parseFile();

	this->m_api->createItems(this->m_form->formId, requests);
}

void DocReaderQuizzesApp::tokenChanged()
{
	this->m_api->setAccessToken(this->m_sso->token());
	this->ui->accessTokenLineEdit->setText(this->m_sso->token());
	this->ui->refreshTokenLineEdit->setText(this->m_sso->refreshToken());
}
