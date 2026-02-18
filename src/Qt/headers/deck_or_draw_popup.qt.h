#ifndef QT_POPUP_DRAW_OR_DECK_H
#define QT_POPUP_DRAW_OR_DECK_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class DrawDeckPopup : public QDialog {
	Q_OBJECT

  private:
	char userChoice;

  public:
	DrawDeckPopup(QWidget *parent = nullptr) : QDialog(parent) {
		QVBoxLayout *layout = new QVBoxLayout(this);

		QPushButton *draw = new QPushButton("Draw", this);
		connect(draw, &QPushButton::clicked, this,
		        [this]() { onChoiceMade('T'); });
		layout->addWidget(draw);

		QPushButton *deck = new QPushButton("Deck", this);
		connect(deck, &QPushButton::clicked, this,
		        [this]() { onChoiceMade('P'); });
		layout->addWidget(deck);
	}

	int getUserChoice() const { return userChoice; }

  private slots:
	void onChoiceMade(char choice) {
		userChoice = choice;
		accept();
	}
};

#endif // QT_POPUP_DRAW_OR_DECK_H
