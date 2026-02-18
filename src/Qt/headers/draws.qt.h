#ifndef QT_DRAWS_H
#define QT_DRAWS_H

#include "card_view.qt.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QVector>
#include <QWidget>

class Qt_Draws : public QWidget {
	Q_OBJECT

  public:
	explicit Qt_Draws(QWidget *parent = nullptr);
	std::vector<Qt_card *> &getTier1() { return tier1Cards; }
	std::vector<Qt_card *> &getTier2() { return tier2Cards; }
	std::vector<Qt_card *> &getTier3() { return tier3Cards; }
	std::vector<Qt_card *> &getRoyalCards() { return royalCards; }

	Qt_card *getDeck1() { return deck1; }
	Qt_card *getDeck2() { return deck2; }
	Qt_card *getDeck3() { return deck3; }

	void connectCards();

  private:
	QHBoxLayout *tier1Layout;
	QHBoxLayout *tier2Layout;
	QHBoxLayout *tier3Layout;
	QHBoxLayout *royalLayout;

	std::vector<Qt_card *> tier1Cards;
	std::vector<Qt_card *> tier2Cards;
	std::vector<Qt_card *> tier3Cards;
	std::vector<Qt_card *> royalCards;

	Qt_card *deck1;
	Qt_card *deck2;
	Qt_card *deck3;

	Qt_card *royalCardsImage;

	void setupTierLayout(QHBoxLayout *&layout, std::vector<Qt_card *> &cards,
	                     int cardCount, Qt_card *deckImage);

  signals:
	void clicked();

  protected:
	void mousePressEvent(QMouseEvent *event) override { emit clicked(); }
};

#endif // QT_DRAWS_H
