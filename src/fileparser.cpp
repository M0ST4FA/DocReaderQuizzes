#include "fileparser.h"
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QMessageBox>


FileParser::FileParser(const QString& path, QObject* parent) : QObject {parent}
{

	if (!path.isEmpty())
		_open_and_read_file(path);

}

const QVector<QChar> FileParser::PREFIX_INDICATOR_DELIMETERS = {'(', '-'};
const QVector<QChar> FileParser::POSTFIX_INDICATOR_DELIMETERS = { '.', '-', ')'};
const QChar FileParser::CORRECTNESS_INDICATOR = '*';

QVector<FileParser::CreateItemRequest> FileParser::parseFile()
{
	using CorrectAnswer = m0st4fa::forms::CorrectAnswer;
	using ChoiceQuestion = m0st4fa::forms::ChoiceQuestion;
	using Option = m0st4fa::forms::Option;
	using Question = m0st4fa::forms::Question;
	using QuestionItem = m0st4fa::forms::QuestionItem;
	using Item = m0st4fa::forms::Item;
	using Location = m0st4fa::forms::update_form::Location;

	QJsonDocument doc = _parse_file();

	if (this->m_foundError) {
		emit aborted();
		return {}; // Return an empty request vector
	}

	QJsonArray questions = doc.array();

	unsigned int location = 0;
	QVector<FileParser::CreateItemRequest> requests;

	for (const QJsonValue& quest : questions) {

		QJsonObject question = quest.toObject();

		QString questionTitle = question.value("title").toObject().value("text").toString();
		QString correctAnswer;
		QVector<Option> options;

		for (const QJsonValue& ans : question.value("options").toArray()) {

			QJsonObject answer = ans.toObject();

			bool isCorrect = answer.value("isCorrectAnswer").toBool();
			QString text = answer.value("text").toString();

			if (isCorrect)
				correctAnswer = text;

			options.append(Option{
				.value = text
				});
		}

		QuestionItem questionItem{
			.question = {
				.required = true,
				.grading {
					.pointValue = 1,
					.correctAnswers = {
						.answers = {CorrectAnswer{.value = correctAnswer}}
					}
				},
				.question = {
					ChoiceQuestion {
						.type = ChoiceQuestion::Type::RADIO,
						.options = options,
						.shuffle = true
					}
				}
			}
		};

		Item item{.title = questionTitle, .item = questionItem};

		CreateItemRequest request{ .item = item, .location {.index = location} };

		requests.append(request);

		location++;
	}

	return requests;
}

void FileParser::setFilePath(const QString& path)
{
	this->_reset_parser();
	_open_and_read_file(path);
}

bool FileParser::foundError() const
{
	return this->m_foundError;
}

void FileParser::_open_and_read_file(const QString& path)
{
	this->m_file.setFileName(path);

	bool opened = m_file.open(QIODeviceBase::OpenModeFlag::ReadOnly | QIODeviceBase::OpenModeFlag::Text | QIODeviceBase::OpenModeFlag::ExistingOnly);

	if (!opened) {

		qCritical() << "Couldn't open file.";
		qCritical() << m_file.errorString();

	}

	this->m_file.waitForReadyRead(-1);

	this->m_fileContent = this->m_file.readAll();
}

QJsonDocument FileParser::_parse_file()
{
	// The file is made from a bunch of questions

	QJsonArray questions;

	// Continue until an empty object is returned
	while (true) {

		QJsonObject question = _parse_question();

		if (question.isEmpty())
			break;

		questions.append(question);
		qInfo() << question;
	}

	return QJsonDocument{ questions };
}

// Parsing questions
QJsonObject FileParser::_parse_question()
{

	// A question is made of a title and a bunch of answers
	QJsonObject title = _parse_question_title();

	if (title.isEmpty())
		return QJsonObject{};

	QJsonArray options;

	int correctOptionNum = 0;

	while (true) {

		QJsonObject option = _parse_question_option();

		if (option.value("isCorrectAnswer").toBool())
			correctOptionNum++;

		if (option.isEmpty())
			break;

		options.append(option);
	}

	if (correctOptionNum == 0) {
		this->m_foundError = true;
		emit reportStatus(StatusReport{
		.level = ReportLevel::ERROR,
		.tag = ErrorTag::NO_CORRECT_OPTION,
		.position = this->m_currentPosition,
		.report = "Every question must specify one, and only one correct option. This question specifies non. Look at the question before the one displayed.",
		.filePath = m_file.fileName(),
		.fileContent = m_fileContent,
		.function = __FUNCTION__
			});
		return QJsonObject{};
	}

	if (correctOptionNum > 1) {
		this->m_foundError = true;
		emit reportStatus(StatusReport{
		.level = ReportLevel::ERROR,
		.tag = ErrorTag::ABUNDANT_CORRECT_OPTIONS,
		.position = this->m_currentPosition,
		.report = "Every question must specify one, and only one correct option. This question specifies more than one. Look at the question before the one displayed.",
		.filePath = m_file.fileName(),
		.fileContent = m_fileContent,
		.function = __FUNCTION__
			});
		return QJsonObject{};
	}



	return QJsonObject{
		std::pair<QString, QJsonValue>{"title", title},
		std::pair<QString, QJsonValue>{"options", options}
	};
}

