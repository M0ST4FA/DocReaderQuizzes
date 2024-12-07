
#include <QApplication>
#include <iostream>
#include "docreaderquizzes.h"
#include "fileparser.h"
#include "googlegeminiapi.h"
#include "googlegeminiapihelperclasses.h"
#include <QIcon>

using namespace m0st4fa::gemini;

int main(int argc, char** argv) {

	QApplication app{argc, argv};
	app.setOrganizationName("DocReader");
	app.setApplicationName("Quizzes");

	QIcon appIcon{ "./assets/icons/appicon.svg" };

	if (appIcon.isNull())
		qInfo() << "App icon didn't load.";
	else
		app.setWindowIcon(appIcon);

	DocReaderQuizzes mainApp{};

	//mainApp.setWindowIcon(QIcon{ ":/icons/appicon.ico" });
	mainApp.show();

	GoogleGeminiAPI* gemini = new GoogleGeminiAPI{ &mainApp };

	app.exec();
}
