
#include <QApplication>
#include <iostream>
#include "docreaderquizzes.h"
#include "fileparser.h"
#include <QIcon>

int main(int argc, char** argv) {

	QApplication app{argc, argv};
	app.setOrganizationName("DocReader");
	app.setApplicationName("Quizzes");

	QIcon appIcon{ "./assets/icons/appicon.ico" };

	if (appIcon.isNull())
		qInfo() << "App icon didn't load.";
	else
		app.setWindowIcon(appIcon);

	DocReaderQuizzes mainApp{};

	//mainApp.setWindowIcon(QIcon{ ":/icons/appicon.ico" });
	mainApp.show();

	app.exec();
}
