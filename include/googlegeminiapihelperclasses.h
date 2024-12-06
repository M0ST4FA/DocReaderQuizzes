#pragma once
#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace m0st4fa::gemini::enums {

	enum class HarmCategory {
		HARM_CATEGORY_UNSPECIFIED,
		DEROGATORY,
		TOXICITY,
		VIOLENCE,
		SEXUAL,
		MEDICAL,
		DANGEROUS,
		HARASSMENT,
		HATE_SPEECH,
		SEXUALLY_EXPLICIT,
		DANGEROUS_CONTENT,
		CIVIC_INTEGRITY
	};

	QString toString(HarmCategory);
	HarmCategory stringToHarmCategory(const QString& str);

	enum class HarmProbability {
		HARM_PROBABILITY_UNSPECIFIED,
		NEGLIGIBLE,
		LOW,
		MEDIUM,
		HIGH
	};

	QString toString(HarmProbability);
	HarmProbability stringToHarmProbability(const QString& str);

	enum class HarmBlockThreshold {
		HARM_BLOCK_THRESHOLD_UNSPECIFIED,
		BLOCK_LOW_AND_ABOVE,
		BLOCK_MEDIUM_AND_ABOVE,
		BLOCK_ONLY_HIGH,
		BLOCK_NONE,
		OFF
	};

	QString toString(HarmBlockThreshold);
	HarmBlockThreshold stringToHarmBlockThreshold(const QString& str);

	enum class BlockReason {
		BLOCK_REASON_UNSPECIFIED,
		SAFETY,
		OTHER,
		BLOCKLIST,
		PROHIBITED_CONTENT
	};

	QString toString(BlockReason reason);
	BlockReason stringToBlockReason(const QString& str);

	enum class FinishReason {
		FINISH_REASON_UNSPECIFIED,
		STOP,
		MAX_TOKENS,
		SAFETY,
		RECITATION,
		LANGUAGE,
		OTHER,
		BLOCKLIST,
		PROHIBITED_CONTENT,
		SPII,
		MALFORMED_FUNCTION_CALL
	};

	QString toString(FinishReason reason);
	FinishReason stringToFinishReason(const QString& str);

	enum class State {
		STATE_UNSPECIFIED,
		PROCESSING,
		ACTIVE,
		FAILED
	};

	QString toString(State);
	State stringToState(const QString&);

}

namespace m0st4fa::gemini::file {

	struct Status {
		int code = 0;
		QString message;
		QVector<QString> details;

		bool isEmpty() const {
			return this->code == 0 && this->message.isEmpty() && this->details.isEmpty();
		}

		QJsonObject toJson() const {
			QJsonObject obj;
			obj["code"] = code;
			obj["message"] = message;

			QJsonArray detailsArray;
			for (const QString& detail : details) {
				detailsArray.append(detail);
			}
			obj["details"] = detailsArray;

			return obj;
		}

		static Status fromJson(const QJsonObject& obj) {
			Status status;
			status.code = obj["code"].toInt();
			status.message = obj["message"].toString();

			QJsonArray detailsArray = obj["details"].toArray();
			for (const QJsonValue& value : detailsArray) {
				status.details.append(value.toString());
			}

			return status;
		}

	};

	struct VideoMetadata {
		QString videoDuration;

		bool isEmpty() const {
			return this->videoDuration.isEmpty();
		}

		QJsonObject toJson() const {
			QJsonObject obj;
			obj["videoDuration"] = videoDuration;
			return obj;
		}
		static VideoMetadata fromJson(const QJsonObject& obj) {
			VideoMetadata metadata;
			metadata.videoDuration = obj["videoDuration"].toString();
			return metadata;
		}
	};

	struct File {
		QString name;
		QString displayName;
		QString mimeType;
		QString sizeBytes;
		QString createTime;
		QString updateTime;
		QString expirationTime;
		QString sha256Hash;
		QString uri;

		enums::State state = enums::State::STATE_UNSPECIFIED;
		Status error;
		VideoMetadata videMetadata;

