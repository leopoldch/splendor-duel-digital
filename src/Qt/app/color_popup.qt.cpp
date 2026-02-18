#include "color_popup.qt.h"
#include "game.h"
#include "main_window.qt.h"
#include <QLabel>
#include <string>

ColorPopup::ColorPopup(QWidget *parent) : QDialog(parent) {

	QLabel *typeLabel = new QLabel("Color:", this);
	comboBox = new QComboBox(this);

	comboBox->addItem("White");
	comboBox->addItem("Blue");
	comboBox->addItem("Black");
	comboBox->addItem("Pearl");
	comboBox->addItem("Red");
	comboBox->addItem("Green");

	QLabel *amountLabel = new QLabel("Amount:", this);
	comboBoxNb = new QComboBox(this);
	for (int i = 1;
	     i <= Game::getGame().getCurrentPlayer().TokenAmount(Color::gold);
	     i++) {
		comboBoxNb->addItem(QString::fromStdString(std::to_string(i)));
	}

	submitButton = new QPushButton("Submit", this);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(typeLabel);
	layout->addWidget(comboBoxNb);
	layout->addWidget(comboBox);

	setWindowTitle("Color choice");

	connect(submitButton, &QPushButton::clicked, this,
	        &ColorPopup::onSubmitClicked);
}

void ColorPopup::onSubmitClicked() {
	std::string nb_string = comboBoxNb->currentText().toStdString();
	std::string colorStr = comboBox->currentText().toStdString();

	setColor(stringToColor(colorStr));
	setNb(std::stoi(nb_string));

	accept();
}
