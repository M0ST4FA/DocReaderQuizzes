#pragma once

#include <QWidget>
#include "ui_docreaderquizzes.h"
#include "googlesso.h"
#include "googleformapi.h"
#include "fileparser.h"


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

private:

	// UI
	std::shared_ptr<Ui::DocReaderQuizzesClass> ui{ new Ui::DocReaderQuizzesClass() };
	static const QString IMAGE_TO_TEXT_PROMPT;
	static const QString FORMAT_TEXT_PROMPT;

	// Authentication
	bool m_authenticated = false;
	QString scope{ "https://www.googleapis.com/auth/forms.body https://www.googleapis.com/auth/drive.file" };
	GoogleSSO* m_sso{ new GoogleSSO{ scope, this } };

	// Forms
	GoogleFormsAPI* m_formsApi{ new GoogleFormsAPI{this} };
	std::shared_ptr<Form> m_form { new Form{} };

	// Session state
	struct {
		QString filePath;
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
		PARSING_FILE,
		CREATING_FORM,
	} m_state = WAITING_FOR_FILE;

	// Parsing
	FileParser* m_parser{ new FileParser{} };

	// Helper functions
	void _process_text_file();
	void _process_pdf_file();
	QVector<CreateItemRequest> _parse_file();
	void _create_form(const QVector<CreateItemRequest>&);
	void _reset_to_new_quiz_state();
	void _set_copyright_info();
	void _set_createQuizBtn_state();
	void _set_state(State);
};
