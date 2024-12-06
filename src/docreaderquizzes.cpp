#include "docreaderquizzes.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>

const QString DocReaderQuizzes::IMAGE_TO_TEXT_PROMPT = R"(Extract the text out of this image. Be conservative. Indicate the correct option by putting * before the letter denoting it. The correct option is colored red. Return the result in code font.)";
const QString DocReaderQuizzes::FORMAT_TEXT_PROMPT = R"(Format the following text in the following format: questions begin with a numeral followed by dot. Options begin with a letter followed by dot. Return the result in code font.)";

DocReaderQuizzes::DocReaderQuizzes(QWidget *parent)
	: QWidget(parent)
{
	ui->setupUi(this);
	this->m_parser->setParent(this);

	this->_set_copyright_info();

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
	connect(this->m_parser, &FileParser::reportStatus, [this](StatusReport report) {

		QColor defaultColor = this->ui->reportingTextEdit->textColor();

		switch (report.level)
		{
		case ReportLevel::INFO:
			this->ui->reportingTextEdit->setTextColor(QColor{135, 206, 235});
			break;

		case ReportLevel::WARNING:
			this->ui->reportingTextEdit->setTextColor(QColor{255, 165, 0});
			break;

		case ReportLevel::ERROR:
			this->ui->reportingTextEdit->setTextColor(QColor{255, 0, 0});
			break;

		default:
			break;
		}

		this->ui->reportingTextEdit->append(report.toString());
		this->ui->reportingTextEdit->setTextColor(defaultColor);
	});
	connect(this->m_parser, &FileParser::aborted, [this]() {

		QMessageBox::critical(this, "DocReaderQuizzes", "Found errors while parsing the file. The form will not be created. Correct the errors and then try again.");

		});
}

DocReaderQuizzes::~DocReaderQuizzes()
{
	delete this->m_parser;
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
		
#ifdef _DEBUG
		qInfo() << "LOGGING REPLIES OBTAINED FROM SERVER:";
		for (const UpdateResponse& reply : response.replies) {
			qInfo() << reply.toJson();
			qInfo() << "***************************";
		}
		qInfo() << "------------------------------------------------------";
#endif

		this->m_form = response.form;

		QString formId = this->m_form->formId;
		QString formLink = QString{"https://docs.google.com/forms/d/%0"}.arg(formId);

		QMessageBox::information(this, "DocReaderQuizzes", QString{ "Form created successfully.\nLink copied to your clipboard." });

		QApplication::clipboard()->setText(formLink);

		this->_reset_to_new_quiz_state();
	}

}

void DocReaderQuizzes::on_loginBtn_clicked()
{
	this->m_sso->authenticate();
}

void DocReaderQuizzes::on_chooseFileBtn_clicked()
{

	QString filePath = QFileDialog::getOpenFileName(this, "Choose the file to be converted to Google Forms quiz.", QString{"."}, "Text Files (*.txt);; PDF Files (*.pdf)");

#ifdef _DEBUG
	qDebug() << "Path of chosen file: " << filePath;
#endif
	
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

	// Parse the file
	QVector<CreateItemRequest> requests = this->_parse_file();

	// Check for correct output of parsing
	if (this->m_parser->foundError()) {
		this->_set_state(WAITING_FOR_FILE);
		return;
	}

	if (requests.isEmpty()) {
		QMessageBox::critical(this, "DocReaderQuizzes", "The provided file is either empty or it doesn't adhere to the correct syntax.");
		this->_set_state(WAITING_FOR_FILE);
		return;
	}

	// Creating the form
	this->_create_form(requests);
}

void DocReaderQuizzes::_process_text_file()
{
}

void DocReaderQuizzes::_process_pdf_file()
{
}

