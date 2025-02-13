#include "docreaderquizzes.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>
#include "googlegeminiapi.h"
#include <QProcess>
#include <QShortcut>

DocReaderQuizzes::DocReaderQuizzes(QWidget *parent)
	: QWidget(parent)
{
	this->m_parser->setParent(this);
	// Load settings first for the language
	this->_load_settings();

	ui->setupUi(this);

	if (!_is_connected_to_internet()) {
		QMessageBox::critical(this, QApplication::instance()->applicationName(), "Your computer is not connected to internet. Make sure it is connected and then try again.");
		std::abort();
	}

	this->_set_copyright_info();

	this->m_formsApi->setIncludeFormInResponse(true);

	if (this->m_authenticated == false)
		this->ui->stackedWidget->setCurrentWidget(this->ui->authenticationPage);

	this->_reset_to_new_quiz_state();

	connect(this->m_sso, &GoogleSSO::granted, this, &DocReaderQuizzes::token_granted);
	connect(this->m_formsApi, &GoogleFormsAPI::formCreated, this, &DocReaderQuizzes::form_created);
	connect(this->m_formsApi, &GoogleFormsAPI::formUpdated, this, &DocReaderQuizzes::form_updated);
	connect(this->m_formsApi, &GoogleFormsAPI::errorOccurred, [this](const QString& errorStr) {
		qCritical() << errorStr;
		});
	connect(this->m_parser, &FileParser::reportStatus, [this](StatusReport report) {

		QColor defaultColor = this->m_logger->textEdit()->textColor();

		switch (report.level)
		{
		case ReportLevel::INFO:
			this->m_logger->textEdit()->setTextColor(QColor{135, 206, 235});
			break;

		case ReportLevel::WARNING:
			this->m_logger->textEdit()->setTextColor(QColor{255, 165, 0});
			break;

		case ReportLevel::ERROR:
			this->m_logger->textEdit()->setTextColor(QColor{255, 0, 0});
			break;

		default:
			break;
		}

		this->m_logger->textEdit()->append(report.toString());
		this->m_logger->textEdit()->setTextColor(defaultColor);
	});
	connect(this->m_parser, &FileParser::aborted, [this]() {

		QMessageBox::critical(this, QApplication::instance()->applicationName(), tr("Found errors while parsing the file. The form will not be created. Correct the errors and then try again."));

		});

	this->_set_shortcuts();
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

		QMessageBox::information(this, QApplication::instance()->applicationName(), QString{ tr("Form created successfully.\nLink copied to your clipboard.") });

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

	QString filePath = QFileDialog::getOpenFileName(this, tr("Choose the file to be converted to Google Forms quiz."), QString{"."}, "PDF Files (*.pdf);; Text Files (*.txt)");

#ifdef _DEBUG
	qDebug() << "Path of chosen file: " << filePath;
#endif
	
	if (filePath.isEmpty()) {
		QMessageBox::critical(this, QApplication::instance()->applicationName(), tr("Either you didn't choose a file or the path is incorrect. Try to choose again."));
		return;
	}

	QFileInfo fileInfo{ filePath };

	this->m_temporaryInfo.suffix = fileInfo.suffix();

	if (this->m_temporaryInfo.suffix == "pdf")
		this->_set_state(DocReaderQuizzes::PREPARING_PRE_PROCESSING_FILE);
	else if (this->m_temporaryInfo.suffix == "txt")
		this->_set_state(DocReaderQuizzes::PREPARING_PARSING_FILE);

	this->m_temporaryInfo.filePath = filePath;
}

void DocReaderQuizzes::on_createQuizBtn_clicked()
{
	using namespace m0st4fa::forms;
	using namespace m0st4fa::forms::update_form;

	this->_process_text_file();
}

void DocReaderQuizzes::on_processPdfBtn_clicked()
{
	this->_set_state(State::PRE_PROCESSING_FILE);
	this->_process_pdf_file();
}

void DocReaderQuizzes::handle_settings_dialog() const
{

	int res = this->m_settings->exec();

	if (res == 0)
		return;

	this->m_parser->setSettings(m_settings->requireAllQuestions(), m_settings->includeOptionIndicator());
}

void DocReaderQuizzes::handle_logging_dialog() const
{
	this->m_logger->show();
}

void DocReaderQuizzes::_process_text_file()
{
	this->_set_state(State::PARSING_FILE);

	// Parse the file
	QVector<CreateItemRequest> requests = this->_parse_file();

	// Check for correct output of parsing
	if (this->m_parser->foundError()) {
		this->_set_state(WAITING_FOR_FILE);
		return;
	}

	if (requests.isEmpty()) {
		QMessageBox::critical(this, QApplication::instance()->applicationName(), tr("The provided file is either empty or it doesn't adhere to the correct syntax."));
		this->_set_state(WAITING_FOR_FILE);
		return;
	}

	// Creating the form
	this->_create_form(requests);
}

void DocReaderQuizzes::_process_pdf_file()
{
	using namespace m0st4fa::gemini;

	this->_set_state(PRE_PROCESSING_FILE);

	GoogleGeminiAPI* gemini = new GoogleGeminiAPI{ this };

	QObject::connect(gemini, &GoogleGeminiAPI::promptExecuted, [this](const m0st4fa::gemini::GenerateContentResponse& response) {
		qInfo() << std::get<0>(response.candidates.at(0).content.parts.at(0).part);

		QFile file{"temp.txt"};

		if (file.open(QIODevice::WriteOnly)) {
			file.write(std::get<0>(response.candidates.at(0).content.parts.at(0).part).toLatin1());
			file.close();
			qDebug() << "File created successfully!";
		}
		else {
			qDebug() << "Error creating file:";
			qDebug() << file.errorString();
		}

		this->m_temporaryInfo.filePath = file.fileName();

		QProcess* proc = new QProcess{ this };
		proc->start("notepad.exe", { file.fileName() }, QIODeviceBase::ReadWrite | QIODeviceBase::Text);

		connect(proc, &QProcess::finished, [](int exitCode, QProcess::ExitStatus status) {

			qInfo() << "Notepad exited with exit code:" << exitCode << "and exit status:" << status;

			});

		this->_set_state(State::PREPARING_PARSING_FILE);

		});

	gemini->executePrompt(this->m_settings->PDFToTextPrompt(), this->m_temporaryInfo.filePath);
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
}

