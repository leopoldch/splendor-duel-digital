#ifndef QT_POPUP_YESNO_H
#define QT_POPUP_YESNO_H

#include <QDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

class popupYesNo : public QDialog {
	Q_OBJECT

  private:
	char userChoice;

  public:
	popupYesNo(QWidget *parent = nullptr, const std::string &info = "")
	    : QDialog(parent) {
		QVBoxLayout *layout = new QVBoxLayout(this);

		if (info != "") {
			QLabel *infolabel = new QLabel(QString::fromStdString(info), this);
			layout->addWidget(infolabel);
		}

		QLabel *text = new QLabel("Confirm your choice");
		layout->addWidget(text);

		QPushButton *yesBtn = new QPushButton("Yes", this);
		connect(yesBtn, &QPushButton::clicked, this,
		        [this]() { onChoiceMade('Y'); });
		layout->addWidget(yesBtn);

		QPushButton *noBtn = new QPushButton("No", this);
		connect(noBtn, &QPushButton::clicked, this,
		        [this]() { onChoiceMade('N'); });
		layout->addWidget(noBtn);
	}

	int getUserChoice() const { return userChoice; }

  private slots:
	void onChoiceMade(char choice) {
		userChoice = choice;
		accept();
	}
};

#endif // QT_POPUP_YESNO_H
