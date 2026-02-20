#include "joker_popup.qt.h"
#include "game.h"
#include "main_window.qt.h"
#include <QLabel>
#include <string>

popupJoker::popupJoker(QWidget *parent) : QDialog(parent) {

	QLabel *typeLabel = new QLabel("Color:", this);
	comboBox = new QComboBox(this);

	int bonus_white =
	    Game::getGame().getCurrentPlayer().calculateBonus(colorBonus::white);
	int bonus_blue =
	    Game::getGame().getCurrentPlayer().calculateBonus(colorBonus::blue);
	int bonus_red =
	    Game::getGame().getCurrentPlayer().calculateBonus(colorBonus::red);
	int bonus_green =
	    Game::getGame().getCurrentPlayer().calculateBonus(colorBonus::green);
	int bonus_black =
	    Game::getGame().getCurrentPlayer().calculateBonus(colorBonus::black);

	if (bonus_white > 0) {
		comboBox->addItem("White");
	}
	if (bonus_blue > 0) {
		comboBox->addItem("Blue");
	}
	if (bonus_red > 0) {
		comboBox->addItem("Red");
	}
	if (bonus_green > 0) {
		comboBox->addItem("Green");
	}
	if (bonus_black > 0) {
		comboBox->addItem("Black");
	}

	submitButton = new QPushButton("Submit", this);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(typeLabel);
	layout->addWidget(comboBox);

	setWindowTitle("Color choice");

	connect(submitButton, &QPushButton::clicked, this,
	        &popupJoker::onSubmitClicked);
}

void popupJoker::onSubmitClicked() {
	std::string colorStr = comboBox->currentText().toUtf8().constData();
	setColor(stringToBonus(colorStr));

	accept();
}
