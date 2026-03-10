#include "color_popup.qt.h"
#include <QLabel>
#include <string>

ColorPopup::ColorPopup(const UiBridge::ColorChoiceViewState &state,
                       QWidget *parent)
    : QDialog(parent), c(Color::blue), nb(0) {

	QLabel *typeLabel = new QLabel("Color:", this);
	comboBox = new QComboBox(this);

	for (const auto &option : state.colors) {
		comboBox->addItem(QString::fromStdString(option.label));
	}

	QLabel *amountLabel = new QLabel("Amount:", this);
	comboBoxNb = new QComboBox(this);
	for (int amount : state.amounts) {
		comboBoxNb->addItem(QString::number(amount));
	}

	submitButton = new QPushButton("Submit", this);
	submitButton->setEnabled(!state.colors.empty() && !state.amounts.empty());

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(typeLabel);
	layout->addWidget(comboBox);
	layout->addWidget(amountLabel);
	layout->addWidget(comboBoxNb);
	layout->addWidget(submitButton);

	setWindowTitle("Color choice");

	connect(submitButton, &QPushButton::clicked, this,
	        &ColorPopup::onSubmitClicked);
}

void ColorPopup::onSubmitClicked() {
	std::string nb_string = comboBoxNb->currentText().toUtf8().constData();
	std::string colorStr = comboBox->currentText().toUtf8().constData();
	if (nb_string.empty() || colorStr.empty()) {
		reject();
		return;
	}

	setColor(stringToColor(colorStr));
	setNb(std::stoi(nb_string));

	accept();
}