QJsonObject FileParser::_parse_question_title()
{
	// Question title is a string that:
	// 1. begins with a number or nothing
	// 2. continues until a new line character

	QString questionIndicator = _parse_question_indicator();

	if (questionIndicator.isEmpty())
		return QJsonObject{};

	QString questionText = _parse_question_text();

	// If there's a question
	return QJsonObject{
		std::pair<QString, QJsonValue>{"indicator", questionIndicator},
		std::pair<QString, QJsonValue>{"text", questionText}
	};
}

QString FileParser::_parse_question_indicator()
{

	QString indicator;

	_clear_white_spaces();

	if (this->m_index >= this->m_fileContent.size()) {
		return QString{};
	}

	QChar c = this->m_fileContent.at(this->m_index);

	// Parse the prefix delimiter
	_parse_prefix_indicator_delimiter();

	// Parse the number
	for (; this->m_index < this->m_fileContent.size(); this->m_index++) {

		QChar c = this->m_fileContent.at(this->m_index);

		if (c.isNumber()) {
			indicator += c;
			this->m_currentPosition.addCharacter();
		}
		else {
			break;
		}

	}

	if (indicator.isEmpty() && this->m_index < this->m_fileContent.size())
		emit reportStatus(StatusReport{
			.level = ReportLevel::ERROR,
			.tag = ErrorTag::INCORRECT_INDICATOR,
			.position = this->m_currentPosition,
			.report = "Question indicator must be a numeral. Option indicator must be a letter. Either this indicator is empty or it doesn't have the correct format",
			.filePath = m_file.fileName(),
			.fileContent = m_fileContent,
			.function = __FUNCTION__
		});

	// Parse the postfix delimiter
	_parse_postfix_indicator_delimiter();
	
	return indicator;
}

QString FileParser::_parse_question_text()
{

	QString text;
	int nlCount = 0;

	_clear_white_spaces();

	for (; this->m_index < this->m_fileContent.size(); this->m_index++, this->m_currentPosition.charNumber++) { // Whenever index++, then charNumber++

		QChar c = this->m_fileContent.at(this->m_index);

		if (c == '\n') {
			this->m_currentPosition.adjustForNewLine();
			nlCount++;
			if (_is_option_next()) {
				this->m_index++; // skip it

				if (nlCount == 1)
					emit reportStatus(StatusReport{
					.level = ReportLevel::WARNING,
					.tag = WarningTag::POSSIBLE_OPTION_BEGINNING,
					.position = this->m_currentPosition,
					.report = "This could possibly be the beginning of an option or the continuation of a question on a new line. If you're having problems and this is the continuation of a question on a new line, consider making the question text in a single line only, instead of two.",
					.filePath = m_file.fileName(),
					.fileContent = m_fileContent,
					.function = __FUNCTION__
						});

				break;
			}
			else
				continue; // it will already be skipped in the next iteration
		}

		if (c.isLetterOrNumber() || c.isSpace() || c.isMark() || c.isPunct()) {
			text += c;
			if (c == '\n')
				this->m_currentPosition.adjustForNewLine();
			else
				this->m_currentPosition.addCharacter();
		}
		else {
			break;
		}

	}

	return text;
}

bool FileParser::_check_correct_answer()
{
	_clear_white_spaces();

	if (this->m_index >= this->m_fileContent.size())
		return false;

	QChar c = this->m_fileContent.at(this->m_index);

	if (c == "*") {
		this->m_index++;
		this->m_currentPosition.addCharacter();
		return true;
	}

	return false;
}

