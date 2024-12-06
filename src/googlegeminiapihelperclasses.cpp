#include "googlegeminiapihelperclasses.h"
#include <QJsonArray>

namespace m0st4fa::gemini::enums {

	QString toString(HarmCategory category)
	{
		switch (category) {
		case HarmCategory::HARM_CATEGORY_UNSPECIFIED:
			return "HARM_CATEGORY_UNSPECIFIED";
		case HarmCategory::DEROGATORY:
			return "DEROGATORY";
		case HarmCategory::TOXICITY:
			return "TOXICITY";
		case HarmCategory::VIOLENCE:
			return "VIOLENCE";
		case HarmCategory::SEXUAL:
			return "SEXUAL";
		case HarmCategory::MEDICAL:
			return "MEDICAL";
		case HarmCategory::DANGEROUS:
			return "DANGEROUS";
		case HarmCategory::HARASSMENT:
			return "HARASSMENT";
		case HarmCategory::HATE_SPEECH:
			return "HATE_SPEECH";
		case HarmCategory::SEXUALLY_EXPLICIT:
			return "SEXUALLY_EXPLICIT";
		case HarmCategory::DANGEROUS_CONTENT:
			return "DANGEROUS_CONTENT";
		case HarmCategory::CIVIC_INTEGRITY:
			return "CIVIC_INTEGRITY";
		default:
			return "UNKNOWN_HARM_CATEGORY";
		}
	}
	HarmCategory stringToHarmCategory(const QString& str)
	{
		if (str == "HARM_CATEGORY_UNSPECIFIED") {
			return HarmCategory::HARM_CATEGORY_UNSPECIFIED;
		}
		else if (str == "DEROGATORY") {
			return HarmCategory::DEROGATORY;
		}
		else if (str == "TOXICITY") {
			return HarmCategory::TOXICITY;
		}
		else if (str == "VIOLENCE") {
			return HarmCategory::VIOLENCE;
		}
		else if (str == "SEXUAL") {
			return HarmCategory::SEXUAL;
		}
		else if (str == "MEDICAL") {
			return HarmCategory::MEDICAL;
		}
		else if (str == "DANGEROUS") {
			return HarmCategory::DANGEROUS;
		}
		else if (str == "HARASSMENT") {
			return HarmCategory::HARASSMENT;
		}
		else if (str == "HATE_SPEECH") {
			return HarmCategory::HATE_SPEECH;
		}
		else if (str == "SEXUALLY_EXPLICIT") {
			return HarmCategory::SEXUALLY_EXPLICIT;
		}
		else if (str == "DANGEROUS_CONTENT") {
			return HarmCategory::DANGEROUS_CONTENT;
		}
		else if (str == "CIVIC_INTEGRITY") {
			return HarmCategory::CIVIC_INTEGRITY;
		}
		else {
			return HarmCategory::HARM_CATEGORY_UNSPECIFIED; // or throw an exception
		}
	}

	QString toString(HarmProbability probability)
	{
		switch (probability) {
		case HarmProbability::HARM_PROBABILITY_UNSPECIFIED:
			return "HARM_PROBABILITY_UNSPECIFIED";
		case HarmProbability::NEGLIGIBLE:
			return "NEGLIGIBLE";
		case HarmProbability::LOW:
			return "LOW";
		case HarmProbability::MEDIUM:
			return "MEDIUM";
		case HarmProbability::HIGH:
			return "HIGH";
		default:
			return "UNKNOWN_HARM_PROBABILITY";
		}
	}
	HarmProbability stringToHarmProbability(const QString& str)
	{
		if (str == "HARM_PROBABILITY_UNSPECIFIED") {
			return HarmProbability::HARM_PROBABILITY_UNSPECIFIED;
		}
		else if (str == "NEGLIGIBLE") {
			return HarmProbability::NEGLIGIBLE;
		}
		else if (str == "LOW") {
			return HarmProbability::LOW;
		}
		else if (str == "MEDIUM") {
			return HarmProbability::MEDIUM;
		}
		else if (str == "HIGH") {
			return HarmProbability::HIGH;
		}
		else {
			return HarmProbability::HARM_PROBABILITY_UNSPECIFIED; // or throw an exception
		}
	}