		QJsonObject toJson() const {
			QJsonObject obj;

			if (!name.isEmpty())
				obj["name"] = name;
			if (!displayName.isEmpty()) {
				obj["displayName"] = displayName;
			}

			if (!mimeType.isEmpty())
				obj["mimeType"] = mimeType;

			if (!sizeBytes.isEmpty())
				obj["sizeBytes"] = sizeBytes;

			if (!createTime.isEmpty())
				obj["createTime"] = createTime;

			if (!updateTime.isEmpty())
				obj["updateTime"] = updateTime;

			if (!expirationTime.isEmpty())
				obj["expirationTime"] = expirationTime;

			if (!sha256Hash.isEmpty())
				obj["sha256Hash"] = sha256Hash;

			if (!uri.isEmpty())
				obj["uri"] = uri;

			if (state != enums::State::STATE_UNSPECIFIED)
				obj["state"] = toString(state);

			if (!error.isEmpty())
				obj["error"] = error.toJson();

			if (!videMetadata.isEmpty())
				obj["videoMetadata"] = videMetadata.toJson();

			return obj;
		}

		static File fromJson(const QJsonObject& obj) {
			File file;
			file.name = obj["name"].toString();
			if (obj.contains("displayName")) {
				file.displayName = obj["displayName"].toString();
			}
			file.mimeType = obj["mimeType"].toString();
			file.sizeBytes = obj["sizeBytes"].toString();
			file.createTime = obj["createTime"].toString();
			file.updateTime = obj["updateTime"].toString();
			file.expirationTime = obj["expirationTime"].toString();
			file.sha256Hash = obj["sha256Hash"].toString();
			file.uri = obj["uri"].toString();

			if (obj.contains("state"))
				file.state = enums::stringToState(obj.value("state").toString());

			file.error = Status::fromJson(obj.value("error").toObject());

			if (obj.contains("videoMetadata")) {
				file.videMetadata = VideoMetadata::fromJson(obj.value("videoMetadata").toObject());
			}

			return file;
		}

	};

	struct MediaUpload {
		File file;

		QJsonObject toJson() const {
			QJsonObject obj;
			obj["file"] = file.toJson();
			return obj;
		};

		static MediaUpload fromJson(const QJsonObject& json) {
			return MediaUpload{
				.file = File::fromJson(json.value("file").toObject())
			};
		}
	};

}

// Request body
namespace m0st4fa::gemini {

	struct Blob {
		QString mimeType;
		QByteArray data;

		QJsonObject toJson() const;
		static Blob fromJson(const QJsonObject&);
	};

	//struct FunctionCall {
	//	QString name;

	//	// optional
	//	QJsonObject args;
	//};

	//struct FunctionResponse {

	//};

	struct FileData {
		QString mimeType;
		QString fileUri;

		QJsonObject toJson() const;
		static FileData fromJson(const QJsonObject&);
	};

	/*struct ExecutableCode {

	};

	struct CodeExecutionResult {

	};*/

	struct Part {
		using PartType = std::variant<QString, Blob, FileData>;

		PartType part;

		QJsonObject toJson() const;
		static Part fromJson(const QJsonObject&);

	};

	struct Content {
		QVector<Part> parts;
		QString role;

		void addText(const QString&);
		void addFileData(const QString&, const QString&);

		QJsonObject toJson() const;
		static Content fromJson(const QJsonObject&);

		static Content textPrompt(const QString&);

	};

	struct RequestBody {
		QVector<Content> contents;

		QJsonObject toJson() const;
		static RequestBody fromJson(const QJsonObject&);

		static RequestBody textPrompt(const QString&);
	};

}

// Response body
namespace m0st4fa::gemini {

	//struct Tool {

	//};

	//struct UsageMetadata {

	//};

	//struct ToolConfig {

	//};

	//struct CachedContent {
	//	QVector<Content> contents;
	//	QVector<Tool> tools;
	//	QString createTime;
	//	QString updateTime;
	//	UsageMetadata usageMetadata;
	//	std::variant<QString, QString> expiration; // expireTime or ttl
	//	QString name;
	//	QString displayName;
	//	QString model;
	//	Content systemInstruction;
	//	ToolConfig toolConfig;

	//	QJsonObject toJson() const {
	//		QJsonObject obj;

	//		if (!this->contents.isEmpty()) {
	//			QJsonArray cnts;

	//			for (const Content& cnt : this->contents)
	//				cnts.append(cnt.toJson());

	//			obj.insert("contents", cnts);
	//		}

	//		obj["tools"] = "";
	//		obj["createTime"] = createTime;
	//		obj["updateTime"] = updateTime;
	//		obj["usageMetadata"] = usageMetadata.toJson();