// Parsing question options
QJsonObject FileParser::_parse_question_option()
{

	// Question option has:
	// 1. indicator
	// 2. text

	bool isCorrectAnswer = _check_correct_answer();
	QString optionIndicator = _parse_option_indicator();

	if (optionIndicator.isEmpty()) {

		if (isCorrectAnswer) {
			this->m_foundError = true;
			emit reportStatus(StatusReport{
				.level = ReportLevel::ERROR,
				.tag = ErrorTag::CORRECTNESS_INDICATOR_IN_WRONG_PLACE,
				.position = this->m_currentPosition,
				.report = "The correctness indicator '*' can only be in front of an option indicator. It cannot be in front of a question indicator. You may have put it here incorrectly. Remove it and add in front of the correct option. ",
				.filePath = m_file.fileName(),
				.fileContent = m_fileContent,
				.function = __FUNCTION__
				});
		}

		return QJsonObject{};
	}

	QString optionText = _parse_option_text();

	return QJsonObject{
		std::pair<QString, QJsonValue>{"isCorrectAnswer", isCorrectAnswer},
		std::pair<QString, QJsonValue>{"indicator", optionIndicator},
		std::pair<QString, QJsonValue>{"text", optionText}
	};
}

QString FileParser::_parse_option_indicator()
{
	QString indicator;

	_clear_white_spaces();

	// Parse the prefix delimiter
	_parse_prefix_indicator_delimiter();

	// Parse the letter
	if (this->m_index < this->m_fileContent.size()) {

		QChar c = this->m_fileContent.at(this->m_index);

		if (c.isLetter()) {
			indicator += c;
			this->m_index++;
			this->m_currentPosition.addCharacter();
		}
		
	}

	//// Parse the postfix delimiter
	//if (_parse_postfix_indicator_delimiter() && indicator.isEmpty()) { // If you don't find it
	//	this->m_foundError = true;
	//	emit reportStatus(StatusReport{
	//		.level = ReportLevel::ERROR,
	//		.tag = ErrorTag::INCORRECT_OPTION_INDICATOR,
	//		.position = this->m_currentPosition,
	//		.report = "Incorrect option indicator. Expected a letter but found something else.",
	//		.filePath = this->m_file.fileName(),
	//		.fileContent = this->m_fileContent
	//		});
	//}
	_parse_postfix_indicator_delimiter();

	return indicator;
}

QString FileParser::_parse_option_text()
{
	QString text;

	_clear_white_spaces();

	for (; this->m_index < this->m_fileContent.size(); this->m_index++, this->m_currentPosition.charNumber++) {

		QChar c = this->m_fileContent.at(this->m_index);

		if (c == "\n") {
			this->m_index++; // skip it
			this->m_currentPosition.adjustForNewLine();
			break;
		}

		if (c.isLetterOrNumber() || c.isSpace() || c.isMark() || c.isPunct()) {
			text += c;
			if (c == '\n')
				this->m_currentPosition.adjustForNewLine();
			else
				this->m_currentPosition.addCharacter();
		}
		else {
			break;
		}

	}

	return text;
}

void FileParser::_parse_prefix_indicator_delimiter()
{
	if (this->m_index >= this->m_fileContent.size())
		return;

	QChar currentChar = this->m_fileContent.at(this->m_index);

	if (PREFIX_INDICATOR_DELIMETERS.contains(currentChar)) {
		this->m_index++;
		this->m_currentPosition.addCharacter();
	}
}

bool FileParser::_parse_postfix_indicator_delimiter()
{
	if (this->m_index >= this->m_fileContent.size())
		return false;

	QChar currentChar = this->m_fileContent.at(this->m_index);

	if (POSTFIX_INDICATOR_DELIMETERS.contains(currentChar)) {
		this->m_index++;
		this->m_currentPosition.addCharacter();
		return true;
	}

	return false;
}

// Lookahead functions
bool FileParser::_is_option_next()
{
	
	return _is_indicator_next(true, this->m_index);
}

bool FileParser::_is_indicator_next(bool option, qsizetype startIndex)
{
	// What we find is:
	// 1. potential white space (we skip it)
	// 2. we check for correctness indicator
	// 2. we check for prefix delimiter
	// 3. if we find option indicator, it will be followed by "." and then no new line
	// 4. if we find text, then it may be question text. It will be alphanumerical, so if we find alphanumerical data, this means we've found question text. (we check for numerical data)

	qsizetype index = _next_non_white_space_at(startIndex);

	// Check for correctness indicator (but only if we're checking for an option indicator)
	if (option && _is_correctness_inicator_next(index))
		return true; // only an option could have a correctness indicator

	// Check for prefix delimiter
	if (_is_prefix_indicator_delimiter_next(startIndex))
		index++;

	QChar c = this->m_fileContent.at(index);

	// Check for indicator text
	if (option) {

		if (c.isLetter()) {
			index++;
		}
		else
			return false;

	}
	else 
		for (; index < this->m_fileContent.size(); index++) {

			QChar c = this->m_fileContent.at(index);

			if (c.isSpace())
				return false;

			if (c.isDigit())
				continue;
			else
				break;
		}

	if(!_is_postfix_indicator_delimiter_next(index))
		return false;

	return true;
}

