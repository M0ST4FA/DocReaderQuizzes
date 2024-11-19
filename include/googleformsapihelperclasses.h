#pragma once
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>


// Items
namespace m0st4fa::forms {

	struct PageBreakItem {

		QJsonObject toJson() const {
			return QJsonObject{};
		};
		static PageBreakItem fromJson(const QJsonObject& json) {
			return PageBreakItem{};
		};

	};

	struct TextItem {

		QJsonObject toJson() const {
			return QJsonObject{};
		};
		static TextItem fromJson(const QJsonObject& json) {
			return TextItem{};
		};

	};

	struct MediaProperties {
		enum Alignment {
			ALIGNMENT_UNSPECIFIED,
			LEFT,
			RIGHT,
			CENTER
		} alignment = ALIGNMENT_UNSPECIFIED;

		int width = 0;

		QJsonObject toJson() const;
		static MediaProperties fromJson(const QJsonObject&);

	};

	struct Image {
		QString contentUri;
		QString altText;

		MediaProperties properties;

		QString sourceUri;

		QJsonObject toJson() const;
		static Image fromJson(const QJsonObject&);

	};

	struct ImageItem {
		Image image;

		QJsonObject toJson() const;
		static ImageItem fromJson(const QJsonObject&);
	};

	struct Video {
		QString youtubeUri;
		MediaProperties properties;

		QJsonObject toJson() const;
		static Video fromJson(const QJsonObject&);
	};

	struct VideoItem {
		Video video;
		QString caption;

		QJsonObject toJson() const;
		static VideoItem fromJson(const QJsonObject&);

	};

	struct RowQuestion {
		QString title;

		QJsonObject toJson() const;
		static RowQuestion fromJson(const QJsonObject&);
	};

	struct TimeQuestion {

		bool duration = false;

		QJsonObject toJson() const;
		static TimeQuestion fromJson(const QJsonObject&);
	};

	struct DateQuestion {

		bool includeTime = false;
		bool includeYear = false;
		
		QJsonObject toJson() const;
		static DateQuestion fromJson(const QJsonObject&);
	};

	struct ScaleQuestion {
		int low;
		int high;
		QString lowLabel;
		QString highLabel;

		QJsonObject toJson() const;
		static ScaleQuestion fromJson(const QJsonObject&);
	};

	struct TextQuestion {

		bool paragraph = false;

		QJsonObject toJson() const;
		static TextQuestion fromJson(const QJsonObject&);
	};

	struct Option {

		QString value;
		Image image;
		bool isOther = false;

		enum GoToAction {
			GO_TO_ACTION_UNSPECIFIED,
			NEXT_SECTION,
			RESTART_FORM,
			SUBMIT_FORM
		};

		using GoTo = std::variant<GoToAction, QString>;

		GoTo goTo;

		static QString goToActionToString(GoToAction action) {

			switch (action)
			{
			case m0st4fa::forms::Option::GO_TO_ACTION_UNSPECIFIED:
				return "GO_TO_ACTION_UNSPECIFIED";
				break;
			case m0st4fa::forms::Option::NEXT_SECTION:
				return "NEXT_SECTION";
				break;
			case m0st4fa::forms::Option::RESTART_FORM:
				return "RESTART_FORM";
				break;
			case m0st4fa::forms::Option::SUBMIT_FORM:
				return "SUBMIT_FORM";
				break;
			default:
				break;
			}

			return "GO_TO_ACTION_UNSPECIFIED";
		};
		static GoToAction stringToGoToAction(const QString& action) {

			if (action == "GO_TO_ACTION_UNSPECIFIED")
				return GO_TO_ACTION_UNSPECIFIED;
			else if (action == "NEXT_SECTION")
				return NEXT_SECTION;
			else if (action == "RESTART_FORM")
				return RESTART_FORM;
			else
				return SUBMIT_FORM;

		};
		QJsonObject toJson() const;
		static Option fromJson(const QJsonObject&);

	};