	QString toString(HarmBlockThreshold threshold)
	{
		switch (threshold) {
		case HarmBlockThreshold::HARM_BLOCK_THRESHOLD_UNSPECIFIED:
			return "HARM_BLOCK_THRESHOLD_UNSPECIFIED";
		case HarmBlockThreshold::BLOCK_LOW_AND_ABOVE:
			return "BLOCK_LOW_AND_ABOVE";
		case HarmBlockThreshold::BLOCK_MEDIUM_AND_ABOVE:
			return "BLOCK_MEDIUM_AND_ABOVE";
		case HarmBlockThreshold::BLOCK_ONLY_HIGH:
			return "BLOCK_ONLY_HIGH";
		case HarmBlockThreshold::BLOCK_NONE:
			return "BLOCK_NONE";
		case HarmBlockThreshold::OFF:
			return "OFF";
		default:
			return "UNKNOWN_HARM_BLOCK_THRESHOLD";
		}
	}
	HarmBlockThreshold stringToHarmBlockThreshold(const QString& str)
	{
		if (str == "HARM_BLOCK_THRESHOLD_UNSPECIFIED") {
			return HarmBlockThreshold::HARM_BLOCK_THRESHOLD_UNSPECIFIED;
		}
		else if (str == "BLOCK_LOW_AND_ABOVE") {
			return HarmBlockThreshold::BLOCK_LOW_AND_ABOVE;
		}
		else if (str == "BLOCK_MEDIUM_AND_ABOVE") {
			return HarmBlockThreshold::BLOCK_MEDIUM_AND_ABOVE;
		}
		else if (str == "BLOCK_ONLY_HIGH") {
			return HarmBlockThreshold::BLOCK_ONLY_HIGH;
		}
		else if (str == "BLOCK_NONE") {
			return HarmBlockThreshold::BLOCK_NONE;
		}
		else if (str == "OFF") {
			return HarmBlockThreshold::OFF;
		}
		else {
			return HarmBlockThreshold::HARM_BLOCK_THRESHOLD_UNSPECIFIED; // or throw an exception
		}
	}

	QString toString(BlockReason reason)
	{
		switch (reason) {
		case BlockReason::BLOCK_REASON_UNSPECIFIED:
			return "BLOCK_REASON_UNSPECIFIED";
		case BlockReason::SAFETY:
			return "SAFETY";
		case BlockReason::OTHER:
			return "OTHER";
		case BlockReason::BLOCKLIST:
			return "BLOCKLIST";
		case BlockReason::PROHIBITED_CONTENT:
			return "PROHIBITED_CONTENT";
		default:
			return "UNKNOWN_BLOCK_REASON";
		}
	}
	BlockReason stringToBlockReason(const QString& str)
	{
		if (str == "BLOCK_REASON_UNSPECIFIED") {
			return BlockReason::BLOCK_REASON_UNSPECIFIED;
		}
		else if (str == "SAFETY") {
			return BlockReason::SAFETY;
		}
		else if (str == "OTHER") {
			return BlockReason::OTHER;
		}
		else if (str == "BLOCKLIST") {
			return BlockReason::BLOCKLIST;
		}
		else if (str == "PROHIBITED_CONTENT") {
			return BlockReason::PROHIBITED_CONTENT;
		}
		else {
			return BlockReason::BLOCK_REASON_UNSPECIFIED; // or throw an exception
		}
	}

