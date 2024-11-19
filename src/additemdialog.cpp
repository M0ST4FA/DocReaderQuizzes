#include "additemdialog.h"
#include <QInputDialog>
#include <QStandardItemModel>
#include <QStandardItem>

AddItemDialog::AddItemDialog(std::shared_ptr<Form> form, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::AddItemDialogClass())
	, m_form{form}
{
	ui->setupUi(this);

	connect(this->ui->addItemDialogBtnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(this->ui->addItemDialogBtnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	connect(this->ui->optionsCombobox, &QComboBox::currentTextChanged, [this](const QString& newText) {

		int currentIndex = this->ui->optionsCombobox->currentIndex();

		this->ui->correctAnswerCombobox->setItemData(currentIndex, newText, Qt::DisplayRole);

		});

}

AddItemDialog::~AddItemDialog()
{
	delete ui;
}

bool AddItemDialog::isQuestionRequired() const
{
	return this->ui->questionRequiredCheckbox->isChecked();
}

bool AddItemDialog::isShufflingEnabled() const
{
	return this->ui->shuffleOptionsCheckbox->isChecked();
}

QVector<AddItemDialog::Option> AddItemDialog::getOptions() const
{

	QComboBox* combo = this->ui->optionsCombobox;
	QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(combo->model());
	QVector<Option> options;

	for (int i = 0; i < model->rowCount(); i++) {

		QString optionText = model->item(i, 0)->data(Qt::DisplayRole).toString();

		Option option{ .value = optionText };

		options.append(option);
	}

	return options;
}

void AddItemDialog::on_addOptionBtn_clicked()
{

	QString option = QInputDialog::getText(this, "Add MCQ.", "Enter MCQ Option.", QLineEdit::Normal);

	this->ui->optionsCombobox->addItem(option);
	this->ui->correctAnswerCombobox->addItem(option);

}
