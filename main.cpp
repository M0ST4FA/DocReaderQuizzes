
#include <QApplication>
#include <iostream>
#include "ez4ms.h"

int main(int argc, char** argv) {

	QApplication app{argc, argv};
	app.setOrganizationName("DocReader");
	app.setApplicationName("Quizzes");

	DocReaderQuizzesApp GUI{};
	GUI.show();

	app.exec();
}
