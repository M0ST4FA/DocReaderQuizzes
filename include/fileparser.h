#pragma once

#include <QJsonDocument>
#include <QFile>
#include "googleformsapihelperclasses.h"
#include <memory>

struct Position {
	int charNumber = 1;
	int lineNumber = 1;
	int index = 0;

	void adjustForNewLine() {
		this->lineNumber++;
		this->charNumber = 1;
		this->index++;
	}

	void addCharacter() {
		this->charNumber++;
		this->index++;
	}
};

enum ReportLevel {
	INFO,
	WARNING,
	ERROR
};

enum WarningTag {
	POSSIBLE_OPTION_BEGINNING,
};

enum ErrorTag {
	INCORRECT_INDICATOR,
	CORRECTNESS_INDICATOR_IN_WRONG_PLACE,
	ZERO_OPTIONS_SPECIFIED,
	NO_CORRECT_OPTION,
	ABUNDANT_CORRECT_OPTIONS
};

struct StatusReport {

	ReportLevel level;
	int tag;
	Position position;
	QString report;
	const QString& filePath;
	const QString& fileContent;
	const char* function = nullptr;

	QString toString();
};

class FileParser : public QObject {
	
	Q_OBJECT

public:

	using CreateItemRequest = m0st4fa::forms::update_form::CreateItemRequest;
	using Form = m0st4fa::forms::Form;

	FileParser(const QString& = "", bool requireAllQuestions = true, bool includeOptionIndicator = true, QObject* = nullptr);

	QVector<FileParser::CreateItemRequest> parseFile();

	void setFilePath(const QString&);
	bool foundError() const;
	void resetErrorState();
	void setSettings(bool requireAllQuestions, bool includeOptionIndicator);

signals:

	void reportStatus(StatusReport report);
	void aborted();

private:

	static const QVector<QChar> POSTFIX_INDICATOR_DELIMETERS;
	static const QVector<QChar> PREFIX_INDICATOR_DELIMETERS;
	static const QChar CORRECTNESS_INDICATOR;

	// Text file state and data
	QFile m_file;
	qsizetype m_index = 0;
	Position m_currentPosition;
	QString m_fileContent;
	bool m_foundError = false;

	// Settings
	bool m_requireAllQuestions = true;
	bool m_includeOptionIndicator = true;

	// Json document intermediate generated from text file
	QJsonDocument m_document;

	// HELPER FUNCTIONS
	void _open_and_read_file(const QString&);
	QVector<FileParser::CreateItemRequest> _questions_to_requests(const QJsonArray&);

	// PARSING FUNCTIONS
	QJsonDocument _parse_file();

	// Question parsing
	QJsonObject _parse_question();
	QJsonObject _parse_question_title();

	QString _parse_question_indicator();
	QString _parse_question_text();

	// Option parsing
	QJsonObject _parse_question_option();
	bool _check_correct_answer();
	QString _parse_option_indicator();
	QString _parse_option_text();

	// Indicator parsing
	void _parse_prefix_indicator_delimiter();
	bool _parse_postfix_indicator_delimiter();

	// Lookahead functions (to reduce context sensitivity)
	/**
	 * @brief Checks for whether what comes after a new line is an option or not.
	 * @warning Called after new line only.
	 * @returns `true` if the next item is an option; `false` otherwise.
	 */
	bool _is_option_next();
	/**
	 * @brief Checks for whether an option indicator starts at the provided index. In particular, it checks that there's an option indicator not followed by new line.
	 * @returns `true` if there's an option indicator next; `false` otherwise.
	 */
	bool _is_indicator_next(bool, qsizetype);
	/**
	 * @brief Gets the index of the next non-white-space character.
	 * @returns The index of the next non-white-space character.
	 */
	qsizetype _next_non_white_space_at(qsizetype);

	/**
	 * @brief Checks for whether what comes after a new line is a question or not.
	 * @warning Called after new line only.
	 * @returns `true` if the next item is an option; `false` otherwise.
	 */
	bool _is_question_next();

	/**
	 * @brief Checks for whether a question indicator starts at the provided index. In particular, it checks that there's an question indicator not followed by new line.
	 * @returns `true` if there's an question indicator next; `false` otherwise.
	 */
	inline bool _is_question_indicator_next(qsizetype);

	inline bool _is_prefix_indicator_delimiter_next(qsizetype);

	inline bool _is_postfix_indicator_delimiter_next(qsizetype);

	inline bool _is_correctness_inicator_next(qsizetype);

	// Handling white space
	void _clear_white_spaces();

	// Reseting parser state
	void _reset_parser();

};