	struct ChoiceQuestion {

		enum ChoiceType {
			CHOICE_TYPE_UNSPECIFIED,
			RADIO,
			CHECKBOX,
			DROP_DOWN
		} type = CHOICE_TYPE_UNSPECIFIED;

		QVector<Option> options;

		bool shuffle = false;

		static QString choiceTypeToString(ChoiceType type) {

			switch (type)
			{
			case m0st4fa::forms::ChoiceQuestion::CHOICE_TYPE_UNSPECIFIED:
				return "CHOICE_TYPE_UNSPECIFIED";
				break;
			case m0st4fa::forms::ChoiceQuestion::RADIO:
				return "RADIO";
				break;
			case m0st4fa::forms::ChoiceQuestion::CHECKBOX:
				return "CHECKBOX";
				break;
			case m0st4fa::forms::ChoiceQuestion::DROP_DOWN:
				return "DROP_DOWN";
				break;
			default:
				break;
			}

			return "CHOICE_TYPE_UNSPECIFIED";
		}
		static ChoiceType stringToChoiceType(QString type) {
			
			if (type == "CHOICE_TYPE_UNSPECIFIED")
				return CHOICE_TYPE_UNSPECIFIED;
			else if (type == "RADIO")
				return RADIO;
			else if (type == "CHECKBOX")
				return CHECKBOX;
			else
				return DROP_DOWN;

		}
		QJsonObject toJson() const;
		static ChoiceQuestion fromJson(const QJsonObject&);
	};

	struct CorrectAnswer{

		QString value;

		QJsonObject toJson() const;
		static CorrectAnswer fromJson(const QJsonObject&);
	};

	struct CorrectAnswers {

		QVector<CorrectAnswer> answers;

		QJsonObject toJson() const;
		static CorrectAnswers fromJson(const QJsonObject&);

	};

	struct TextLink {

		QString uri;
		QString displayText;

		QJsonObject toJson() const;
		static TextLink fromJson(const QJsonObject&);
	};

	struct VideoLink {

		QString displayText;
		QString youtubeUri;

		QJsonObject toJson() const;
		static VideoLink fromJson(const QJsonObject&);
	};

	struct ExtraMaterial {

		using LinkType = std::variant<TextLink, VideoLink>;

		LinkType link;

		QJsonObject toJson() const;
		static ExtraMaterial fromJson(const QJsonObject&);

	};

	struct Feedback {

		QString text;

		QVector<ExtraMaterial> material;

		QJsonObject toJson() const;
		static Feedback fromJson(const QJsonObject&);

	};

	struct Grading {

		int pointValue;
		CorrectAnswers correctAnswers;

		Feedback whenRight;
		Feedback whenWrong;
		Feedback generalFeedback;

		QJsonObject toJson() const;
		static Grading fromJson(const QJsonObject&);
	};

	struct Question {

		QString questionId;
		bool required = false;
		Grading grading;

		using QuestionType = std::variant<
			ChoiceQuestion,
			TextQuestion,
			ScaleQuestion,
			DateQuestion,
			TimeQuestion,
			RowQuestion
		>;

		QuestionType question;

		QJsonObject toJson() const;
		static Question fromJson(const QJsonObject&);
	};

	struct QuestionItem {

		Question question;
		Image image;

		QJsonObject toJson() const;
		static QuestionItem fromJson(const QJsonObject&);
	};

	struct Grid {

		ChoiceQuestion columns;
		bool shuffleQuestions = false;

		QJsonObject toJson() const;
		static Grid fromJson(const QJsonObject&);

	};

	struct QuestionGroupItem {
		QVector<Question> questions;
		Image image;

		Grid grid;

		QJsonObject toJson() const;
		static QuestionGroupItem fromJson(const QJsonObject&);
	};

	struct Item {

		QString itemId;
		QString title;
		QString description;

