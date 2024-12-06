
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

	QObject::connect(gemini, &GoogleGeminiAPI::promptExecuted, [](const GenerateContentResponse& res) {
		});

	gemini->executePrompt("Extract the text out of this image. Be conservative. Do not include in the response any text that is not part of the file. Do not include any headers; only questions and options. Extract all questions and options. Indicate the correct option by prepending an * before the letter denoting it. The correct option is colored red.", "C:/Users/mosta/Desktop/Temporary/mcqs.pdf");

	QObject::connect(gemini, &GoogleGeminiAPI::promptExecuted, [](const m0st4fa::gemini::GenerateContentResponse& response) {
		qInfo() << std::get<0>(response.candidates.at(0).content.parts.at(0).part);
		});

	app.exec();
}
