
#include <QApplication>
#include "docreaderquizzes.h"
#include <QIcon>

int main(int argc, char** argv) {

	QApplication app{argc, argv};
	app.setOrganizationName("DocReader");
	app.setApplicationName("Quizzes");

	QIcon appIcon{ ":/icons/appicon.ico" };

	if (appIcon.isNull())
		qInfo() << "App icon didn't load.";
	else
		app.setWindowIcon(appIcon);

	DocReaderQuizzes mainApp{};

	mainApp.show();

	app.exec();
}
