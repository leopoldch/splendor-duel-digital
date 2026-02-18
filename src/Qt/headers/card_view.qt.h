#ifndef QT_VUE_CARTE_H
#define QT_VUE_CARTE_H

#include "card.h"
#include <QBrush>
#include <QPainter>
#include <QPen>
#include <QPushButton>
#include <QResizeEvent>
#include <QStaticText>
#include <QString>
#include <QWidget>

class Qt_card : public QPushButton {
	Q_OBJECT

  private:
	bool isClicked; // Track if the card was clicked
	const Card *card;
	QPixmap m_image;
	int index_in_draw;
	bool is_reserved;

  public:
	const int getIndex() const { return index_in_draw; }
	void setIndice(int x) { index_in_draw = x; }

	const bool getReserved() const { return is_reserved; }
	void setReserved(bool x) { is_reserved = x; }

	explicit Qt_card(QWidget *parent = nullptr);

	void toggleClicked();
	void updateAppearance();
	void updateAppearance(const std::string &string);

	const Card *getCard() const { return card; }
	void setCard(const Card *j) { card = j; }

  protected:
	void paintEvent(QPaintEvent *event) override;

  signals:
	void cardClicked(Qt_card *c);

  public slots:
	void clickedEvent() { emit cardClicked(this); }
};

#endif // QT_VUE_CARTE_H
