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

	void token_granted();
	void form_created(std::shared_ptr<Form>);
	void form_updated(const UpdateResponseBody&);

	void on_loginBtn_clicked();

	void on_chooseFileBtn_clicked();

	void on_createQuizBtn_clicked();

private:

	Ui::DocReaderQuizzesClass *ui;

	bool m_authenticated = false;

	QString scope{ "https://www.googleapis.com/auth/forms.body https://www.googleapis.com/auth/drive https://www.googleapis.com/auth/drive.file" };
	GoogleSSO* m_sso = new GoogleSSO{ scope, this };

	m0st4fa::forms::GoogleFormsAPI* m_formsApi = new GoogleFormsAPI{this};
	std::shared_ptr<m0st4fa::forms::Form> m_form = std::shared_ptr<Form>{ new Form{} };

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

	FileParser* m_parser = new FileParser{this->m_form};

	void _reset_to_new_quiz_state();
};
