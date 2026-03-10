#include "joker_popup.qt.h"
#include <QLabel>
#include <string>

popupJoker::popupJoker(const UiBridge::JokerChoiceViewState &state,
                       QWidget *parent)
    : QDialog(parent), c(colorBonus::white) {

	QLabel *typeLabel = new QLabel("Color:", this);
	comboBox = new QComboBox(this);

	for (const auto &option : state.bonuses) {
		comboBox->addItem(QString::fromStdString(option.label));
	}

	submitButton = new QPushButton("Submit", this);
	submitButton->setEnabled(!state.bonuses.empty());

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(typeLabel);
	layout->addWidget(comboBox);
	layout->addWidget(submitButton);

	setWindowTitle("Color choice");

	connect(submitButton, &QPushButton::clicked, this,
	        &popupJoker::onSubmitClicked);
}

void popupJoker::onSubmitClicked() {
	std::string colorStr = comboBox->currentText().toUtf8().constData();
	if (colorStr.empty()) {
		reject();
		return;
	}
	setColor(stringToBonus(colorStr));

	accept();
}
