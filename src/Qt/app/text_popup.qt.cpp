#include "text_popup.qt.h"
#include <QLabel>

InputPopup::InputPopup(QWidget *parent) : QDialog(parent) {
	QLabel *nameLabel1 = new QLabel("Player 1 Name:", this);
	lineEdit1 = new QLineEdit(this);
	QLabel *nameLabel2 = new QLabel("Player 2 Name:", this);
	lineEdit2 = new QLineEdit(this);
	QLabel *typeLabel1 = new QLabel("Player 1 Type:", this);
	comboBox1 = new QComboBox(this);
	comboBox1->addItem("Player");
	comboBox1->addItem("RandomPlayer");

	QLabel *typeLabel2 = new QLabel("Player 2 Type:", this);
	comboBox2 = new QComboBox(this);
	comboBox2->addItem("Player");
	comboBox2->addItem("RandomPlayer");

	submitButton = new QPushButton("Submit", this);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(nameLabel1);
	layout->addWidget(lineEdit1);
	layout->addWidget(typeLabel1);
	layout->addWidget(comboBox1);
	layout->addWidget(nameLabel2);
	layout->addWidget(lineEdit2);
	layout->addWidget(typeLabel2);
	layout->addWidget(comboBox2);
	layout->addWidget(submitButton);

	setWindowTitle("Player Settings");
	connect(submitButton, &QPushButton::clicked, this,
	        &InputPopup::onSubmitClicked);
}

void InputPopup::onSubmitClicked() { accept(); }

UiBridge::PlayerSetupData InputPopup::getPlayerSetupData() const {
	return {lineEdit1->text().toUtf8().constData(),
	        lineEdit2->text().toUtf8().constData(),
	        comboBox1->currentText().toUtf8().constData(),
	        comboBox2->currentText().toUtf8().constData()};
}
