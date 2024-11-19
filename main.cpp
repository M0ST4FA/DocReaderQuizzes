
#include <QApplication>
#include <iostream>
#include "ez4ms.h"
#include "fileparser.h"

int main(int argc, char** argv) {

	QApplication app{argc, argv};
	app.setOrganizationName("DocReader");
	app.setApplicationName("Quizzes");

	DocReaderQuizzesApp GUI{};
	GUI.show();

	FileParser parser{ nullptr, DEVELOPMENT_PATH"/resources/MCQs.txt" };

	app.exec();
}
