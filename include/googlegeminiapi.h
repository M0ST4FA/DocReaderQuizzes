#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include "googlegeminiapihelperclasses.h"

namespace m0st4fa::gemini {
	class GoogleGeminiAPI : public QObject
	{
		Q_OBJECT

	public:
		GoogleGeminiAPI(QObject* parent);
		~GoogleGeminiAPI();

	public slots:

		void executePrompt(const QString&);

		void executePrompt(const QString&, const QString&);

		void uploadFileMetadata(const QString&);
		void uploadFile(const QString&);

	signals:

		void promptExecuted(const GenerateContentResponse&);
		void fileMetadataUploaded(const QString& uploadUri);
		void fileUploaded(const file::File&);

	private:

		static const QString ENDPOINT_BASE;
		static const QString GENERATE_CONTENT_ENDPOINT;
		static const QString UPLOAD_MEDIA_ENDPOINT;
		static const QString UPLOAD_FILE_CONTENT_ENDPOINT;

		QNetworkAccessManager* m_manager = new QNetworkAccessManager{ this };

	};
}