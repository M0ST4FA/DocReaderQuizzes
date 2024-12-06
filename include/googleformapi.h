#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QFuture>
#include <QtConcurrent>
#include "googleformsapihelperclasses.h"

namespace m0st4fa::forms {
	class GoogleFormsAPI : public QObject
	{
		Q_OBJECT;

		enum GET_TYPE {
			FORM,
			RESPONSE,
			RESPONSES
		};

	public:
		GoogleFormsAPI(QObject*);
		~GoogleFormsAPI();

		QString accessToken() const;
		void setAccessToken(const QString&);

		bool includeFormInResponse() const;
		void setIncludeFormInResponse(bool = true);

	public slots:
		void getForm(const QString&);
		void executePrompt(const QString&, const QString&);
		void getResponses(const QString&);

		void createForm(const QString&, const QString&);

		void updateForm(const QString&, const update_form::UpdateRequestBody&);

		void updateFormInfo(const QString&, const update_form::UpdateFormInfoRequest&);

		void updateFormSettings(const QString&, const update_form::UpdateSettingsRequest&);

		void createItems(const QString&, const QVector<update_form::CreateItemRequest>&);

	signals:

		void formFetched(std::shared_ptr<Form> form);
		void responseFetched(const QJsonDocument& response);
		void responsesFetched(const QJsonDocument& responses);

		void formCreated(std::shared_ptr<Form> form);

		void formUpdated(const update_form::UpdateResponseBody);

		void errorOccurred(const QString& errorMsg);

	private:

		static const QString ENDPOINT_BASE;

		QString m_accessToken;

		bool m_includeFormInResponse = true;

		// FUNCTIONS
		template<typename T = QJsonDocument>
		void _handle_get_future(GET_TYPE, const QFuture<T>& future, const QString&);

		// Dealing with requests
		QNetworkRequest _prepare_request(const QString&, int = 0);
		QJsonDocument _execute_request(const QNetworkRequest&);
		QJsonDocument _execute_request(const QNetworkRequest&, const QByteArray&);
		bool _check_json_error(const QJsonDocument&);

		// Dealing with forms
		std::shared_ptr<Form> _fetch_form(const QString&);
		QJsonDocument _fetch_response(const QString&, const QString&);
		QJsonDocument _fetch_responses(const QString&);
		update_form::UpdateResponseBody _update_form(const QString&, update_form::UpdateRequestBody);

	};

	template<typename T>
	inline void GoogleFormsAPI::_handle_get_future(GET_TYPE type, const QFuture<T>& future, const QString& errorMsg)
	{
		QFutureWatcher<QJsonDocument>* watcher = new QFutureWatcher<QJsonDocument>{ this };

		connect(watcher, &QFutureWatcher<QJsonDocument>::finished, [this, type, watcher, errorMsg]() {

			// Check if there was an error or if we got valid data
			if (watcher->isRunning()) {
				qDebug() << "Request is still running.";
				return;
			}

			if (watcher->result().isNull()) {
				emit errorOccurred(errorMsg);
			}
			else {
				switch (type)
				{
				case GoogleFormsAPI::RESPONSE:
					emit responseFetched(watcher->result());
					break;
				case GoogleFormsAPI::RESPONSES:
					emit responsesFetched(watcher->result());
					break;
				default:
					break;
				}
			}

			watcher->deleteLater(); // Clean up
			});

		watcher->setFuture(future); // Set the future for monitoring
	}

}