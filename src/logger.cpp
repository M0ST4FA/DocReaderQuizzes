#include "logger.h"

Logger::Logger(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::LoggerClass())
{
	ui->setupUi(this);
}

Logger::~Logger()
{
	delete ui;
}

QTextEdit* Logger::textEdit()
{
	return this->ui->textEdit;
}
