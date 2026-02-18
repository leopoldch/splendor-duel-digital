#ifndef QT_VUE_JETON_H
#define QT_VUE_JETON_H

#include "tokens.h"
#include <QBrush>
#include <QPainter>
#include <QPen>
#include <QPushButton>
#include <QResizeEvent>
#include <QStaticText>
#include <QString>
#include <QWidget>

class Qt_token : public QPushButton {
	Q_OBJECT

  private:
	bool isClicked; // Track if the token was clicked
	const Token *token;
	int index;

  public:
	explicit Qt_token(QWidget *parent = nullptr);
	void toggleClicked();
	void updateAppearance(); // Update image

	const Token *getToken() const { return token; }
	void setToken(const Token *j) { token = j; }

	const int getIndex() const { return index; }
	void setIndice(int x) { index = x; }

  protected:
	void paintEvent(QPaintEvent *event) override;

  signals:
	void tokenClicked(Qt_token *j); // Signal to emit if the token was clicked

  private slots:
	void clickedEvent() { emit tokenClicked(this); }
};

#endif // QT_VUE_JETON_H
