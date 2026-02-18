#include "player_input_dialog.qt.h"

PlayerInputDialog::PlayerInputDialog(QWidget *parent) : QDialog(parent) {}

void PlayerInputDialog::onSubmitClicked() {
	emit inputValuesSubmitted(name1Edit->text(), name2Edit->text(),
	                          choice1Edit->text(), choice2Edit->text());
	close();
}
