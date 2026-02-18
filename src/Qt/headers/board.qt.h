#ifndef QT_BOARD_H
#define QT_BOARD_H

#include "token_view.qt.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

#define NTOKENS 25

class Qt_Board : public QWidget {
	Q_OBJECT;

  public:
	void connectTokens();

	const std::vector<Qt_token *> &getTokens() const { return tokens; }

	static Qt_Board &getBoard() {
		if (handler.instance == nullptr)
			handler.instance = new Qt_Board();
		return *handler.instance;
	}

	void freeBoard() {
		delete handler.instance;
		handler.instance = nullptr;
	}

	Qt_token *getPrivilege1() { return privilege1; }
	Qt_token *getPrivilege2() { return privilege2; }
	Qt_token *getPrivilege3() { return privilege3; }

  private:
	struct Handler {
		Qt_Board *instance = nullptr;
		~Handler() {
			delete instance;
			instance = nullptr;
		}
	};

	static Handler handler;
	Qt_Board(QWidget *parent = nullptr);
	~Qt_Board();

	Qt_Board(const Qt_Board &) = delete;
	Qt_Board &operator=(const Qt_Board &) = delete;

	QGridLayout *layout;
	QGridLayout *privilegesLayout;
	std::vector<Qt_token *> tokens;
	Qt_token *privilege1;
	Qt_token *privilege2;
	Qt_token *privilege3;

  signals:
	void clicked();

  protected:
	void mousePressEvent(QMouseEvent *event) override { emit clicked(); }
	void paintEvent(QPaintEvent *event) override;
};

#endif // QT_BOARD_H
