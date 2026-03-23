#ifndef POPUP_TEXT_H
#define POPUP_TEXT_H

#include "view_bridge.h"
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class InputPopup : public QDialog {
	Q_OBJECT

  public:
	explicit InputPopup(QWidget *parent = nullptr);
	UiBridge::PlayerSetupData getPlayerSetupData() const;

  signals:
	void inputSubmitted(const QString &input);

  private slots:
	void onSubmitClicked();

  private:
	QLineEdit *lineEdit1;
	QLineEdit *lineEdit2;

	QComboBox *comboBox1;
	QComboBox *comboBox2;

	QPushButton *submitButton;
};

#endif // POPUP_TEXT_H
