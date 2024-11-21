#pragma once

#include <QJsonDocument>
#include <QFile>
#include "googleformsapihelperclasses.h"
#include <memory>

class FileParser{
	
public:

	using CreateItemRequest = m0st4fa::forms::update_form::CreateItemRequest;
	using Form = m0st4fa::forms::Form;

	FileParser(std::shared_ptr<Form> = nullptr, const QString& = "");

	QVector<FileParser::CreateItemRequest> parseFile();

	void setFilePath(const QString&);
	void setForm(std::shared_ptr<Form>);

private:

	std::shared_ptr<Form> m_form;
	QFile m_file;
	qsizetype m_index = 0;
	QString m_fileContent;
	QJsonDocument m_document;

	void _open_and_read_file(const QString&);

	QJsonDocument _parse_file();

	QJsonObject _parse_question();
	QJsonObject _parse_question_title();
	QJsonObject _parse_question_option();

	QString _parse_question_indicator();
	QString _parse_question_text();

	bool _check_correct_answer();
	QString _parse_option_indicator();
	QString _parse_option_text();

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
	bool _is_option_indicator_next(qsizetype);

	void _clear_white_spaces();

	/**
	 * @brief Gets the index of the next non-white-space character.
	 * @returns The index of the next non-white-space character.
	 */
	qsizetype _next_non_white_space_at(qsizetype);

	void _reset_parser();

};