qsizetype FileParser::_next_non_white_space_at(qsizetype startIndex)
{
	for (qsizetype i = startIndex; i < this->m_fileContent.size(); i++) {

		QChar c = this->m_fileContent.at(i);

		if (c.isSpace())
			continue;

		return i;
	}

	// This is in case we're at the end of the file
	return startIndex;
}

bool FileParser::_is_question_next()
{
	return _is_indicator_next(false, this->m_index);
}

bool FileParser::_is_prefix_indicator_delimiter_next(qsizetype index)
{
	if (this->m_index >= this->m_fileContent.size())
		return false;

	return PREFIX_INDICATOR_DELIMETERS.contains(this->m_fileContent.at(index));
}

bool FileParser::_is_postfix_indicator_delimiter_next(qsizetype index)
{
	if (this->m_index >= this->m_fileContent.size())
		return false;

	return POSTFIX_INDICATOR_DELIMETERS.contains(this->m_fileContent.at(index));
}

inline bool FileParser::_is_correctness_inicator_next(qsizetype index)
{
	if (this->m_index >= this->m_fileContent.size())
		return false;

	return this->m_fileContent.at(index) == CORRECTNESS_INDICATOR;
}

// Dealing with whitespace
void FileParser::_clear_white_spaces()
{
	for (; this->m_index < this->m_fileContent.size(); this->m_index++) {

		QChar c = this->m_fileContent.at(this->m_index);

		if (c.isSpace()) {
			if (c == '\n') {
				this->m_currentPosition.adjustForNewLine();
			}
			else
				this->m_currentPosition.addCharacter();

			continue;
		}

		return;
	}

}

// Reseting parser state
void FileParser::_reset_parser()
{
	// Reset file state
	this->m_file.close();
	this->m_index = 0;
	this->m_currentPosition = Position{};
	this->m_fileContent.clear();

	// Reset document
	this->m_document = QJsonDocument{};
}

QString StatusReport::toString()
{

	QString data;
	QTextStream stream{&data, QIODeviceBase::Append};

	// Function info
#ifdef _DEBUG
	stream << "Function <" << this->function << ">\n";
#endif

	// File and line
	stream << "File <" << filePath << ">, Line " << this->position.lineNumber << "\n\t";

	// Context
	int beginning = this->position.index < this->fileContent.size() ? this->position.index : this->position.index - 1;
	int end = this->fileContent.size() - 1;

	for (int i = this->position.index; i >= 0 && i < this->fileContent.size(); i--) {

		QChar c = this->fileContent.at(i);

		if (c == '\n') {
			beginning = i + 1;
			break;
		}
	}

	for (int i = this->position.index + 1; i < fileContent.length(); i++) {

		QChar c = this->fileContent.at(i);

		if (c == '\n') {
			end = i;
			break;
		}
	}

	QString context = this->fileContent.sliced(beginning, end - beginning);

	stream << context << "\n";

	// Tag
	QString tag;

	if (this->level == ReportLevel::WARNING)
		switch (this->tag) {

		case WarningTag::POSSIBLE_OPTION_BEGINNING:
			tag = "PossibleOptionBeginningWarning: ";
			break;
		default:
			break;
		}

	if (this->level == ReportLevel::ERROR) {
		switch (this->tag) {

		case ErrorTag::INCORRECT_INDICATOR:
			tag = "IncorrectIndicator: ";
			break;
		case ErrorTag::CORRECTNESS_INDICATOR_IN_WRONG_PLACE:
			tag = "CorrectnessIndicatorInTheWrongPlace: ";
			break;
		case ErrorTag::NO_CORRECT_OPTION:
			tag = "NoCorrectOption: ";
			break;
		case ErrorTag::ABUNDANT_CORRECT_OPTIONS:
			tag = "AbundantCorrectOptions: ";
			break;
		default:
			break;
		}
	}

	stream << tag << report << "\n--------------------------------------";

	return data;
}
