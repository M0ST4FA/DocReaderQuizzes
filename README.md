# DocReaderQuizzes Quiz Generator

A simple application that converts a text file into a Google Forms quiz.

## Table of Contents
- [About The Project](#about-the-project)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Roadmap](#roadmap)
- [License](#license)
- [Third-party Libraries](#third-party-libraries)

## About The Project

This project allows users to create quizzes from a text file formatted in a specific way. It automates the process of generating questions and answers for Google Forms.

## Getting Started

To get a local copy up and running follow these simple steps.

### Dependencies

- Qt 6.8.x (dynamically-linked)

### Installation

Right now, the only way is to clone the repo and build the application from source, but other installation methods are coming soon.

## Usage

1. Open DocReaderQuizzes.exe
2. When you open it for the first time, it will ask you to authenticate with your Google account. It saves the refresh token (e.g., in the Windows registry in case of Windows) so that you don't have to authenticate every time you open the app. After you authenticate, the main page will open.
3. In this page, you have to enter the title of the form, the name of the document in GDrive, and a description of the form. You then choose a file from which the form will be generated.

### Text File Specification

- Questions consist of:
  1. Question title
  2. Options

- Question titles consist of:
  1. Question indicator, which is a numeral followed by "."
  2. Question text, which can span multiple lines.

- Each option consists of:
  1. (optional) Correct answer indicator, which is "*", and indicates a correct answer. There must be one, and only one correct answer. Otherwise, behavior is undefined. This behavior is not enforced by the parser.
  2. Option indicator, which is a sequence of one or more letters followed by "."
  3. Option text, which cannot span multiple lines

### Example of Text File Specification

``` text
1. What is the outer layer 
of the heart?
	a. Epicardium
	* b. Pericardium
	c. Endocardium
	d. Tunica intima
2. The value of the GFR is:
  a. 150 ml/min
  *b. 125 ml/min
  c. 400 ml/min
  d. 500 ml/min
```
Notice how  the first question spans multiple lines. Also notice that you can indent options for ease of reading. The correct answer is indicated by "*" preceding the option indicator.

### Roadmap

- [ ] Support additional quiz specifications (e.g., TOML and YAML)
- [ ] Support generating quizzes from pdf directly (you can do it now with copy-and-paste, assuming correct format)
- [ ] Support generating quizzes from image directly (you can do it now with copy-and-paste and the help of Google lens, assuming correct format)

### License

See [LICENSE](LICENSE) for more information.

### Third-party Libraries

This application uses the following third-party libraries:

- **Qt Libraries**: Licensed under the GNU Lesser General Public License (LGPL).
  - Source code available at: https://github.com/qt/qtbase
  - Full text of LGPL license: https://www.gnu.org/licenses/lgpl-3.0.txt