	//		if (std::holds_alternative<QString>(expiration)) {
	//			obj["expireTime"] = std::get<QString>(expiration);
	//		}
	//		else {
	//			obj["ttl"] = std::get<QString>(expiration);
	//		}

	//		obj["name"] = name;
	//		obj["displayName"] = displayName;
	//		obj["model"] = model;
	//		obj["systemInstruction"] = systemInstruction.toJson();
	//		obj["toolConfig"] = toolConfig.toJson();
	//		return obj;
	//	}

	//	static CachedContent fromJson(const QJsonObject& obj) {
	//		CachedContent content;
	//		content.contents = obj["contents"].toArray().toVariantList().map([](const QJsonValue& value) {
	//			return Content::fromJson(value.toObject());
	//			}).toVector();
	//		content.tools = obj["tools"].toArray().toVariantList().map([](const QJsonValue& value) {
	//			return Tool::fromJson(value.toObject());
	//			}).toVector();
	//		content.createTime = obj["createTime"].toString();
	//		content.updateTime = obj["updateTime"].toString();
	//		content.usageMetadata = UsageMetadata::fromJson(obj["usageMetadata"].toObject());
	//		content.expiration = obj["expireTime"].toString().isEmpty() ? obj["ttl"].toString() : obj["expireTime"].toString();
	//		content.name = obj["name"].toString();
	//		content.displayName = obj["displayName"].toString();
	//		content.model = obj["model"].toString();
	//		content.systemInstruction = Content::fromJson(obj["systemInstruction"].toObject());
	//		content.toolConfig = ToolConfig::fromJson(obj["toolConfig"].toObject());
	//		return content;
	//	}
	//};

	struct SafetyRating {

		enums::HarmCategory category = enums::HarmCategory::HARM_CATEGORY_UNSPECIFIED;
		enums::HarmProbability probability = enums::HarmProbability::HARM_PROBABILITY_UNSPECIFIED;
		bool blocked = false;

		QJsonObject toJson() const;
		static SafetyRating fromJson(const QJsonObject&);
	};

	struct SafetySetting {
		enums::HarmCategory category = enums::HarmCategory::HARM_CATEGORY_UNSPECIFIED;
		enums::HarmBlockThreshold threshold = enums::HarmBlockThreshold::HARM_BLOCK_THRESHOLD_UNSPECIFIED;

		QJsonObject toJson() const;
		static SafetySetting fromJson(const QJsonObject);
	};

	struct PromptFeedback {
		enums::BlockReason blockReason = enums::BlockReason::BLOCK_REASON_UNSPECIFIED;
		QVector<SafetyRating> safetyRatings;

		bool isEmpty() const {
			return blockReason == enums::BlockReason::BLOCK_REASON_UNSPECIFIED && safetyRatings.isEmpty();
		}

		QJsonObject toJson() const;
		static PromptFeedback fromJson(const QJsonObject&);
	};

	struct SemanticRetrieverChunk {
		QString source;
		QString chunk;

		QJsonObject toJson() const {
			QJsonObject obj;
			obj["source"] = source;
			obj["chunk"] = chunk;
			return obj;
		}

		static SemanticRetrieverChunk fromJson(const QJsonObject& obj) {
			SemanticRetrieverChunk chunk;
			chunk.source = obj["source"].toString();
			chunk.chunk = obj["chunk"].toString();
			return chunk;
		}
	};

	struct AttributionSourceId {
		// Assuming these fields are strings or integers based on the JSON representation
		QString passageId;
		int partIndex;

		QJsonObject toJson() const {
			QJsonObject obj;
			obj["passageId"] = passageId;
			obj["partIndex"] = partIndex;
			return obj;
		}

		static AttributionSourceId fromJson(const QJsonObject& obj) {
			AttributionSourceId id;
			id.passageId = obj["passageId"].toString();
			id.partIndex = obj["partIndex"].toInt();
			return id;
		}
	};

	struct GroundingAttribution {
		AttributionSourceId sourceId;
		Content content;

		QJsonObject toJson() const {
			QJsonObject obj;
			obj["sourceId"] = sourceId.toJson();
			obj["content"] = content.toJson();
			return obj;
		}

		static GroundingAttribution fromJson(const QJsonObject& obj) {
			GroundingAttribution attribution;
			attribution.sourceId = AttributionSourceId::fromJson(obj["sourceId"].toObject());
			attribution.content = Content::fromJson(obj["content"].toObject());
			return attribution;
		}
	};

