#include "googleformapi.h"
#include <QNetworkAccessManager>

#include <QNetworkReply>
#include <QFutureWatcher>
#include <QJsonValue>

namespace m0st4fa::forms {
	GoogleFormsAPI::GoogleFormsAPI(QObject* parent)
		: QObject(parent)
		, m_networkManager{ new QNetworkAccessManager{ this } }
	{
	}

	GoogleFormsAPI::~GoogleFormsAPI()
	{
	}

	const QString GoogleFormsAPI::ENDPOINT_BASE = "https://forms.googleapis.com/v1/forms";

	void GoogleFormsAPI::getForm(const QString& formId)
	{
		if (m_accessToken.isEmpty()) {
			emit errorOccurred("Invalid access token.");
			return;
		};

		QFuture<std::shared_ptr<Form>> future = QtConcurrent::run([this, formId]() {
			return this->_fetch_form(formId);
			});

		QFutureWatcher<std::shared_ptr<Form>>* watcher = new QFutureWatcher<std::shared_ptr<Form>>{ this };

		connect(watcher, &QFutureWatcher<std::shared_ptr<Form>>::finished, [this, watcher]() {

			// Check if there was an error or if we got valid data
			if (watcher->isRunning()) {
				qDebug() << "Request is still running.";
				return;
			}

			if (watcher->result() == nullptr) {
				emit errorOccurred("Failed to fetch form.");
			}
			else {
				emit formFetched(watcher->result());
			}

			watcher->deleteLater(); // Clean up
			});

		watcher->setFuture(future); // Set the future for monitoring
	}

	void GoogleFormsAPI::getResponse(const QString& formId, const QString& responseId)
	{
		if (m_accessToken.isEmpty()) {
			emit errorOccurred("Invalid access token.");
			return;
		};

		QFuture<QJsonDocument> future = QtConcurrent::run([this, formId, responseId]() {
			return this->_fetch_response(formId, responseId);
			});

		_handle_get_future<QJsonDocument>(RESPONSE, future, "Failed to fetch response.");

	}

	void GoogleFormsAPI::getResponses(const QString& formId)
	{
		if (m_accessToken.isEmpty()) {
			emit errorOccurred("Invalid access token.");
			return;
		};

		QFuture<QJsonDocument> future = QtConcurrent::run([this, formId]() {
			return this->_fetch_responses(formId);
			});

		_handle_get_future<QJsonDocument>(RESPONSES, future, "Failed to fetch responses.");
	}

	void GoogleFormsAPI::createForm(const QString& title, const QString& documentTitle)
	{

		// Prepare request body
		QJsonObject info{ {
			std::pair<QString, QJsonValue>{"title", title},
			std::pair<QString, QJsonValue>{"documentTitle", documentTitle}
			}
		};

		QJsonObject object{ {
			std::pair<QString, QJsonValue>{"info", info},
			}
		};

		QJsonDocument jsonDoc{ object };

		QByteArray json = jsonDoc.toJson();

		// Prepare request
		QNetworkRequest request = _prepare_request(ENDPOINT_BASE, json.size());

		// Execute request and get response
		QJsonDocument response = _execute_request(request, json);

		qInfo() << response;

	}

	void GoogleFormsAPI::updateForm(const QString& formId, const update_form::UpdateRequestBody& payload)
	{
		QString endpoint = ENDPOINT_BASE + "/" + formId + ":batchUpdate";

		QFuture<update_form::UpdateResponseBody> future = QtConcurrent::run([this, formId, payload]() {
			return this->_update_form(formId, payload);
			});

		QFutureWatcher<update_form::UpdateResponseBody>* watcher = new QFutureWatcher<update_form::UpdateResponseBody>{ this };

		connect(watcher, &QFutureWatcher<QJsonDocument>::finished, [this, watcher]() {

			// Check if there was an error or if we got valid data
			if (watcher->isRunning()) {
				qDebug() << "Request is still running.";
				return;
			}

			emit formUpdated(watcher->result());

			watcher->deleteLater(); // Clean up
			});

		watcher->setFuture(future); // Set the future for monitoring
	}

	void m0st4fa::forms::GoogleFormsAPI::updateFormInfo(const QString& formId, const update_form::UpdateFormInfoRequest& request)
	{
		using namespace update_form;

		// If nothing has changed (nothing is to be updated)
		if (request.updateMask == "")
			return;

		UpdateRequest req{ request };
		UpdateRequestBody reqBody{
			.includeFormInResponse = m_includeFormInResponse,
			.requests = {req},
		};

		this->updateForm(formId, reqBody);
	}

	void GoogleFormsAPI::updateFormSettings(const QString& formId, const update_form::UpdateSettingsRequest& request)
	{
		using namespace update_form;

		// If nothing has changed (nothing to be updated)
		if (request.updateMask == "")
			return;

		UpdateRequest req{ request };
		UpdateRequestBody reqBody{
			.includeFormInResponse = m_includeFormInResponse,
			.requests = {req},
		};

		this->updateForm(formId, reqBody);
	}

	void GoogleFormsAPI::createItems(const QString& formId, const QVector<update_form::CreateItemRequest>& requests)
	{
		using namespace update_form;

		// If nothing has changed (nothing is to be updated)
		QVector<UpdateRequest> requestsVector;

		for (const CreateItemRequest& request : requests)
			requestsVector.append( UpdateRequest{ request });

		UpdateRequestBody reqBody{
			.includeFormInResponse = m_includeFormInResponse,
			.requests = requestsVector,
		};

		this->updateForm(formId, reqBody);
	}

