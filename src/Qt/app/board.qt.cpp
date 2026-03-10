#include "board.qt.h"
#include "main_window.qt.h"
#include "token_view.qt.h"

namespace {

constexpr double kBoardScale = 0.75;
const QSize kBoardSize(371 * kBoardScale, 446 * kBoardScale);
const QSize kTokenSize(64 * kBoardScale, 64 * kBoardScale);
const QSize kPrivilegeSize(60 * kBoardScale, 60 * kBoardScale);

Qt_token *createTransparentToken(QWidget *parent, const QSize &size) {
	Qt_token *token = new Qt_token(parent);
	token->setFixedSize(size);
	token->setStyleSheet("background: transparent;");
	return token;
}

} // namespace

Qt_Board::Handler Qt_Board::handler;

Qt_Board::~Qt_Board() {
	// Nothing
}

Qt_Board::Qt_Board(QWidget *parent) : QWidget(parent) {
	layout = new QGridLayout(this);

	layout->setHorizontalSpacing(7);
	layout->setVerticalSpacing(18);

	for (int i = 0; i < NTOKENS; ++i) {
		Qt_token *token = createTransparentToken(this, kTokenSize);
		token->setIndice(i);
		layout->addWidget(token, i / 5, i % 5);
		tokens.push_back(token);
	}

	privilegesLayout = new QGridLayout(this);
	privilegesLayout->setHorizontalSpacing(4);

	privilege1 = createTransparentToken(this, kPrivilegeSize);
	privilege2 = createTransparentToken(this, kPrivilegeSize);
	privilege3 = createTransparentToken(this, kPrivilegeSize);

	privilegesLayout->addWidget(privilege1, 0, 0); // Row 0, Column 0
	privilegesLayout->addWidget(privilege2, 0, 1); // Row 0, Column 1
	privilegesLayout->addWidget(privilege3, 0, 2); // Row 0, Column 2

	layout->addLayout(privilegesLayout, 5, 1, 1, 3);

	for (int i = 0; i < 5; ++i) {
		layout->setRowStretch(i, 0);
		layout->setColumnStretch(i, 0);
	}

	setFixedSize(kBoardSize);
}

void Qt_Board::paintEvent(QPaintEvent *event) {
	QPainter painter(this);

	QPixmap backgroundPixmap("../src/assets/rest_detoured/Board.png");
	QPixmap stretchedPixmap = backgroundPixmap.scaled(
	    kBoardSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	int x = (this->width() - stretchedPixmap.width()) / 2;
	int y = (this->height() - stretchedPixmap.height()) / 2;

	painter.drawPixmap(x, y, stretchedPixmap);

	QWidget::paintEvent(event);
}

void Qt_Board::connectTokens() {
	for (Qt_token *token : tokens) {
		connect(token, &Qt_token::tokenClicked, &MainWindow::getMainWindow(),
		        &MainWindow::tokenClicked);
	}
}