	struct CitationSource {
		int startIndex;
		int endIndex;
		QString uri;
		QString license;

		QJsonObject toJson() const {
			QJsonObject obj;
			obj["startIndex"] = startIndex;
			obj["endIndex"] = endIndex;
			if (!uri.isEmpty()) {
				obj["uri"] = uri;
			}
			if (!license.isEmpty()) {
				obj["license"] = license;
			}
			return obj;
		}

		static CitationSource fromJson(const QJsonObject& obj) {
			CitationSource source;
			source.startIndex = obj["startIndex"].toInt();
			source.endIndex = obj["endIndex"].toInt();
			if (obj.contains("uri")) {
				source.uri = obj["uri"].toString();
			}
			if (obj.contains("license")) {
				source.license = obj["license"].toString();
			}
			return source;
		}
	};

	struct CitationMetadata {
		QVector<CitationSource> citationSources;

		bool isEmpty() const {
			return this->citationSources.isEmpty();
		}

		QJsonObject toJson() const {
			QJsonObject obj;
			QJsonArray citationSourcesArray;
			for (const auto& source : citationSources) {
				citationSourcesArray.append(source.toJson());
			}
			obj["citationSources"] = citationSourcesArray;
			return obj;
		}

		static CitationMetadata fromJson(const QJsonObject& obj) {
			CitationMetadata metadata;
			QJsonArray citationSourcesArray = obj["citationSources"].toArray();
			for (const QJsonValue& value : citationSourcesArray) {
				metadata.citationSources.push_back(CitationSource::fromJson(value.toObject()));
			}
			return metadata;
		}
	};

	struct SearchEntryPoint {
		QString renderedContent;
		QString sdkBlob;

		bool isEmpty() const {
			return this->renderedContent.isEmpty() && this->sdkBlob.isEmpty();
		}

		QJsonObject toJson() const {
			QJsonObject obj;
			if (!renderedContent.isEmpty()) {
				obj["renderedContent"] = renderedContent;
			}
			if (!sdkBlob.isEmpty()) {
				obj["sdkBlob"] = sdkBlob;
			}
			return obj;
		}

		static SearchEntryPoint fromJson(const QJsonObject& obj) {
			SearchEntryPoint entryPoint;
			if (obj.contains("renderedContent")) {
				entryPoint.renderedContent = obj["renderedContent"].toString();
			}
			if (obj.contains("sdkBlob")) {
				entryPoint.sdkBlob = obj["sdkBlob"].toString();
			}
			return entryPoint;
		}
	};

	struct Segment {
		int partIndex;
		int startIndex;
		int endIndex;
		QString text;

		QJsonObject toJson() const {
			QJsonObject obj;
			obj["partIndex"] = partIndex;
			obj["startIndex"] = startIndex;
			obj["endIndex"] = endIndex;
			obj["text"] = text;
			return obj;
		}

		static Segment fromJson(const QJsonObject& obj) {
			Segment segment;
			segment.partIndex = obj["partIndex"].toInt();
			segment.startIndex = obj["startIndex"].toInt();
			segment.endIndex = obj["endIndex"].toInt();
			segment.text = obj["text"].toString();
			return segment;
		}
	};

	struct GroundingSupport {
		QVector<int> groundingChunkIndices;
		QVector<double> confidenceScores;
		Segment segment;

		QJsonObject toJson() const {
			QJsonObject obj;
			QJsonArray groundingChunkIndicesArray;
			for (int index : groundingChunkIndices) {
				groundingChunkIndicesArray.append(index);
			}
			obj["groundingChunkIndices"] = groundingChunkIndicesArray;

			QJsonArray confidenceScoresArray;
			for (double score : confidenceScores) {
				confidenceScoresArray.append(score);
			}
			obj["confidenceScores"] = confidenceScoresArray;

			obj["segment"] = segment.toJson();
			return obj;
		}

		static GroundingSupport fromJson(const QJsonObject& obj) {
			GroundingSupport support;
			QJsonArray groundingChunkIndicesArray = obj["groundingChunkIndices"].toArray();
			for (const QJsonValue& value : groundingChunkIndicesArray) {
				support.groundingChunkIndices.push_back(value.toInt());
			}

			QJsonArray confidenceScoresArray = obj["confidenceScores"].toArray();
			for (const QJsonValue& value : confidenceScoresArray) {
				support.confidenceScores.push_back(value.toDouble());
			}

			support.segment = Segment::fromJson(obj["segment"].toObject());
			return support;
		}
	};