void DocReaderQuizzes::_set_copyright_info()
{
	QWidget* authenticationPage = this->ui->authenticationPage;
	QGridLayout* authPageLayout = dynamic_cast<QGridLayout*>(authenticationPage->layout());
	this->ui->logoLabel->setPixmap(QPixmap{ "assets/icons/appicon.svg" }.scaled(QSize{ 100, 100 }));

}

void DocReaderQuizzes::_load_settings()
{
	this->m_parser->setSettings(m_settings->requireAllQuestions(), m_settings->includeOptionIndicator());
}

bool DocReaderQuizzes::_is_connected_to_internet()
{
	QUrl host("1.1.1.1"); // put here URL (or IP) of your own website if you have one
	qDebug() << tr("Checking internet connection: connecting to %1 ...").arg(host.toString());

	QTcpSocket* sock = new QTcpSocket(this);
	connect(sock, &QTcpSocket::errorOccurred, this, [](QAbstractSocket::SocketError error) {
		qDebug() << "This error occurred during Internet checking:" << error;
		});

	sock->connectToHost(host.toString(), 80);
	bool connected = sock->waitForConnected(3'000);//ms

	if (!connected)
	{
		sock->abort();
		qDebug() << tr("%1 is not reachable. Not connected to the Internet").arg(host.toString());
		return false;
	}

	sock->close();
	qDebug() << tr("Connected to the Internet");
	return true;
}

void DocReaderQuizzes::_set_createQuizBtn_state()
{

	switch (this->m_state)
	{
	case WAITING_FOR_FILE:
		[[fallthrough]];
	case PREPARING_PARSING_FILE:
		this->ui->createQuizBtn->setEnabled(true);
		this->ui->createQuizBtn->setText(tr("Create Quiz"));
		break;
	case PARSING_FILE:
		this->ui->createQuizBtn->setEnabled(false);
		this->ui->createQuizBtn->setText(tr("Parsing File..."));
		break;
	case CREATING_FORM:
		this->ui->createQuizBtn->setEnabled(false);
		this->ui->createQuizBtn->setText(tr("Creating Quiz..."));
		break;
	default:
		this->ui->createQuizBtn->setEnabled(false);
		this->ui->createQuizBtn->setText(tr("Unhandled state"));
		break;
	}

}

void DocReaderQuizzes::_set_processPdfBtn_state()
{
	switch (this->m_state)
	{
	case WAITING_FOR_FILE:
		this->ui->processPdfBtn->setEnabled(false);
		this->ui->processPdfBtn->setText(tr("Process PDF"));
		break;
	case PREPARING_PRE_PROCESSING_FILE:
		this->ui->processPdfBtn->setEnabled(true);
		break;
	case PRE_PROCESSING_FILE:
		this->ui->processPdfBtn->setEnabled(false);
		this->ui->processPdfBtn->setText(tr("Processing PDF..."));
		break;
	case CREATING_FORM:
		this->ui->processPdfBtn->setEnabled(false);
		break;
	default:
		this->ui->processPdfBtn->setEnabled(false);
		this->ui->processPdfBtn->setText(tr("Unhandled state"));
		break;
	}
}

void DocReaderQuizzes::_set_state(State state)
{
	this->m_state = state;

	switch (this->m_state)
	{
	case WAITING_FOR_FILE:
		m_temporaryInfo.reset();

		this->m_form = nullptr;

		this->ui->formTitleLineEdit->setText("");
		this->ui->formDocumentLineEdit->setText("");
		this->ui->formDescriptionLineEdit->setText("");

		this->ui->chooseFileBtn->show();
		this->ui->processPdfBtn->hide();
		this->_set_processPdfBtn_state();
		this->ui->createQuizBtn->hide();
		this->m_parser->resetErrorState();
		break;
	case PREPARING_PRE_PROCESSING_FILE:
		this->ui->processPdfBtn->show();
		this->_set_processPdfBtn_state();
		break;
	case PRE_PROCESSING_FILE:
		this->ui->chooseFileBtn->hide();
		this->ui->processPdfBtn->show();
		this->_set_processPdfBtn_state();
		break;
	case PREPARING_PARSING_FILE:
		this->ui->processPdfBtn->hide();
		this->ui->createQuizBtn->show();
		this->_set_createQuizBtn_state();
		this->m_parser->resetErrorState();
		break;
	case PARSING_FILE:
		this->ui->chooseFileBtn->hide();
		this->ui->createQuizBtn->hide();
		this->m_logger->textEdit()->clear();
		break;
	case CREATING_FORM:
		break;
	default:
		break;
	}

}

void DocReaderQuizzes::_set_shortcuts()
{
	QShortcut* settingsShortcut = new QShortcut{ QKeySequence{tr("Ctrl+S")}, this };
	connect(settingsShortcut, &QShortcut::activated, this, &DocReaderQuizzes::handle_settings_dialog);

	QShortcut* loggerShortcut = new QShortcut{ QKeySequence{tr("Ctrl+L")}, this };
	connect(loggerShortcut, &QShortcut::activated, this, &DocReaderQuizzes::handle_logging_dialog);
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