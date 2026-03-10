#ifndef QT_CHOICEPOPUP_H
#define QT_CHOICEPOPUP_H

#include <QDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <utility>
#include <vector>

class ChoiceDialog : public QDialog {
	Q_OBJECT

  private:
	int userChoice = -1;

  public:
	ChoiceDialog(const std::vector<std::pair<int, QString>> &options,
	             QWidget *parent = nullptr)
	    : QDialog(parent) {
		QVBoxLayout *layout = new QVBoxLayout(this);

		QLabel *text = new QLabel("Choice");
		text->setAlignment(Qt::AlignCenter);
		layout->addWidget(text);

		for (const auto &option : options) {
			QPushButton *btn = new QPushButton(option.second, this);
			connect(btn, &QPushButton::clicked, this,
			        [this, value = option.first]() { onChoiceMade(value); });
			layout->addWidget(btn);
		}
	}

	int getUserChoice() const { return userChoice; }

  private slots:
	void onChoiceMade(int choice) {
		userChoice = choice;
		accept();
	}
};

#endif // QT_CHOICEPOPUP_H
