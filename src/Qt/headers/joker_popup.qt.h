#ifndef QT_POPUP_JOKER_H
#define QT_POPUP_JOKER_H

#include "card.h"
#include "game.h"
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class popupJoker : public QDialog {
	Q_OBJECT

  public:
	explicit popupJoker(QWidget *parent = nullptr);

	const colorBonus getColor() const { return c; }
	void setColor(const colorBonus &bonus) { c = bonus; }

  private slots:
	void onSubmitClicked();

  private:
	QComboBox *comboBox;

	colorBonus c;

	QPushButton *submitButton;
};

#endif // QT_POPUP_JOKER_H