	QString toString(FinishReason reason)
	{
		switch (reason) {
		case FinishReason::FINISH_REASON_UNSPECIFIED:
			return "FINISH_REASON_UNSPECIFIED";
		case FinishReason::STOP:
			return "STOP";
		case FinishReason::MAX_TOKENS:
			return "MAX_TOKENS";
		case FinishReason::SAFETY:
			return "SAFETY";
		case FinishReason::RECITATION:
			return "RECITATION";
		case FinishReason::LANGUAGE:
			return "LANGUAGE";
		case FinishReason::OTHER:
			return "OTHER";
		case FinishReason::BLOCKLIST:
			return "BLOCKLIST";
		case FinishReason::PROHIBITED_CONTENT:
			return "PROHIBITED_CONTENT";
		case FinishReason::SPII:
			return "SPII";
		case FinishReason::MALFORMED_FUNCTION_CALL:
			return "MALFORMED_FUNCTION_CALL";
		default:
			return "UNKNOWN_FINISH_REASON";
		}
	}

	FinishReason stringToFinishReason(const QString& str)
	{
		if (str == "FINISH_REASON_UNSPECIFIED") {
			return FinishReason::FINISH_REASON_UNSPECIFIED;
		}
		else if (str == "STOP") {
			return FinishReason::STOP;
		}
		else if (str == "MAX_TOKENS") {
			return FinishReason::MAX_TOKENS;
		}
		else if (str == "SAFETY") {
			return FinishReason::SAFETY;
		}
		else if (str == "RECITATION") {
			return FinishReason::RECITATION;
		}
		else if (str == "LANGUAGE") {
			return FinishReason::LANGUAGE;
		}
		else if (str == "OTHER") {
			return FinishReason::OTHER;
		}
		else if (str == "BLOCKLIST") {
			return FinishReason::BLOCKLIST;
		}
		else if (str == "PROHIBITED_CONTENT") {
			return FinishReason::PROHIBITED_CONTENT;
		}
		else if (str == "SPII") {
			return FinishReason::SPII;
		}
		else if (str == "MALFORMED_FUNCTION_CALL") {
			return FinishReason::MALFORMED_FUNCTION_CALL;
		}
		else {
			return FinishReason::FINISH_REASON_UNSPECIFIED; // or throw an exception
		}
	}

	QString toString(State state)
	{
		switch (state) {
		case State::STATE_UNSPECIFIED:
			return "STATE_UNSPECIFIED";
		case State::PROCESSING:
			return "PROCESSING";
		case State::ACTIVE:
			return "ACTIVE";
		case State::FAILED:
			return "FAILED";
		default:
			return "UNKNOWN_STATE";
		}
	}

	State stringToState(const QString& str)
	{
		if (str == "STATE_UNSPECIFIED") {
			return State::STATE_UNSPECIFIED;
		}
		else if (str == "PROCESSING") {
			return State::PROCESSING;
		}
		else if (str == "ACTIVE") {
			return State::ACTIVE;
		}
		else if (str == "FAILED") {
			return State::FAILED;
		}
		else {
			return State::STATE_UNSPECIFIED; // or throw an exception
		}
	}

}

namespace m0st4fa::gemini {

