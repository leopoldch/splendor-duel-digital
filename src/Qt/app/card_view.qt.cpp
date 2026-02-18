#include "card_view.qt.h"
#include <QDebug>

Qt_card::Qt_card(QWidget *parent)
    : QPushButton(parent), isClicked(false), card(nullptr) {
	connect(this, SIGNAL(clicked()), this, SLOT(clickedEvent()));
}

void Qt_card::paintEvent(QPaintEvent *event) {
	QPushButton::paintEvent(event); // Call base class paint event

	QPainter painter(this);
	if (isDown()) {
		painter.fillRect(rect(), QColor(128, 128, 128, 128)); // Grey out
	}

	painter.setRenderHint(QPainter::Antialiasing);

	painter.setBrush(Qt::transparent);
}

void Qt_card::updateAppearance() {
	if (card != nullptr) {
		qDebug() << card->getVisual();
		QIcon icon(QPixmap(QString::fromStdString(card->getVisual())));
		this->setIcon(icon);
		this->setIconSize(this->size());
	}

	else {
		this->setIcon(QIcon());
		this->setIconSize(this->size());
		qDebug() << "prblm";
	}
}

void Qt_card::updateAppearance(const std::string &string) {
	m_image = QPixmap(QString::fromStdString(string));
	setIcon(QIcon(m_image));
	setIconSize(size());
	update();
}

void Qt_card::toggleClicked() {
	isClicked = !isClicked;
	update();
}
