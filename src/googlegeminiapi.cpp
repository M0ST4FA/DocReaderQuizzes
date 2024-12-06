#include "googlegeminiapi.h"
#include "googlegeminiapihelperclasses.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>

namespace m0st4fa::gemini {

	GoogleGeminiAPI::GoogleGeminiAPI(QObject* parent)
		: QObject(parent)
	{

	}

	GoogleGeminiAPI::~GoogleGeminiAPI()
	{
	}

	void GoogleGeminiAPI::uploadFile(const QString& path)
	{
		this->uploadFileMetadata(path);

		connect(this, &GoogleGeminiAPI::fileMetadataUploaded, [path, this](const QString& uploadUri) {

			QFile file{ path };

			if (!file.open(QIODeviceBase::ReadOnly))
				qInfo() << "Couldn't open file:" << path;

			QNetworkRequest request{uploadUri};
			request.setHeader(QNetworkRequest::ContentLengthHeader, file.size());
			request.setRawHeader("X-Goog-Upload-Offset", "0");
			request.setRawHeader("X-Goog-Upload-Command", "upload, finalize");

			QByteArray binaryData = file.readAll();

			QNetworkReply* reply = this->m_manager->post(request, binaryData);

			connect(reply, &QNetworkReply::finished, [this, reply]() {

				QByteArray fileData = reply->readAll();

				QJsonObject obj = QJsonDocument::fromJson(fileData).object();

				file::File file = file::MediaUpload::fromJson(obj).file;

				emit fileUploaded(file);

				reply->deleteLater();
				});

			});

	}

	void GoogleGeminiAPI::executePrompt(const QString& prompt, const QString& filePath)
	{
		uploadFile(filePath);

		connect(this, &GoogleGeminiAPI::fileUploaded, [this, prompt, filePath](const file::File& file) {
			
			Content content;
			content.addText(prompt);
			content.addFileData(file.mimeType, file.uri);
			RequestBody body{
				.contents = {content}
			};

			QNetworkRequest req{ GENERATE_CONTENT_ENDPOINT };
			req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

			using namespace m0st4fa::gemini;

			QByteArray json = QJsonDocument{ body.toJson() }.toJson();

			req.setHeader(QNetworkRequest::ContentLengthHeader, json.size());

			QNetworkReply* reply = this->m_manager->post(req, json);

			connect(reply, &QNetworkReply::finished, [reply, this]() {

				QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();

				emit promptExecuted(GenerateContentResponse::fromJson(obj));

				reply->deleteLater();
				});


			});
	}

	void GoogleGeminiAPI::uploadFileMetadata(const QString& path)
	{
		QFileInfo file{ path };

		if (!file.exists()) {
			qInfo() << __FUNCTION__ " File" << path << "doesn't exist.";
			return;
		}

		QByteArray contentType;

		if (file.suffix() == "pdf")
			contentType = "application/pdf";
		else if (file.suffix() == "png")
			contentType = "image/png";
		else if (file.suffix() == "jpeg")
			contentType = "image/jpeg";
		else {
			qInfo() << __FUNCTION__ " Suffix" << file.suffix() << "is not an approved suffix.";
			return;
		}

		QNetworkRequest request{ UPLOAD_MEDIA_ENDPOINT };

		request.setRawHeader("X-Goog-Upload-Protocol", "resumable");
		request.setRawHeader("X-Goog-Upload-Command", "start");
		request.setRawHeader("X-Goog-Upload-Header-Content-Length", QString::number(file.size()).toLatin1());
		request.setRawHeader("X-Goog-Upload-Header-Content-Type", contentType);
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

		file::MediaUpload body{
			.file = file::File {
				.displayName = "mcqs"
				}
		};

		QByteArray bodyStr = QJsonDocument(body.toJson()).toJson();

		QNetworkReply* reply = this->m_manager->post(request, bodyStr);

		connect(reply, &QNetworkReply::finished, [this, reply]() {

			qInfo() << "Metadata uploaded.";

			QList<std::pair<QByteArray, QByteArray>> rep = reply->rawHeaderPairs();

			auto it = std::find_if(rep.begin(), rep.end(), [](const std::pair<QByteArray, QByteArray>& headerPair) ->bool {

				if (headerPair.first == "x-goog-upload-url")
					return true;

				return false;
				});

			emit fileMetadataUploaded(it->second);
			reply->deleteLater();
			});
	}

	void GoogleGeminiAPI::executePrompt(const QString& prompt)
	{
		QNetworkRequest req{ GENERATE_CONTENT_ENDPOINT };
		req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

		using namespace m0st4fa::gemini;

		RequestBody reqBody = RequestBody::textPrompt(prompt);

		QByteArray json = QJsonDocument{ reqBody.toJson() }.toJson();

		req.setHeader(QNetworkRequest::ContentLengthHeader, json.size());

		qInfo() << json;

		QNetworkReply* reply = this->m_manager->post(req, json);

		connect(reply, &QNetworkReply::finished, [reply, this]() {

			QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();

			emit promptExecuted(GenerateContentResponse::fromJson(obj));

			reply->deleteLater();
			});

	}

	const QString GoogleGeminiAPI::ENDPOINT_BASE = "https://generativelanguage.googleapis.com";

	const QString GoogleGeminiAPI::GENERATE_CONTENT_ENDPOINT = ENDPOINT_BASE + "/v1beta/models/gemini-1.5-flash:generateContent?key=" GEMINI_API_KEY;

	const QString GoogleGeminiAPI::UPLOAD_MEDIA_ENDPOINT = ENDPOINT_BASE + "/upload/v1beta/files?key=" GEMINI_API_KEY;

}