	struct Web {
		QString uri;
		QString title;

		QJsonObject toJson() const {
			QJsonObject obj;
			obj["uri"] = uri;
			obj["title"] = title;
			return obj;
		}

		static Web fromJson(const QJsonObject& obj) {
			Web web;
			web.uri = obj["uri"].toString();
			web.title = obj["title"].toString();
			return web;
		}
	};

	struct GroundingChunk {
		std::variant<Web> chunkType;

		QJsonObject toJson() const {
			QJsonObject obj;
			if (std::holds_alternative<Web>(chunkType)) {
				obj["web"] = std::get<Web>(chunkType).toJson();
			}
			return obj;
		}

		static GroundingChunk fromJson(const QJsonObject& obj) {
			GroundingChunk chunk;
			if (obj.contains("web")) {
				chunk.chunkType = Web::fromJson(obj["web"].toObject());
			}
			return chunk;
		}
	};

	struct RetrievalMetadata {
		double googleSearchDynamicRetrievalScore;

		QJsonObject toJson() const {
			QJsonObject obj;
			obj["googleSearchDynamicRetrievalScore"] = googleSearchDynamicRetrievalScore;
			return obj;
		}

		static RetrievalMetadata fromJson(const QJsonObject& obj) {
			RetrievalMetadata metadata;
			metadata.googleSearchDynamicRetrievalScore = obj["googleSearchDynamicRetrievalScore"].toDouble();
			return metadata;
		}
	};

	struct GroundingMetadata {
		QVector<GroundingChunk> groundingChunks;
		QVector<GroundingSupport> groundingSupports;
		QStringList webSearchQueries;
		SearchEntryPoint searchEntryPoint;
		RetrievalMetadata retrievalMetadata;

		bool isEmpty() const {
			return this->groundingChunks.isEmpty() && this->groundingSupports.isEmpty() && webSearchQueries.isEmpty() && searchEntryPoint.isEmpty();
		}

		QJsonObject toJson() const {
			QJsonObject obj;
			QJsonArray groundingChunksArray;
			for (const auto& chunk : groundingChunks) {
				groundingChunksArray.append(chunk.toJson());
			}
			obj["groundingChunks"] = groundingChunksArray;

			QJsonArray groundingSupportsArray;
			for (const auto& support : groundingSupports) {
				groundingSupportsArray.append(support.toJson());
			}
			obj["groundingSupports"] = groundingSupportsArray;

			obj["webSearchQueries"] = QJsonArray::fromStringList(this->webSearchQueries);

			if (!searchEntryPoint.isEmpty()) {
				obj["searchEntryPoint"] = searchEntryPoint.toJson();
			}

			obj["retrievalMetadata"] = retrievalMetadata.toJson();
			return obj;
		}

		static GroundingMetadata fromJson(const QJsonObject& obj) {
			GroundingMetadata metadata;
			QJsonArray groundingChunksArray = obj["groundingChunks"].toArray();
			for (const QJsonValue& value : groundingChunksArray) {
				metadata.groundingChunks.push_back(GroundingChunk::fromJson(value.toObject()));
			}

			QJsonArray groundingSupportsArray = obj["groundingSupports"].toArray();
			for (const QJsonValue& value : groundingSupportsArray) {
				metadata.groundingSupports.push_back(GroundingSupport::fromJson(value.toObject()));
			}

			metadata.webSearchQueries = QStringList{};
			for (const QJsonValue& val : obj["webSearchQueries"].toArray())
				metadata.webSearchQueries.append(val.toString());
			
			if (obj.contains("searchEntryPoint")) {
				metadata.searchEntryPoint = SearchEntryPoint::fromJson(obj["searchEntryPoint"].toObject());
			}

			metadata.retrievalMetadata = RetrievalMetadata::fromJson(obj["retrievalMetadata"].toObject());
			return metadata;
		}
	};

	struct LogprobsCandidate {
		QString token;
		int tokenId;
		double logProbability;

		QJsonObject toJson() const {
			QJsonObject obj;
			obj["token"] = token;
			obj["tokenId"] = tokenId;
			obj["logProbability"] = logProbability;
			return obj;
		}

