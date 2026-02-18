#ifndef PLAYERINPUTDIALOG_H
#define PLAYERINPUTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QString>

class PlayerInputDialog : public QDialog {
	Q_OBJECT

  public:
	PlayerInputDialog(QWidget *parent = nullptr);

  signals:
	void inputValuesSubmitted(QString name1, QString name2,
	                          QString user_player_choice1,
	                          QString user_player_choice2);

  private slots:
	void onSubmitClicked();

  private:
	QLineEdit *name1Edit;
	QLineEdit *name2Edit;
	QLineEdit *choice1Edit;
	QLineEdit *choice2Edit;
	QPushButton *submitButton;
	QPushButton *cancelButton;
};

#endif // PLAYERINPUTDIALOG_H