QVector<DocReaderQuizzes::CreateItemRequest> DocReaderQuizzes::_parse_file()
{
	// Parsing the file
	this->m_parser->setFilePath(m_temporaryInfo.filePath);
	this->_set_state(PARSING_FILE);

	QVector<CreateItemRequest> requests = this->m_parser->parseFile();

#ifdef _DEBUG
	qInfo() << "LOGGING OBTAINED REQUESTS:";
	for (const CreateItemRequest& req : requests) {
		qInfo() << req.toJson();
		qInfo() << "*****************";
	}
	qInfo() << "------------------------------------------------------------";
#endif

	return requests;
}

void DocReaderQuizzes::_create_form(const QVector<CreateItemRequest>& requests)
{
	// Creating the form
	QString title = this->ui->formTitleLineEdit->text();

	if (title.isEmpty()) {
		QMessageBox::critical(this, "DocReaderQuizzes", "The form title is mandatory. You have entered one. Enter a form title and then try again.");
		this->_set_state(WAITING_FOR_FILE);
		return;
	}

	QString documentTitle = this->ui->formDocumentLineEdit->text();
	QString description = this->ui->formDescriptionLineEdit->text();

	m_temporaryInfo.description = description;
	m_temporaryInfo.requests = requests;
	m_temporaryInfo.updated = false;

	this->_set_state(CREATING_FORM);
	this->m_formsApi->createForm(title, documentTitle);

}

void DocReaderQuizzes::_reset_to_new_quiz_state()
{
	this->_set_state(WAITING_FOR_FILE);

	QColor color = this->ui->reportingTextEdit->textColor();

	this->ui->reportingTextEdit->setTextColor(QColor(135, 206, 235));

	QString msg1 = "Prompt for converting image to MCQs\n------------------------------------------\n%0";
	QString msg2 = "Prompt for formatting ill-formatted MCQs\n----------------------------------------------\n%0";

	this->ui->reportingTextEdit->append(msg1.arg(IMAGE_TO_TEXT_PROMPT));
	this->ui->reportingTextEdit->append(msg2.arg(FORMAT_TEXT_PROMPT));

	this->ui->reportingTextEdit->setTextColor(color);
}

void DocReaderQuizzes::_set_copyright_info()
{
	QWidget* authenticationPage = this->ui->authenticationPage;
	QGridLayout* authPageLayout = dynamic_cast<QGridLayout*>(authenticationPage->layout());
	this->ui->logoLabel->setPixmap(QPixmap{ "assets/icons/appicon.svg" }.scaled(QSize{ 100, 100 }));

}

void DocReaderQuizzes::_set_createQuizBtn_state()
{

	switch (this->m_state)
	{
	case WAITING_FOR_FILE:
		this->ui->createQuizBtn->setEnabled(true);
		this->ui->createQuizBtn->setText("Create Quiz");
		break;
	case PARSING_FILE:
		this->ui->createQuizBtn->setEnabled(false);
		this->ui->createQuizBtn->setText("Parsing File...");
		break;
	case CREATING_FORM:
		this->ui->createQuizBtn->setEnabled(false);
		this->ui->createQuizBtn->setText("Creating Quiz...");
		break;
	default:
		this->ui->createQuizBtn->setEnabled(false);
		this->ui->createQuizBtn->setText("Unhandled state");
		break;
	}

}

void DocReaderQuizzes::_set_state(State state)
{
	this->m_state = state;

	this->_set_createQuizBtn_state();

	switch (this->m_state)
	{
	case WAITING_FOR_FILE:
		m_temporaryInfo.reset();

		this->m_form = nullptr;

		this->ui->formTitleLineEdit->setText("");
		this->ui->formDocumentLineEdit->setText("");
		this->ui->formDescriptionLineEdit->setText("");
		break;
	case PARSING_FILE:
		this->ui->reportingTextEdit->clear();
		break;
	case CREATING_FORM:
		break;
	default:
		break;
	}

}

void DocReaderQuizzes::token_granted()
{

#ifdef _DEBUG
	qInfo() << "Granted token:" << this->m_sso->token();
#endif
	this->m_authenticated = true;
	this->m_formsApi->setAccessToken(this->m_sso->token());
	this->ui->stackedWidget->setCurrentWidget(this->ui->applicationPage);
}