	QString GoogleFormsAPI::accessToken() const
	{
		return this->m_accessToken;
	}

	void GoogleFormsAPI::setAccessToken(const QString& token)
	{
		this->m_accessToken = token;
	}

	bool GoogleFormsAPI::includeFormInResponse() const
	{
		return this->m_includeFormInResponse;
	}

	void GoogleFormsAPI::setIncludeFormInResponse(bool includeFormInResponse)
	{
		this->m_includeFormInResponse = includeFormInResponse;
	}

	QNetworkRequest GoogleFormsAPI::_prepare_request(const QString& endpoint, int contentLength)
	{
		// Configuring the request
		qInfo() << "Requesting content.";
		QNetworkRequest request = QNetworkRequest{ QUrl{endpoint} };

		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
		request.setHeader(QNetworkRequest::ContentLengthHeader, contentLength);

		QString authString = "Bearer " + m_accessToken;
		request.setRawHeader("Authorization", authString.toLatin1());

		return request;
	}

	QJsonDocument GoogleFormsAPI::_execute_request(const QNetworkRequest& request)
	{
		QNetworkAccessManager* networkManager = new QNetworkAccessManager{};

		// Creating the reply
		QNetworkReply* reply = networkManager->get(request);

		// Blocking the thread until the reply's data is ready
		QEventLoop eventLoop;
		connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
		eventLoop.exec();  // Block until finished

		// Receiving the reply's data
		QJsonDocument jsonDoc;

		if (reply->error() == QNetworkReply::NoError) {
			QByteArray responseData = reply->readAll();
			jsonDoc = QJsonDocument::fromJson(responseData);
		}
		else {
			qDebug() << "Error fetching form:" << reply->errorString();
			jsonDoc = QJsonDocument();  // Return an empty document on error
		}

		networkManager->deleteLater();
		reply->deleteLater();

		bool error = _check_json_error(jsonDoc);

		if (!error)
			return jsonDoc;
		else
			return QJsonDocument{};
	}

	QJsonDocument GoogleFormsAPI::_execute_request(const QNetworkRequest& request, const QByteArray& payload)
	{
		QNetworkAccessManager* networkManager = new QNetworkAccessManager{};

		// Creating the reply
		QNetworkReply* reply = networkManager->post(request, payload);

		qInfo() << "Payload for updating info: " << payload;

		// Blocking the thread until the reply's data is ready
		QEventLoop eventLoop;
		connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
		eventLoop.exec();  // Block until finished

		// Receiving the reply's data
		QJsonDocument jsonDoc;

		if (reply->error() == QNetworkReply::NoError) {
			QByteArray responseData = reply->readAll();
			jsonDoc = QJsonDocument::fromJson(responseData);
		}
		else {
			qDebug() << "Error:" << reply->errorString() << reply->readAll();
			jsonDoc = QJsonDocument();  // Return an empty document on error
		}

		networkManager->deleteLater();
		reply->deleteLater();

		bool error = _check_json_error(jsonDoc);

		if (!error)
			return jsonDoc;
		else
			return QJsonDocument{};
	}

	bool GoogleFormsAPI::_check_json_error(const QJsonDocument& json)
	{

		QJsonObject object = json.object();
		QStringList keys = object.keys();
		int errorIndex = keys.indexOf("error");

		if (errorIndex == -1)
			return false;

		QDebug info = qInfo().nospace().noquote();

		info << "Error sent by server.\n";
		info << "Error code: " << object.value("code").toString() << "\n";
		info << "Message: " << object.value("message").toString() << "\n";
		info << "Status: " << object.value("status").toString() << "\n";

		return true;
	}

	std::shared_ptr<Form> GoogleFormsAPI::_fetch_form(const QString& formId)
	{
		// Configuring the request
		QString endpoint = QString{ ENDPOINT_BASE } + "/" + formId;
		QNetworkRequest request = _prepare_request(endpoint);

		// Executing the request
		QJsonDocument jsonDoc = _execute_request(request);

		if (jsonDoc.isNull())
			return std::shared_ptr<Form>{ nullptr };

		// Convert document to form
		std::shared_ptr<Form> form = std::shared_ptr<Form>(new Form{ Form::fromJson(jsonDoc.object()) });

		return form;  // Return the JSON document
	}

	QJsonDocument GoogleFormsAPI::_fetch_response(const QString& formId, const QString& responseId)
	{
		// Configuring the request
		QString endpoint = QString{ ENDPOINT_BASE } + "/" + formId + "/responses/" + responseId;
		QNetworkRequest request = _prepare_request(endpoint);

		// Executing the request
		QJsonDocument jsonDoc = _execute_request(request);

		return jsonDoc;  // Return the JSON document
	}

	QJsonDocument GoogleFormsAPI::_fetch_responses(const QString& formId)
	{
		// Configuring the request
		QString endpoint = QString{ ENDPOINT_BASE } + "/" + formId + "/responses";
		QNetworkRequest request = _prepare_request(endpoint);

		// Executing the request
		QJsonDocument jsonDoc = _execute_request(request);

		return jsonDoc;  // Return the JSON document
	}

	update_form::UpdateResponseBody GoogleFormsAPI::_update_form(const QString& formId, update_form::UpdateRequestBody payload)
	{

		QString endpoint = ENDPOINT_BASE + "/" + formId + ":batchUpdate";

		QByteArray str = QJsonDocument{ payload.toJson() }.toJson();

		QNetworkRequest request = _prepare_request(endpoint, str.size());
		update_form::UpdateResponseBody reply = update_form::UpdateResponseBody::fromJson(_execute_request(request, str).object());

		return reply;
	}

}