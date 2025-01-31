#pragma once

#include <QWidget>
#include "ui_docreaderquizzes.h"
#include "googlesso.h"
#include "googleformapi.h"
#include "fileparser.h"
#include "settings.h"
#include "logger.h"

class DocReaderQuizzes : public QWidget
{
	Q_OBJECT

public:
	using GoogleFormsAPI = m0st4fa::forms::GoogleFormsAPI;
	using Form = m0st4fa::forms::Form;
	using UpdateResponseBody = m0st4fa::forms::update_form::UpdateResponseBody;
	using CreateItemRequest = m0st4fa::forms::update_form::CreateItemRequest;

	DocReaderQuizzes(QWidget *parent = nullptr);
	~DocReaderQuizzes();

protected slots:

	// Responding to state changes
	void token_granted();
	void form_created(std::shared_ptr<Form>);
	void form_updated(const UpdateResponseBody&);

	// Handling buttons
	void on_loginBtn_clicked();

	void on_chooseFileBtn_clicked();

	void on_createQuizBtn_clicked();

	void on_processPdfBtn_clicked();

	// Handling dialogs
	void handle_settings_dialog() const;
	void handle_logging_dialog() const;

private:

	// UI
	std::shared_ptr<Ui::DocReaderQuizzesClass> ui{ new Ui::DocReaderQuizzesClass() };
	static const QString DEFAULT_IMAGE_TO_TEXT_PROMPT;
	static const QString DEFAULT_PDF_TO_TEXT_PROMPT;
	static const QString FORMAT_TEXT_PROMPT;

	// Authentication
	bool m_authenticated = false;
	QString scope{ "https://www.googleapis.com/auth/drive.file" };
	GoogleSSO* m_sso{ new GoogleSSO{ scope, this } };

	// Forms
	GoogleFormsAPI* m_formsApi{ new GoogleFormsAPI{this} };
	std::shared_ptr<Form> m_form { new Form{} };

	// Session state
	struct {
		QString filePath;
		QString suffix;
		QString description;
		QVector<CreateItemRequest> requests;
		bool updated = false;

		void reset() {
			filePath = "";
			description = "";
			requests.clear();
			updated = false;
		};
	} m_temporaryInfo;
	enum State {
		WAITING_FOR_FILE,
		PREPARING_PRE_PROCESSING_FILE,
		PRE_PROCESSING_FILE,
		PREPARING_PARSING_FILE,
		PARSING_FILE,
		CREATING_FORM,
	} m_state = WAITING_FOR_FILE;
	QString m_PDFToTextPrompt = DEFAULT_PDF_TO_TEXT_PROMPT;

	// Parsing
	FileParser* m_parser{ new FileParser{} };

	// Dialogs
	Settings* m_settings{ new Settings{this} };
	Logger* m_logger{ new Logger{this} };

	// Helper functions

	// Processing
	void _process_text_file();
	void _process_pdf_file();
	QVector<CreateItemRequest> _parse_file();
	void _create_form(const QVector<CreateItemRequest>&);


	// Buttons
	void _set_createQuizBtn_state();
	void _set_processPdfBtn_state();
	void _set_shortcuts();

	// State
	void _set_state(State);
	void _reset_to_new_quiz_state();
	void _set_copyright_info();
	void _load_settings();
	bool _is_connected_to_internet();
};