		using ItemObject = std::variant<
			QuestionItem,
			QuestionGroupItem,
			PageBreakItem,
			TextItem,
			ImageItem,
			VideoItem
		>;

		ItemObject item;

		QJsonObject toJson() const;
		static Item fromJson(const QJsonObject&);
	};

}

namespace m0st4fa::forms {


	struct Info {
		QString title;
		QString documentTitle;
		QString description;

		QJsonObject toJson() const;

		static Info fromJson(const QJsonObject&);
	};

	struct Settings {

		struct QuizSettings {
			bool isQuiz = false;
		} quizSettings;

		QJsonObject toJson() const;

		static Settings fromJson(const QJsonObject&);

	};

	struct Form {

		QString formId;
		Info info;
		Settings settings;
		QVector<Item> items;
		QString revisionId;
		QString responderUri;
		QString linkedSheetId;

		QJsonObject toJson() const;
		static Form fromJson(const QJsonObject&);

	};

}

namespace m0st4fa::forms::update_form {

	struct UpdateFormInfoRequest {
		Info info;
		QString updateMask = "";

		QJsonObject toJson() const;
		static UpdateFormInfoRequest fromJson(const QJsonObject&);

	};

	struct UpdateSettingsRequest {
		Settings settings;
		QString updateMask = "";

		QJsonObject toJson() const;
		static UpdateSettingsRequest fromJson(const QJsonObject&);
	};

	struct Location {
		unsigned int index = 0;

		QJsonValue toJson() const;
		static Location fromJson(const QJsonValue& val) {
			return Location{
				.index = (unsigned)val.toInt()
			};
		}
	};

	struct CreateItemRequest {
		Item item;
		Location location;

		QJsonObject toJson() const;
		static CreateItemRequest fromJson(const QJsonObject&);
	};

	struct MoveItemRequest {
		Location originalLocation;
		Location newLocation;

		QJsonObject toJson() const;
		static MoveItemRequest fromJson(const QJsonObject&);

	};

	struct DeleteItemRequest {
		Location location;

		QJsonObject toJson() const;
		static DeleteItemRequest fromJson(const QJsonObject&);
	};

	struct UpdateItemRequest {
		Item item;
		Location location;
		QString updateMask = "";

		QJsonObject toJson() const;
		static UpdateItemRequest fromJson(const QJsonObject&);
	};

	struct UpdateRequest {

		enum UpdateRequestType {
			UPDATE_FORM_INFO = 0,
			UPDATE_SETTINGS,
			CREATE_ITEM,
			MOVE_ITEM,
			DELETE_ITEM,
			UPDATE_ITEM,
			NONE
		};

		using Request = std::variant<
			UpdateFormInfoRequest,
			UpdateSettingsRequest,
			CreateItemRequest,
			MoveItemRequest,
			DeleteItemRequest,
			UpdateItemRequest
		>;
		Request request;

		QJsonObject toJson() const;

		static UpdateRequest fromJson(const QJsonObject&);

	};

	struct WriteControl {

		/**
		 * @brief `true` if revision Id required; `false` if it is just the target.
		 */
		bool isRequired = true;

		/**
		 * @brief The revision Id.
		 */
		QString revisionId;

		QJsonObject toJson() const;
		static WriteControl fromJson(const QJsonObject&);
	};

	struct UpdateRequestBody {
		bool includeFormInResponse = false;
		QVector<UpdateRequest> requests;
		WriteControl writeControl;

		QJsonObject toJson() const;
		static UpdateRequestBody fromJson(const QJsonObject&);

	};

	struct UpdateResponse {

		QString itemId;
		QVector<QString> questionId;

		QJsonObject toJson() const;
		static UpdateResponse fromJson(const QJsonObject&);

	};

	struct UpdateResponseBody {

		std::shared_ptr<Form> form;
		QVector<UpdateResponse> replies;
		WriteControl writeControl;

		QJsonObject toJson() const;
		static UpdateResponseBody fromJson(const QJsonObject&);

	};
}