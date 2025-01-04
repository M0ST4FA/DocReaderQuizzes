#pragma once

#include <QDialog>
#include "ui_logger.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoggerClass; };
QT_END_NAMESPACE

class Logger : public QDialog
{
	Q_OBJECT

public:
	Logger(QWidget *parent = nullptr);
	~Logger();

	QTextEdit* textEdit();

protected slots:

	void on_clearBtn_clicked();

private:
	Ui::LoggerClass *ui;
};
