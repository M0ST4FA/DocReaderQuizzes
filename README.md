# DocReaderQuizzes Quiz Generator

A simple application that converts a text file into a Google Forms quiz.

## Table of Contents

- [About The Project](#about-the-project)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Settings](#settings)
- [Text File Specification/Format](#text-file-specification)
- [Precautions](#precautions)
- [Roadmap](#roadmap)
- [License](#license)
- [Third-party Libraries](#third-party-libraries)

## About The Project

This project allows users to create quizzes from a text file formatted in a specific way. It automates the process of generating questions and answers for Google Forms.

## Getting Started

To get a local copy up and running follow these simple steps.

### Dependencies

- Qt 6.8.x (dynamically-linked.)

### Installation

#### For Developers

You can fork the repo or you can clone it. Then you can build it locally and modify it as you like.

#### For end users

Install the built app from the [releases page](https://github.com/M0ST4FA/DocReaderQuizzes/releases).

> [!NOTE]
> Choose the latest version, since it will have the latest fixes and features.

> [!IMPORTANT]
> Be sure to choose the build that matches your OS. Otherwise, the app will not work on your machine.

## Usage

1. Open Quizzes.exe
2. When you open it for the first time, you will be directed to the authentication page.
   ![Authentication page](assets/images/login-page.png 'Login page')

   - Press `Login with Google` button. This will direct you to the familiar Google authentication page.
   - Your credentials will be saved locally (we don't save any sensitive information on any server, only on your computer for convenience purposes) so that you don't have to re-authenticate the next times you open the app.

3. After you authenticate, you will be directed to the main page.
   ![Main page](assets/images/application-page.png 'Main page')

   - **In this page:**
     1. Enter the title of the form.
     2. Enter the title of the document in your Google Drive.
     3. Enter a description of the form.
     4. Click `Choose File`. This will open up a file dialog. Choose the file from which the form will be generated.
        - There are two types of files you can choose from, either text or PDF files.

4. In case you chose a text file, everything will be straightforward from here (it is similar to the next steps, so follow up.)
5. In case you chose a PDF file, you will have this window:
   ![Choose PDF window](assets/images/choose-pdf.png 'Choose PDF window')

   - After clicking `Process PDF`, you will have to wait until the AI processes the PDF.

6. After it finishes, it will open up notepad with the text that Gemini has generated. Ensure that it adheres the [syntax](#text-file-specification) expected by the app.

   - You will have this window now:
     ![Create quiz](assets/images/create-quiz-page.png 'Create quiz')
   - Press `Create Quiz` and your quiz will be created.

7. If the form was created successfully, you get the following dialog:
   ![Successfully created form](/assets/images/successfully-created-form.png 'Successfully created form')

   - The link to the form is copied to your clipboard.

8. If there are fatal errors, you will get the following dialog:
   ![Errors dialog](/assets/images/errors-dialog.png 'Errors dialog')
   - If you press Ctrl+L, this will open the logger window, that will give you important information that will help you solve the problem.
     ![Logger window](/assets/images/logger-window.png 'Logger window')

## Settings

In order to show the settings page, press Ctrl+S.

The following window will open:

![Settings page](assets/images/settings-page-arrows.png 'Settings page')

**In this window:**

1. This allows you to set the language of the application. Effective on restart.
2. This indicates whether you want all questions to be required or not.

   - In case you make them all required, a person who solves the quiz has to solve every question in it before they submit it.

   - In the other case, they can submit the form before solving every question.

3. Whether or not you want to include option indicator (the letter in front of the option.)
4. The prompt used by the AI to convert the input PDF to a textual representation understood by the application. This gives you the option to modify the prompt.

## Text File Specification

#### Format

- The text file consists of a list of questions.

- Each question consists of:

  1. Question title
  2. Options

- Each question title consist of:

  1. Question indicator, which is a numeral followed by one of the following: ".", "-", ")". It can also be preceded by one of the following: "(", "-").
  2. Question text, which can span multiple lines.

- Each option consists of:
  1. (optional) Correct answer indicator, which is "\*", and indicates a correct answer. There must be one, and only one correct answer. Otherwise, an error is indicated.
  2. Option indicator, which is a sequence of one or more letters followed by one of the following: ".", "-", ")". It can also be preceded by one of the following: "(", "-").
  3. Option text, which can span multiple lines.

#### Example of Text File

```text
1. What is the outer layer
of the heart?
	* a. Epicardium
	b. Pericardium
	c. Endocardium
	d. Tunica
  intima
2) The value of the GFR is about:
  (a) 150 ml/min
  *b- 125 ml/min
  c. 400 ml/min
  d. 500 ml/min
```

Notice how the first question spans multiple lines. Likewise, the last option of the first question spans multiple lines. Also notice that you can indent options for ease of reading. The correct answer is indicated by "\*" preceding the option indicator.

## Precautions

1. If the PDF is too large (contains too much information, independent on its size), the app may "crash".

   - This is due to the fact that Gemini has imposed certain restrictions on the size of a request but I haven't handled this condition yet in the app 🥸.

2. The AI ignores any solutions on the file and solves any questions itself. This implies that you will always have to verify its solutions.

## Roadmap

- [ ] Support additional quiz specifications (e.g., TOML and YAML)
- [x] Support generating quizzes from pdf directly (you can do it now with copy-and-paste, assuming correct format)
- [ ] Support generating quizzes from image directly (you can do it now with copy-and-paste and the help of Google lens, assuming correct format)

## License

See [LICENSE](LICENSE). for more information.

## Third-party Libraries

This application uses the following third-party libraries:

- **Qt Libraries**: Licensed under the GNU Lesser General Public License (LGPL).
  - Source code available at: https://github.com/qt/qtbase
  - Full text of LGPL license: https://www.gnu.org/licenses/lgpl-3.0.txt
