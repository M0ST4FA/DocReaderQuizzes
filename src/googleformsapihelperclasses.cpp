#include "googleformsapihelperclasses.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace m0st4fa::forms {

	QJsonObject m0st4fa::forms::update_form::UpdateRequestBody::toJson() const
	{
		QJsonValue ifir = QJsonValue{ this->includeFormInResponse };
		QJsonArray reqs = QJsonArray{};

		for (const UpdateRequest& req : this->requests)
			reqs.append(req.toJson());

		bool includeWriteControl = true;
		if (writeControl.revisionId.isEmpty())
			includeWriteControl = false;

		if (includeWriteControl)
			return QJsonObject{
				{
					std::pair<QString, QJsonValue>{"includeFormInResponse", ifir},
					std::pair<QString, QJsonValue>{"requests", reqs},
					std::pair<QString, QJsonValue>{"writeControl", this->writeControl.toJson()}
				}

		};
		else
			return QJsonObject{
				{
					std::pair<QString, QJsonValue>{"includeFormInResponse", ifir},
					std::pair<QString, QJsonValue>{"requests", reqs}
				}

		};
	}

	update_form::UpdateRequestBody update_form::UpdateRequestBody::fromJson(const QJsonObject& json)
	{
		UpdateRequestBody body = UpdateRequestBody{
			.includeFormInResponse = json.value("includeFormInResponse").toBool(),
			.writeControl = WriteControl::fromJson(json.value("writeControl").toObject())
		};

		QVector<UpdateRequest> requests;

		for (const QJsonValue& val : json.value("requests").toArray())
			requests.append(UpdateRequest::fromJson(val.toObject()));

		body.requests = requests;

		return body;
	}

	QJsonObject m0st4fa::forms::update_form::UpdateRequest::toJson() const
	{

		switch (request.index())
		{
		case m0st4fa::forms::update_form::UpdateRequest::UPDATE_FORM_INFO:
			return QJsonObject{
				std::pair<QString, QJsonValue> {"updateFormInfo", std::get<UpdateFormInfoRequest>(this->request).toJson()}
			};
			break;
		case m0st4fa::forms::update_form::UpdateRequest::UPDATE_SETTINGS:
			return QJsonObject{
				std::pair<QString, QJsonValue> {"updateSettings", std::get<UpdateSettingsRequest>(this->request).toJson()}
			};
			break;
		case m0st4fa::forms::update_form::UpdateRequest::CREATE_ITEM:
			return QJsonObject{
				std::pair<QString, QJsonValue> {"createItem", std::get<CreateItemRequest>(this->request).toJson()}
			};
			break;
		case m0st4fa::forms::update_form::UpdateRequest::MOVE_ITEM:
			return QJsonObject{
				std::pair<QString, QJsonValue> {"moveItem", std::get<MoveItemRequest>(this->request).toJson()}
			};
			break;
		case m0st4fa::forms::update_form::UpdateRequest::DELETE_ITEM:
			return QJsonObject{
				std::pair<QString, QJsonValue> {"deleteItem", std::get<DeleteItemRequest>(this->request).toJson()}
			};
			break;
		case m0st4fa::forms::update_form::UpdateRequest::UPDATE_ITEM:
			return QJsonObject{
				std::pair<QString, QJsonValue> {"updateItem", std::get<UpdateItemRequest>(this->request).toJson()}
			};
			break;
		case m0st4fa::forms::update_form::UpdateRequest::NONE:
			return QJsonObject{};
			break;
		default:
			break;
		}

		return QJsonObject{

		};
	}

	update_form::UpdateRequest update_form::UpdateRequest::fromJson(const QJsonObject& json)
	{
		UpdateRequest req = UpdateRequest{};

		if (json.contains("updateFormInfo"))
			req.request = UpdateFormInfoRequest::fromJson(json.value("updateFormInfo").toObject());
		else if (json.contains("updateSettings"))
			req.request = UpdateSettingsRequest::fromJson(json.value("updateSettings").toObject());
		else if (json.contains("createItem"))
			req.request = CreateItemRequest::fromJson(json.value("createItem").toObject());
		else if (json.contains("moveItem"))
			req.request = MoveItemRequest::fromJson(json.value("moveItem").toObject());
		else if (json.contains("deleteItem"))
			req.request = DeleteItemRequest::fromJson(json.value("deleteItem").toObject());
		else if (json.contains("updateItem"))
			req.request = UpdateItemRequest::fromJson(json.value("updateItem").toObject());

		return req;
	}

	QJsonObject m0st4fa::forms::update_form::UpdateFormInfoRequest::toJson() const
	{
		return QJsonObject{
			{
				std::pair<QString, QJsonValue>{"info", this->info.toJson()},
				std::pair<QString, QJsonValue>{"updateMask", this->updateMask}
			}
		};
	}

	update_form::UpdateFormInfoRequest update_form::UpdateFormInfoRequest::fromJson(const QJsonObject& json)
	{
		return UpdateFormInfoRequest{
			.info = Info::fromJson(json.value("info").toObject()),
			.updateMask = json.value("updateMask").toString()
		};
	}

	QJsonObject m0st4fa::forms::Info::toJson() const
	{
		return QJsonObject{
			{
				std::pair<QString, QJsonValue>{"title", this->title},
				std::pair<QString, QJsonValue>{"documentTitle", this->documentTitle},
				std::pair<QString, QJsonValue>{"description", this->description}
			}
		};
	}

	m0st4fa::forms::Info m0st4fa::forms::Info::fromJson(const QJsonObject& json)
	{
		QString title = json.value("title").toString();
		QString documentTitle = json.value("documentTitle").toString();
		QString description = json.value("description").toString();

		return Info{
			.title = title,
			.documentTitle = documentTitle,
			.description = description
		};
	}

	QJsonObject m0st4fa::forms::Settings::toJson() const
	{
		QJsonObject quizSetts{
			{std::pair<QString, QJsonValue>{"isQuiz", this->quizSettings.isQuiz},}
		};

		return QJsonObject{
			{
				std::pair<QString, QJsonValue>{"quizSettings", quizSetts}
			}
		};
	}

	m0st4fa::forms::Settings m0st4fa::forms::Settings::fromJson(const QJsonObject& json)
	{
		bool isQuiz = json.value("quizSettings").toObject().value("isQuiz").toBool();

		Settings settings{};
		settings.quizSettings.isQuiz = isQuiz;

		return settings;
	}

	QJsonObject m0st4fa::forms::Form::toJson() const
	{
		QJsonArray its;

		for (const Item& item : this->items)
			its.append(item.toJson());

		return QJsonObject{

			{
				std::pair<QString, QJsonValue>{"formId", this->formId},
				std::pair<QString, QJsonValue>{"info", this->info.toJson()},
				std::pair<QString, QJsonValue>{"settings", this->settings.toJson()},
				std::pair<QString, QJsonValue>{"items", its},
				std::pair<QString, QJsonValue>{"revisionId", this->revisionId},
				std::pair<QString, QJsonValue>{"responderId", this->responderUri},
				std::pair<QString, QJsonValue>{"linkedSheetId", this->linkedSheetId}

			}

		};
	}

	Form m0st4fa::forms::Form::fromJson(const QJsonObject& json)
	{
		QString formId = json.value("formId").toString();
		Info info = Info::fromJson(json.value("info").toObject());
		Settings settings = Settings::fromJson(json.value("settings").toObject());

		QVector<Item> items;

		for (const QJsonValue& val : json.value("items").toArray())
			items.append(Item::fromJson(val.toObject()));

		QString revisionId = json.value("revisionId").toString();
		QString responderId = json.value("responderId").toString();
		QString linkedSheetId = json.value("linkedSheetId").toString();

		return Form{
				.formId = formId,
				.info = info,
				.settings = settings,
				.items = items,
				.revisionId = revisionId,
				.responderUri = responderId,
				.linkedSheetId = linkedSheetId
		};
	}

	QJsonObject m0st4fa::forms::update_form::UpdateSettingsRequest::toJson() const
	{
		return QJsonObject{
			{
				std::pair<QString, QJsonValue>{"settings", this->settings.toJson()},
				std::pair<QString, QJsonValue>{"updateMask", this->updateMask}
			}
		};
	}

	update_form::UpdateSettingsRequest update_form::UpdateSettingsRequest::fromJson(const QJsonObject& json)
	{
		return UpdateSettingsRequest{
			.settings = Settings::fromJson(json.value("settings").toObject()),
			.updateMask = json.value("updateMask").toString()
		};
	}

	QJsonObject m0st4fa::forms::update_form::Location::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>("index", QJsonValue{(int)this->index})
		};
	}

	QJsonObject m0st4fa::forms::update_form::CreateItemRequest::toJson() const
	{
		return QJsonObject{
			{
				std::pair<QString, QJsonValue>{"item", this->item.toJson()},
				std::pair<QString, QJsonValue>{"location", this->location.toJson()}
			}

		};
	}

	update_form::CreateItemRequest update_form::CreateItemRequest::fromJson(const QJsonObject& json)
	{
		return CreateItemRequest{
			.item = Item::fromJson(json.value("item").toObject()),
			.location = Location::fromJson(json.value("location").toObject())
		};
	}

	QJsonObject m0st4fa::forms::update_form::MoveItemRequest::toJson() const
	{
		return QJsonObject{
			{
				std::pair<QString, QJsonValue>{"originalLocation", this->originalLocation.toJson()},
				std::pair<QString, QJsonValue>{"newLocation", this->newLocation.toJson()}
			}
		};
	}

	update_form::MoveItemRequest update_form::MoveItemRequest::fromJson(const QJsonObject& json)
	{
		return MoveItemRequest{
			.originalLocation = Location::fromJson(json.value("originalLocation").toObject()),
			.newLocation = Location::fromJson(json.value("newLocation").toObject())
		};
	}

	QJsonObject m0st4fa::forms::update_form::DeleteItemRequest::toJson() const
	{
		return QJsonObject{
			{
				std::pair<QString, QJsonValue>{"location", this->location.toJson()},
			}
		};
	}

	update_form::DeleteItemRequest update_form::DeleteItemRequest::fromJson(const QJsonObject& json)
	{
		return DeleteItemRequest{
			.location = Location::fromJson(json.value("location").toObject())
		};
	}

	QJsonObject m0st4fa::forms::update_form::UpdateItemRequest::toJson() const
	{
		return QJsonObject{
		{
			std::pair<QString, QJsonValue>{"item", this->item.toJson()},
			std::pair<QString, QJsonValue>{"location", this->location.toJson()},
			std::pair<QString, QJsonValue>{"updateMask", this->updateMask}
		}

		};
	}

	update_form::UpdateItemRequest update_form::UpdateItemRequest::fromJson(const QJsonObject& json)
	{
		return UpdateItemRequest{
			.item = Item::fromJson(json.value("item").toObject()),
			.location = Location::fromJson(json.value("location").toObject()),
			.updateMask = json.value("updateMask").toString()
		};
	}

	QJsonObject m0st4fa::forms::update_form::WriteControl::toJson() const
	{

		if (isRequired)
			return QJsonObject{
				{
					std::pair<QString, QJsonValue>{"requiredRevisionId", this->revisionId},
				}
		};
		else
			return QJsonObject{
				{
					std::pair<QString, QJsonValue>{"targetRevisionId", this->revisionId},
				}
		};
	}

	update_form::WriteControl update_form::WriteControl::fromJson(const QJsonObject& json)
	{
		if (json.contains("requiredRevisionId"))
			return WriteControl{ .isRequired = true, .revisionId = json.value("requiredRevisionId").toString() };
		else
			return WriteControl{ .isRequired = false, .revisionId = json.value("targetRevisionId").toString() };

	}

	QJsonObject m0st4fa::forms::update_form::UpdateResponseBody::toJson() const
	{
		QJsonArray reps = QJsonArray{};

		for (const UpdateResponse& rep : this->replies)
			reps.append(rep.toJson());

		bool includeWriteControl = true;
		if (writeControl.revisionId.isEmpty())
			includeWriteControl = false;

		if (includeWriteControl)
			return QJsonObject{
				{
					std::pair<QString, QJsonValue>{"replies", reps},
					std::pair<QString, QJsonValue>{"writeControl", this->writeControl.toJson()}
				}

		};
		else
			return QJsonObject{
				{
					std::pair<QString, QJsonValue>{"replies", reps}
				}

		};

	}

	update_form::UpdateResponseBody m0st4fa::forms::update_form::UpdateResponseBody::fromJson(const QJsonObject& json)
	{
		std::shared_ptr<Form> form = std::shared_ptr<Form>(new Form{ Form::fromJson(json.value("form").toObject()) });

		QVector<UpdateResponse> replies;

		for (const QJsonValue& reply : json.value("replies").toArray())
			replies.append(UpdateResponse::fromJson(reply.toObject()));

		bool writeControlExists = json.contains("writeControl");

		if (!writeControlExists)
			return UpdateResponseBody{
			.form = form,
			.replies = replies
		};

		WriteControl writeControl = WriteControl::fromJson(json.value("writeControl").toObject());

		return UpdateResponseBody{
			.form = form,
			.replies = replies,
			.writeControl = writeControl
		};
	}

	QJsonObject m0st4fa::forms::update_form::UpdateResponse::toJson() const
	{
		QJsonValue iid = this->itemId;
		QJsonArray qid;

		for (const QString& id : this->questionId)
			qid.append(id);

		return QJsonObject{
			std::pair<QString, QJsonValue>{"itemId", iid},
			std::pair<QString, QJsonValue>{"questionId", qid},
		};
	}

	update_form::UpdateResponse m0st4fa::forms::update_form::UpdateResponse::fromJson(const QJsonObject& json)
	{
		QString itemId = json.value("itemId").toString();
		QVector<QString> questionId;

		for (const QJsonValue& val : json.value("questionId").toArray())
			questionId.append(val.toString());

		return UpdateResponse{
			.itemId = itemId,
			.questionId = questionId
		};
	}

	QJsonObject MediaProperties::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"alignment", alignmentToString(this->alignment)},
			std::pair<QString, QJsonValue>{"width", this->width}
		};
	}

	MediaProperties MediaProperties::fromJson(const QJsonObject& json)
	{
		return MediaProperties{
			.alignment = stringToAlignment(json.value("alignment").toString()),
			.width = json.value("width").toInt()
		};
	}

	QJsonObject Image::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"contentUri", this->contentUri},
			std::pair<QString, QJsonValue>{"altText", this->altText},
			std::pair<QString, QJsonValue>{"properties", this->properties.toJson()},
			std::pair<QString, QJsonValue>{"sourceUri", this->sourceUri},
		};
	}

	Image Image::fromJson(const QJsonObject& json)
	{
		return Image{
			.contentUri = json.value("contentUri").toString(),
			.altText = json.value("altText").toString(),
			.properties = MediaProperties::fromJson(json.value("properties").toObject()),
			.sourceUri = json.value("sourceUri").toString()
		};
	}

	QJsonObject ImageItem::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"image", this->image.toJson()},
		};
	}

	ImageItem ImageItem::fromJson(const QJsonObject& json)
	{
		return ImageItem {
			.image = Image::fromJson(json.value("image").toObject())
		};
	}

	QJsonObject Video::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"youtubeUri", this->youtubeUri},
			std::pair<QString, QJsonValue>{"properties", this->properties.toJson()}
		};
	}

	Video Video::fromJson(const QJsonObject& json)
	{
		return Video{
			.youtubeUri = json.value("youtubeUri").toString(),
			.properties = MediaProperties::fromJson(json.value("properties").toObject())
		};
	}

	QJsonObject VideoItem::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"video", this->video.toJson()},
			std::pair<QString, QJsonValue>{"caption", this->caption}
		};
	}

	VideoItem VideoItem::fromJson(const QJsonObject& json)
	{
		return VideoItem{
			.video = Video::fromJson(json.value("video").toObject()),
			.caption = json.value("caption").toString()
		};
	}

	QJsonObject RowQuestion::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"title", this->title}
		};
	}

	RowQuestion RowQuestion::fromJson(const QJsonObject& json)
	{
		return RowQuestion{
			.title = json.value("title").toString()
		};
	}

	QJsonObject TimeQuestion::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"duration", this->duration}
		};
	}

	TimeQuestion TimeQuestion::fromJson(const QJsonObject& json)
	{
		return TimeQuestion{
			.duration = json.value("duration").toBool()
		};
	}

	QJsonObject DateQuestion::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"includeTime", this->includeTime},
			std::pair<QString, QJsonValue>{"includeYear", this->includeYear}
		};
	}

	DateQuestion DateQuestion::fromJson(const QJsonObject& json)
	{
		return DateQuestion{
			.includeTime = json.value("includeTime").toBool(),
			.includeYear = json.value("includeYear").toBool()
		};
	}

	QJsonObject ScaleQuestion::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"low", this->low},
			std::pair<QString, QJsonValue>{"high", this->high},
			std::pair<QString, QJsonValue>{"lowLabel", this->lowLabel},
			std::pair<QString, QJsonValue>{"highLabel", this->highLabel}
		};
	}

	ScaleQuestion ScaleQuestion::fromJson(const QJsonObject& json)
	{
		return ScaleQuestion{
			.low = json.value("low").toInt(),
			.high = json.value("high").toInt(),
			.lowLabel = json.value("lowLabel").toString(),
			.highLabel = json.value("highLabel").toString()
		};
	}

	QJsonObject TextQuestion::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"paragraph", this->paragraph}
		};
	}

	TextQuestion TextQuestion::fromJson(const QJsonObject& json)
	{
		return TextQuestion{
			.paragraph = json.value("paragraph").toBool()
		};
	}

	QJsonObject Option::toJson() const
	{
		QJsonObject obj = QJsonObject{
			std::pair<QString, QJsonValue>{"value", this->value},
			std::pair<QString, QJsonValue>{"isOther", this->isOther}
		};

		bool includeImage = !this->image.sourceUri.isEmpty();

		if (includeImage)
			obj.insert("image", this->image.toJson());

		if (goTo.index() == 0)
			if (std::get<0>(goTo) == GoToAction::GO_TO_ACTION_UNSPECIFIED);
			else
				obj.insert("goToAction", goToActionToString(std::get<0>(goTo)));
		else
			if (!std::get<1>(goTo).isEmpty())
				obj.insert("goToSectionId", std::get<1>(goTo));

		return obj;
	}

	Option Option::fromJson(const QJsonObject& json)
	{
		Option option = Option{
			.value = json.value("value").toString(),
			.image = Image::fromJson(json.value("image").toObject()),
			.isOther = json.value("isOther").toBool()
		};

		if (json.contains("goToAction"))
			option.goTo = stringToGoToAction(json.value("goToAction").toString());
		else
			option.goTo = json.value("goToSectionId").toString();

		return option;
	}

	QJsonObject ChoiceQuestion::toJson() const
	{
		QJsonArray opts;

		for (const Option& opt : this->options)
			opts.append(opt.toJson());

		return QJsonObject{
			std::pair<QString, QJsonValue>{"type", choiceTypeToString(this->type)},
			std::pair<QString, QJsonValue>{"options", opts},
			std::pair<QString, QJsonValue>{"shuffle", this->shuffle},
		};
	}

	ChoiceQuestion ChoiceQuestion::fromJson(const QJsonObject& json)
	{
		QVector<Option> options;

		for (const QJsonValue& val : json.value("options").toArray())
			options.append(Option::fromJson(val.toObject()));

		return ChoiceQuestion{
			.type = stringToChoiceType(json.value("type").toString()),
			.options = options,
			.shuffle = json.value("shuffle").toBool()

		};
	}

	QJsonObject CorrectAnswer::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"value", this->value}
		};
	}

	CorrectAnswer CorrectAnswer::fromJson(const QJsonObject& json)
	{
		return CorrectAnswer{
			.value = json.value("value").toString()
		};
	}

	QJsonObject CorrectAnswers::toJson() const
	{
		QJsonArray anss;

		for (const CorrectAnswer& ans : this->answers)
			anss.append(ans.toJson());

		return QJsonObject{
			std::pair<QString, QJsonValue>{"answers", anss}
		};
	}

	CorrectAnswers CorrectAnswers::fromJson(const QJsonObject& json)
	{
		QVector<CorrectAnswer> answers;

		for (const QJsonValue& val : json.value("answers").toArray())
			answers.append(CorrectAnswer::fromJson(val.toObject()));

		return CorrectAnswers{
			.answers = answers
		};
	}

	QJsonObject TextLink::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"uri", this->uri},
			std::pair<QString, QJsonValue>{"displayText", this->displayText}
		};
	}

	TextLink TextLink::fromJson(const QJsonObject& json)
	{
		return TextLink{
			.uri = json.value("uri").toString(),
			.displayText = json.value("displayText").toString()
		};
	}

	QJsonObject VideoLink::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"displayText", this->displayText},
			std::pair<QString, QJsonValue>{"youtubeUr", this->youtubeUri},
		};
	}

	VideoLink VideoLink::fromJson(const QJsonObject& json)
	{
		return VideoLink{
			.displayText = json.value("displayText").toString(),
			.youtubeUri = json.value("youtubeUri").toString(),
		};
	}

	QJsonObject ExtraMaterial::toJson() const
	{

		if (link.index() == 0)
			return QJsonObject{
				std::pair<QString, QJsonValue>{"link", std::get<0>(this->link).toJson()}
		};
		else
			return QJsonObject{
				std::pair<QString, QJsonValue>{"video", std::get<1>(this->link).toJson()}
			};

	}

	ExtraMaterial ExtraMaterial::fromJson(const QJsonObject& json)
	{

		if (json.contains("link"))
			return ExtraMaterial{
			.link = TextLink::fromJson(json.value("link").toObject())
		};
		else
			return ExtraMaterial{
			.link = VideoLink::fromJson(json.value("video").toObject())
		};

	}

	QJsonObject Feedback::toJson() const
	{
		QJsonArray mats;

		for (const ExtraMaterial& mat : this->material)
			mats.append(mat.toJson());

		return QJsonObject{
			std::pair<QString, QJsonValue>{"text", this->text},
			std::pair<QString, QJsonValue>{"material", mats}
		};
	}

	Feedback Feedback::fromJson(const QJsonObject& json)
	{
		QVector<ExtraMaterial> material;

		for (const QJsonValue& val : json.value("material").toArray())
			material.append(ExtraMaterial::fromJson(val.toObject()));

		return Feedback{
			.text = json.value("text").toString(),
			.material = material
		};
	}

	QJsonObject Grading::toJson() const
	{
		QJsonObject obj = QJsonObject{
			std::pair<QString, QJsonValue>{"pointValue", this->pointValue},
			std::pair<QString, QJsonValue>{"correctAnswers", this->correctAnswers.toJson()}
		};

		bool whenRightIncluded = !this->whenRight.text.isEmpty();
		bool whenWrongIncluded = !this->whenWrong.text.isEmpty();
		bool generalFeedbackIncluded = !this->generalFeedback.text.isEmpty();

		if (whenRightIncluded)
			obj.insert("whenRight", this->whenRight.toJson());

		if (whenWrongIncluded)
			obj.insert("whenWrong", this->whenWrong.toJson());

		if (generalFeedbackIncluded)
			obj.insert("generalFeedback", this->generalFeedback.toJson());

		return obj;
	}

	Grading Grading::fromJson(const QJsonObject& json)
	{
		return Grading{
			.pointValue = json.value("pointValue").toInt(),
			.correctAnswers = CorrectAnswers::fromJson(json.value("correctAnswers").toObject()),
			.whenRight = json.value("whenRight").toString(),
			.whenWrong = json.value("whenWrong").toString(),
			.generalFeedback = json.value("generalFeedback").toString()
		};
	}

	QJsonObject Question::toJson() const
	{
		QJsonObject obj = QJsonObject{
			std::pair<QString, QJsonValue>{"questionId", this->questionId},
			std::pair<QString, QJsonValue>{"required", this->required},
			std::pair<QString, QJsonValue>{"grading", this->grading.toJson()}
		};


		switch (question.index()) {

		case 0:
			obj.insert("choiceQuestion", std::get<0>(this->question).toJson());
			break;
		case 1:
			obj.insert("textQuestion", std::get<1>(this->question).toJson());
			break;
		case 2:
			obj.insert("scaleQuestion", std::get<2>(this->question).toJson());
			break;
		case 3:
			obj.insert("dateQuestion", std::get<3>(this->question).toJson());
			break;
		case 4:
			obj.insert("timeQuestion", std::get<4>(this->question).toJson());
			break;
		case 5:
			obj.insert("rowQuestion", std::get<5>(this->question).toJson());
			break;

		}

		return obj;
	}

	Question Question::fromJson(const QJsonObject& json)
	{
		Question question = Question{
			.questionId = json.value("questionId").toString(),
			.required = json.value("required").toBool(),
			.grading = Grading::fromJson(json.value("grading").toObject())
		};

		if (json.contains("choiceQuestion"))
			question.question = ChoiceQuestion::fromJson(json.value("choiceQuestion").toObject());
		else if (json.contains("textQuestion"))
			question.question = TextQuestion::fromJson(json.value("textQuestion").toObject());
		else if (json.contains("scaleQuestion"))
			question.question = ScaleQuestion::fromJson(json.value("scaleQuestion").toObject());
		else if (json.contains("dateQuestion"))
			question.question = DateQuestion::fromJson(json.value("dateQuestion").toObject());
		else if (json.contains("timeQuestion"))
			question.question = TimeQuestion::fromJson(json.value("timeQuestion").toObject());
		else if (json.contains("rowQuestion"))
			question.question = RowQuestion::fromJson(json.value("rowQuestion").toObject());

		return question;
	}

	QJsonObject QuestionItem::toJson() const
	{
		bool includeImage = !this->image.sourceUri.isEmpty();

		if (includeImage)
			return QJsonObject{
				std::pair<QString, QJsonValue>{"question", this->question.toJson()},
				std::pair<QString, QJsonValue>{"image", this->image.toJson()}
			};

		return QJsonObject{
				std::pair<QString, QJsonValue>{"question", this->question.toJson()},
		};

	}

	QuestionItem QuestionItem::fromJson(const QJsonObject& json)
	{
		return QuestionItem{
			.question = Question::fromJson(json.value("question").toObject()),
			.image = Image::fromJson(json.value("image").toObject()),
		};
	}

	QJsonObject Grid::toJson() const
	{
		return QJsonObject{
			std::pair<QString, QJsonValue>{"columns", this->columns.toJson()},
			std::pair<QString, QJsonValue>{"shuffleQuestions", this->shuffleQuestions}
		};
	}

	Grid Grid::fromJson(const QJsonObject& json)
	{
		return Grid{
			.columns = ChoiceQuestion::fromJson(json.value("columns").toObject()),
			.shuffleQuestions = json.value("shuffleQuestions").toBool()
		};
	}

	QJsonObject QuestionGroupItem::toJson() const
	{
		QJsonArray quets;

		for (const Question& quet : this->questions)
			quets.append(quet.toJson());

		return QJsonObject{
			std::pair<QString, QJsonValue>{"questions", quets},
			std::pair<QString, QJsonValue>{"image", this->image.toJson()},
			std::pair<QString, QJsonValue>{"grid", this->grid.toJson()},
		};
	}

	QuestionGroupItem QuestionGroupItem::fromJson(const QJsonObject& json)
	{
		QVector<Question> questions;

		for (const QJsonValue& val : json.value("questions").toArray())
			questions.append(Question::fromJson(val.toObject()));

		return QuestionGroupItem{
			.questions = questions,
			.image = Image::fromJson(json.value("image").toObject()),
			.grid = Grid::fromJson(json.value("grid").toObject())
		};
	}

	QJsonObject Item::toJson() const
	{
		QJsonObject obj = QJsonObject{
			std::pair<QString, QJsonValue>{"itemId", this->itemId},
			std::pair<QString, QJsonValue>{"title", this->title},
			std::pair<QString, QJsonValue>{"description", this->description},
		};

		switch (this->item.index()) {

		case 0:
			obj.insert("questionItem", std::get<0>(this->item).toJson());
			break;
		case 1:
			obj.insert("questionGroupItem", std::get<1>(this->item).toJson());
			break;
		case 2:
			obj.insert("pageBreakItem", std::get<2>(this->item).toJson());
			break;
		case 3:
			obj.insert("textItem", std::get<3>(this->item).toJson());
			break;
		case 4:
			obj.insert("imageItem", std::get<4>(this->item).toJson());
			break;
		case 5:
			obj.insert("videoItem", std::get<5>(this->item).toJson());
			break;

		}

		return obj;
	}

	Item Item::fromJson(const QJsonObject& json)
	{
		Item item = Item{
			.itemId = json.value("itemId").toString(),
			.title = json.value("title").toString(),
			.description = json.value("description").toString(),
		};

		if (json.contains("questionItem"))
			item.item = QuestionItem::fromJson(json.value("questionItem").toObject());
		else if (json.contains("questionGroupItem"))
			item.item = QuestionGroupItem::fromJson(json.value("questionGroupItem").toObject());
		else if (json.contains("pageBreakItem"))
			item.item = PageBreakItem::fromJson(json.value("pageBreakItem").toObject());
		else if (json.contains("textItem"))
			item.item = TextItem::fromJson(json.value("textItem").toObject());
		else if (json.contains("imageItem"))
			item.item = ImageItem::fromJson(json.value("imageItem").toObject());
		else if (json.contains("videoItem"))
			item.item = VideoItem::fromJson(json.value("videoItem").toObject());

		return item;
	}

}