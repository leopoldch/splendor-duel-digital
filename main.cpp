#include "main.h"
#include "game.h"
#include "info_popup.qt.h"
#include "main_window.qt.h"
#include "player.h"
#include "yesno_popup.qt.h"
#include <QApplication>
#include <QMainWindow>
#include <QString>
#include <QVBoxLayout>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	InfoDialog info_dialog("Welcome to Splendor Duel!");
	info_dialog.exec();

	popupYesNo yesno(nullptr, "Do you want to resume the last game in progress (Yes/No)?");
	yesno.exec();
	char tmp = yesno.getUserChoice();

	if (tmp == 'Y') {
		gameFromJson(argc, argv);
	} else {
		gameFromScratch(argc, argv);
	}

	Game::free();

	char tmp2 = 'Y';
	while (tmp2 == 'Y') {

		popupYesNo relaunch(nullptr,
		                    "Do you want to replay (Yes) or close (No)?");
		relaunch.exec();
		try {
			tmp2 = relaunch.getUserChoice();
			if (tmp2 == 'Y') {
				gameFromScratch(argc, argv);
				Game::free();
			}
		} catch (SplendorException &e) {
			MainWindow::getMainWindow().triggerInfo(e.getInfo());
		}
	}

	History::freeHistory();
	return app.exec();
}
