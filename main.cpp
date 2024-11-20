
#include <QApplication>
#include <iostream>
#include "docreaderquizes.h"
#include "fileparser.h"

int main(int argc, char** argv) {

	QApplication app{argc, argv};
	app.setOrganizationName("DocReader");
	app.setApplicationName("Quizzes");

	DocReaderQuizzes mainApp{};
	mainApp.show();

	app.exec();
}
