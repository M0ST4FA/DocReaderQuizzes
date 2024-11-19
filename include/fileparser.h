#pragma once

#include <QJsonDocument>
#include <QFile>
#include "googleformsapihelperclasses.h"
#include <memory>

class FileParser{
	
public:

	using CreateItemRequest = m0st4fa::forms::update_form::CreateItemRequest;
	using Form = m0st4fa::forms::Form;

	FileParser(std::shared_ptr<Form>, const QString&);

	QVector<FileParser::CreateItemRequest> parseFile();

private:

	std::shared_ptr<Form> m_form;
	QFile m_file;
	qsizetype m_index = 0;
	QString m_fileContent;
	QJsonDocument m_document;

	QJsonDocument _parse_file();
	QJsonObject _parse_question();
	QJsonObject _parse_question_title();
	QJsonObject _parse_question_option();

	QString _parse_question_indicator();
	QString _parse_question_text();

	bool _check_correct_answer();
	QString _parse_option_indicator();
	QString _parse_option_text();

	void _clear_white_spaces();

};