	QJsonObject m0st4fa::gemini::Blob::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"mimeType", this->mimeType},
			std::pair<QString, QJsonValue>{"data", QVariant{ this->data }.toJsonValue() }
		};
	}

	Blob m0st4fa::gemini::Blob::fromJson(const QJsonObject& json)
	{

		return Blob{
			.mimeType = json.value("mimeType").toString(),
			.data = qvariant_cast<QByteArray>(json.value("data"))
		};
	}

	QJsonObject FileData::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"mimeType", this->mimeType},
			std::pair<QString, QJsonValue>{"fileUri", this->fileUri}
		};
	}

	FileData FileData::fromJson(const QJsonObject& json)
	{
		return FileData{
			.mimeType = json.value("mimeType").toString(),
			.fileUri = json.value("fileUri").toString()
		};
	}

	QJsonObject Part::toJson() const
	{
		QJsonObject obj;

		switch (this->part.index()) {

		case 0:
			obj.insert("text", std::get<0>(part));
			break;

		case 1:
			obj.insert("inlineData", std::get<1>(part).toJson());
			break;

		case 2:
			obj.insert("fileData", std::get<2>(part).toJson());
			break;

		default:
			qInfo() << __FUNCTION__": Unrecognized part type.";
			break;
		}

		return obj;
	}

	Part Part::fromJson(const QJsonObject& json)
	{
		Part part;

		if (json.contains("text"))
			part.part = json.value("text").toString();
		else if (json.contains("inlineData"))
			part.part = Blob::fromJson(json.value("inlineData").toObject());
		else if (json.contains("fileData"))
			part.part = FileData::fromJson(json.value("fileData").toObject());
		else
			qInfo() << __FUNCTION__": Unrecognized part type.";

		return part;
	}

	void Content::addText(const QString& text)
	{
		this->parts.append(
			Part{
				.part = text
			}
		);
	}

	void Content::addFileData(const QString& mimeType, const QString& fileUri)
	{
		this->parts.append(
			Part{
				.part = FileData{
					.mimeType = mimeType,
					.fileUri = fileUri
				}
			}
		);
	}

	QJsonObject Content::toJson() const
	{
		QJsonArray pts;

		for (const Part& part : parts)
			pts.append(part.toJson());

		QJsonObject obj;
		obj.insert("parts", pts);

		if (!role.isEmpty())
			obj.insert("role", this->role);

		return obj;
	}

	Content Content::fromJson(const QJsonObject& json)
	{
		Content content;
		QVector<Part> parts;

		for (const QJsonValue& val : json.value("parts").toArray()) {
			QJsonObject obj = val.toObject();
			parts.append(Part::fromJson(obj));
		}

		content.parts = parts;

		if (json.contains("role"))
			content.role = json.value("role").toString();
			
		return content;
	}

	Content m0st4fa::gemini::Content::textPrompt(const QString& prompt)
	{
		return Content{
				.parts = QVector<Part> {
					Part {
						.part = prompt
					}
				}
		};
	}

	QJsonObject RequestBody::toJson() const
	{
		QJsonArray cnts;

		for (const Content& content : this->contents)
			cnts.append(content.toJson());

		return QJsonObject{
			std::pair<QString, QJsonValue>{"contents", cnts}
		};
	}

	RequestBody RequestBody::fromJson(const QJsonObject& json)
	{
		return RequestBody();
	}

	RequestBody RequestBody::textPrompt(const QString& prompt)
	{
		return RequestBody{
			.contents = QVector<Content> { Content::textPrompt(prompt) }
		};
	}

	QJsonObject UsageMetadata::toJson() const
	{
		QJsonObject obj;
		obj["promptTokenCount"] = promptTokenCount;
		obj["cachedContentTokenCount"] = cachedContentTokenCount;
		obj["candidatesTokenCount"] = candidatesTokenCount;
		obj["totalTokenCount"] = totalTokenCount;
		return obj;
	}
	UsageMetadata UsageMetadata::fromJson(const QJsonObject& json)
	{
		UsageMetadata metadata;
		metadata.promptTokenCount = json["promptTokenCount"].toInt();
		metadata.cachedContentTokenCount = json["cachedContentTokenCount"].toInt();
		metadata.candidatesTokenCount = json["candidatesTokenCount"].toInt();
		metadata.totalTokenCount = json["totalTokenCount"].toInt();
		return metadata;
	}

	QJsonObject SafetyRating::toJson() const
	{
		QJsonObject obj;
		obj["category"] = enums::toString(this->category);
		obj["probability"] = enums::toString(this->probability);
		obj["blocked"] = this->blocked;
		
		return obj;
	}
	SafetyRating SafetyRating::fromJson(const QJsonObject& json)
	{
		return SafetyRating{
			.category = enums::stringToHarmCategory(json.value("category").toString()),
			.probability = enums::stringToHarmProbability(json.value("probability").toString()),
			.blocked = json.value("blocked").toBool()
		};
	}

	QJsonObject m0st4fa::gemini::SafetySetting::toJson() const
	{
		QJsonObject obj;

		obj["category"] = enums::toString(this->category);
		obj["threshold"] = enums::toString(this->threshold);

		return obj;
	}
	SafetySetting SafetySetting::fromJson(const QJsonObject json)
	{
		return SafetySetting{
			.category = enums::stringToHarmCategory(json.value("category").toString()),
			.threshold = enums::stringToHarmBlockThreshold(json.value("threshold").toString()),
		};
	}

	QJsonObject m0st4fa::gemini::PromptFeedback::toJson() const
	{
		QJsonObject obj;

		if (this->blockReason != enums::BlockReason::BLOCK_REASON_UNSPECIFIED)
			obj["blockReason"] = enums::toString(blockReason);

		if (!this->safetyRatings.isEmpty()) {
			QJsonArray safetyRatingsArray;
			for (const auto& rating : safetyRatings)
				safetyRatingsArray.append(rating.toJson());

			obj["safetyRatings"] = safetyRatingsArray;
		}

		return obj;
	}
	PromptFeedback PromptFeedback::fromJson(const QJsonObject& json)
	{
		PromptFeedback feedback;

		if (json.contains("blockReason"))
			feedback.blockReason = enums::stringToBlockReason(json["blockReason"].toString());

		if (json.contains("safetyRatings")) {
			QJsonArray safetyRatingsArray = json["safetyRatings"].toArray();
			for (const QJsonValue& value : safetyRatingsArray) {
				feedback.safetyRatings.push_back(SafetyRating::fromJson(value.toObject()));
			}
		}

		return feedback;
	}

	QJsonObject m0st4fa::gemini::Candidate::toJson() const
	{
		QJsonObject obj;
		obj["content"] = content.toJson();

		if(this->finishReason != enums::FinishReason::FINISH_REASON_UNSPECIFIED)
			obj["finishReason"] = enums::toString(this->finishReason);

		QJsonArray safetyRatingsArray;
		for (const auto& rating : safetyRatings) {
			safetyRatingsArray.append(rating.toJson());
		}
		obj["safetyRatings"] = safetyRatingsArray;

		obj["citationMetadata"] = citationMetadata.toJson();

		obj["tokenCount"] = tokenCount;

		QJsonArray groundingAttributionsArray;
		for (const auto& attribution : groundingAttributions) {
			groundingAttributionsArray.append(attribution.toJson());
		}
		obj["groundingAttributions"] = groundingAttributionsArray;

		obj["groundingMetadata"] = groundingMetadata.toJson();

		obj["avgLogprob"] = avgLogprob;

		obj["logprobsResult"] = logprobsResult.toJson();

		obj["index"] = index;

		return obj;
	}

	Candidate Candidate::fromJson(const QJsonObject& json)
	{
		Candidate candidate;
		candidate.content = Content::fromJson(json["content"].toObject());

		if (json.contains("finishReason"))
			candidate.finishReason = enums::stringToFinishReason(json["finishReason"].toString());

		if (json.contains("safetyRatings")) {
			QJsonArray safetyRatingsArray = json["safetyRatings"].toArray();
			for (const QJsonValue& value : safetyRatingsArray) {
				candidate.safetyRatings.push_back(SafetyRating::fromJson(value.toObject()));
			}
		}

		if (json.contains("citationMetadata")) {
			candidate.citationMetadata = CitationMetadata::fromJson(json["citationMetadata"].toObject());
		}

		candidate.tokenCount = json["tokenCount"].toInt();

		QJsonArray groundingAttributionsArray = json["groundingAttributions"].toArray();
		for (const QJsonValue& value : groundingAttributionsArray) {
			candidate.groundingAttributions.push_back(GroundingAttribution::fromJson(value.toObject()));
		}

		if (json.contains("groundingMetadata")) {
			candidate.groundingMetadata = GroundingMetadata::fromJson(json["groundingMetadata"].toObject());
		}

		candidate.avgLogprob = json["avgLogprobs"].toDouble();

		candidate.logprobsResult = LogprobsResult::fromJson(json["logprobsResult"].toObject());

		candidate.index = json["index"].toInt();

		return candidate;
	}

}