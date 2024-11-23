#include "fileparser.h"
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>


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

	while (true) {

		QJsonObject option = _parse_question_option();

		if (option.isEmpty())
			break;

		options.append(option);
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

	// Parse the prefix delimiter
	_parse_prefix_indicator_delimiter();

	// Parse the number
	for (; this->m_index < this->m_fileContent.size(); this->m_index++) {

		QChar c = this->m_fileContent.at(this->m_index);

		if (c.isNumber()) {
			indicator += c;
			this->m_currentPosition.charNumber++;
		}
		else {
			break;
		}

	}

	// Parse the postfix delimiter
	_parse_postfix_indicator_delimiter();
	
	return indicator;
}

QString FileParser::_parse_question_text()
{

	QString text;

	_clear_white_spaces();

	for (; this->m_index < this->m_fileContent.size(); this->m_index++, this->m_currentPosition.charNumber++) { // Whenever index++, then charNumber++

		QChar c = this->m_fileContent.at(this->m_index);

		if (c == "\n") {
			if (_is_option_next()) {
				this->m_index++; // skip it
				this->m_currentPosition.lineNumber++;
				this->m_currentPosition.charNumber = 1;
				break;
			}
			else
				continue; // it will already be skipped in the next iteration
		}

		if (c.isLetterOrNumber() || c.isSpace() || c.isMark() || c.isPunct())
			text += c;
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

	if (optionIndicator.isEmpty())
		return QJsonObject{};

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

	// Parse the number
	for (; this->m_index < this->m_fileContent.size(); this->m_index++, this->m_currentPosition.charNumber++) {

		QChar c = this->m_fileContent.at(this->m_index);

		if (c.isLetter())
			indicator += c;
		else {
			break;
		}

	}

	// Parse the postfix delimiter
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
			this->m_currentPosition.charNumber++;
			break;
		}

		if (c.isLetterOrNumber() || c.isSpace() || c.isMark() || c.isPunct())
			text += c;
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
		this->m_currentPosition.charNumber++;
	}
}

void FileParser::_parse_postfix_indicator_delimiter()
{
	if (this->m_index >= this->m_fileContent.size())
		return;

	QChar currentChar = this->m_fileContent.at(this->m_index);

	if (POSTFIX_INDICATOR_DELIMETERS.contains(currentChar)) {
		this->m_index++;
		this->m_currentPosition.charNumber++;
	}
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
			emit warning(ParserWarning{
				.type = ParserWarning::POSSIBLE_OPTION_BEGINNING,
				.position = this->m_currentPosition 
				});
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
				this->m_currentPosition.lineNumber++;
				this->m_currentPosition.charNumber = 1;
			}
			else
				this->m_currentPosition.charNumber++;

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