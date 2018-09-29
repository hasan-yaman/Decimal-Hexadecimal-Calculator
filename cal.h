#ifndef CAL_H
#define CAL_H

#include <QDialog>

class QLineEdit;
class QPushButton;
class QButtonGroup;
class QGridLayout;

class Cal : public QDialog 
{
	Q_OBJECT

public:
	Cal(QWidget *parent = 0);

private:
	QLineEdit *display;
	QPushButton *button[16];	// Holds the number buttons
	QButtonGroup *buttonGroup;
	QGridLayout *layout;
	QPushButton *multButton;
	QPushButton *divButton;
	QPushButton *addButton;
	QPushButton *subsButton;
	QPushButton *hexModeButton;
	QPushButton *decModeButton;
	QPushButton *clearButton;
	QPushButton *equalButton;
	QStringList strList;		// Holds the A-F for number buttons.
	

	int sumSoFar;
	int factorSoFar;
	// to keep track of previous pressed operator.
	enum Operators {
		ADD,
		SUB,
		MULT,
		DIV,
		NONE
	};
	Operators prevAddSub;
	Operators prevMultDiv;
	
	bool waitingForOperand;
	
	bool isDecimalMode;
	bool afterEqualPressed;

private slots:
	void numberClicked(int i);
	void addOrSubClicked();
	void multOrDivClicked();
	void equalButtonClicked();
	void clearButtonClicked();
	bool calculate(int number,Operators oper);
	void raiseError();
	void modeButtonClicked();
};
#endif
