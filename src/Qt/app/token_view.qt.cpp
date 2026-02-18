#include "token_view.qt.h"
#include "main_window.qt.h"
#include <QCoreApplication>

Qt_token::Qt_token(QWidget *parent)
    : QPushButton(parent), token(nullptr), isClicked(false) {
	connect(this, SIGNAL(clicked()), this, SLOT(clickedEvent()));
}

void Qt_token::toggleClicked() {
	isClicked = !isClicked;
	update();
}

void Qt_token::paintEvent(QPaintEvent *event) {
	QPushButton::paintEvent(event);

	QPainter painter(this);
	if (isDown()) {
		painter.fillRect(rect(), QColor(128, 128, 128, 128));
	}
}

void Qt_token::updateAppearance() {
	if (token) {
		QIcon icon(QPixmap(QString::fromStdString(token->getVisual())));
		this->setIcon(icon);
		this->setIconSize(this->size());
	} else {
		this->setIcon(QIcon());
		this->setIconSize(this->size());
	}
	this->update();
}