		static LogprobsCandidate fromJson(const QJsonObject& obj) {
			LogprobsCandidate candidate;
			candidate.token = obj["token"].toString();
			candidate.tokenId = obj["tokenId"].toInt();
			candidate.logProbability = obj["logProbability"].toDouble();
			return candidate;
		}
	};

	struct TopLogprobsCandidates {
		QVector<LogprobsCandidate> candidates;

		QJsonObject toJson() const {
			QJsonObject obj;
			QJsonArray candidatesArray;
			for (const auto& candidate : candidates) {
				candidatesArray.append(candidate.toJson());
			}
			obj["candidates"] = candidatesArray;
			return obj;
		}

		static TopLogprobsCandidates fromJson(const QJsonObject& obj) {
			TopLogprobsCandidates candidates;
			QJsonArray candidatesArray = obj["candidates"].toArray();
			for (const QJsonValue& value : candidatesArray) {
				candidates.candidates.push_back(LogprobsCandidate::fromJson(value.toObject()));
			}
			return candidates;
		}
	};

	struct LogprobsResult {
		QVector<TopLogprobsCandidates> topCandidates;
		QVector<LogprobsCandidate> chosenCandidates;

		QJsonObject toJson() const {
			QJsonObject obj;
			QJsonArray topCandidatesArray;
			for (const auto& candidates : topCandidates) {
				topCandidatesArray.append(candidates.toJson());
			}
			obj["topCandidates"] = topCandidatesArray;

			QJsonArray chosenCandidatesArray;
			for (const auto& candidate : chosenCandidates) {
				chosenCandidatesArray.append(candidate.toJson());
			}
			obj["chosenCandidates"] = chosenCandidatesArray;
			return obj;
		}

		static LogprobsResult fromJson(const QJsonObject& obj) {
			LogprobsResult result;
			QJsonArray topCandidatesArray = obj["topCandidates"].toArray();
			for (const QJsonValue& value : topCandidatesArray) {
				result.topCandidates.push_back(TopLogprobsCandidates::fromJson(value.toObject()));
			}

			QJsonArray chosenCandidatesArray = obj["chosenCandidates"].toArray();
			for (const QJsonValue& value : chosenCandidatesArray) {
				result.chosenCandidates.push_back(LogprobsCandidate::fromJson(value.toObject()));
			}
			return result;
		}
	};

	struct Candidate {
		Content content;
		enums::FinishReason finishReason;
		QVector<SafetyRating> safetyRatings;
		CitationMetadata citationMetadata;
		int tokenCount = 0;
		QVector<GroundingAttribution> groundingAttributions;
		GroundingMetadata groundingMetadata;
		double avgLogprob = 0;
		LogprobsResult logprobsResult;
		int index = 0;

		QJsonObject toJson() const;
		static Candidate fromJson(const QJsonObject&);
	};

	struct UsageMetadata {
		int promptTokenCount;
		int cachedContentTokenCount;
		int candidatesTokenCount;
		int totalTokenCount;

		QJsonObject toJson() const;
		static UsageMetadata fromJson(const QJsonObject& obj);
	};

	struct GenerateContentResponse {
		QVector<Candidate> candidates;
		QString modelVersion;
		PromptFeedback promptFeedback;
		UsageMetadata usageMetadata;

		QJsonObject toJson() const {
			QJsonObject obj;
			QJsonArray candidatesArray;
			for (const auto& candidate : candidates) {
				candidatesArray.append(candidate.toJson());
			}
			obj["candidates"] = candidatesArray;

			obj["modelVersion"] = this->modelVersion;

			if (!promptFeedback.isEmpty())
				obj["promptFeedback"] = promptFeedback.toJson();

			obj["usageMetadata"] = usageMetadata.toJson();
			return obj;
		}
		static GenerateContentResponse fromJson(const QJsonObject& obj) {
			GenerateContentResponse response;
			QJsonArray candidatesArray = obj["candidates"].toArray();
			for (const QJsonValue& value : candidatesArray) {
				response.candidates.push_back(Candidate::fromJson(value.toObject()));
			}

			if (obj.contains("modelVersion"))
				response.modelVersion = obj.value("modelVersion").toString();

			if (obj.contains("promptFeedback"))
				response.promptFeedback = PromptFeedback::fromJson(obj["promptFeedback"].toObject());

			response.usageMetadata = UsageMetadata::fromJson(obj["usageMetadata"].toObject());
			return response;
		}
	};

}
