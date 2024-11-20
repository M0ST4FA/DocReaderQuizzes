
#include <QApplication>
#include <iostream>
#include "ez4ms.h"
#include "docreaderquizes.h"
#include "fileparser.h"

int main(int argc, char** argv) {

	QApplication app{argc, argv};
	app.setOrganizationName("DocReader");
	app.setApplicationName("Quizzes");

	/*ez4ms testApp{};
	testApp.show();*/

	DocReaderQuizzes mainApp{};
	mainApp.show();

	app.exec();
}
