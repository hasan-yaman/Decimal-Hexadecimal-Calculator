#include <QtGui>
#include "cal.h"

Cal::Cal(QWidget *parent) : QDialog(parent)
{
	// initializon of variables, default mode is decimal mode
	// and we don't have any previous operator(NONE).
	
	sumSoFar = 0;
	factorSoFar = 0;
	waitingForOperand = true;

	prevAddSub = NONE;
	prevMultDiv = NONE;

	isDecimalMode = true;
	afterEqualPressed = false;

	// create QLineEdit object for display
	// to prevent editing from user set read only.
	display = new QLineEdit("0");
	display->setReadOnly(true);
	display->setAlignment(Qt::AlignRight);
	
	buttonGroup = new QButtonGroup;
	layout = new QGridLayout;
	
	// create calculator buttons
	multButton = new QPushButton("*");
	divButton = new QPushButton("/");
	addButton = new QPushButton("+");	
	subsButton = new QPushButton("-");

	hexModeButton = new QPushButton("Hex");
	decModeButton = new QPushButton("Dec");
	
	// Remember, default mode is decimal mode
	// and set the background of mode that is selected
	// orange.
	decModeButton->setStyleSheet("background: orange");
	hexModeButton->setStyleSheet("background: white");

	clearButton = new QPushButton("Clear");
	equalButton = new QPushButton("=");

	// arrange the layout of buttons

	layout->addWidget(display,0,0,1,-1);

	layout->addWidget(multButton,1,0,1,2);
	layout->addWidget(divButton,1,2,1,2);
	layout->addWidget(addButton,2,0,1,2);
	layout->addWidget(subsButton,2,2,1,2);

	layout->addWidget(hexModeButton,1,4,1,1);
	layout->addWidget(decModeButton,2,4,1,1);

	layout->addWidget(clearButton,3,4,2,1);
	layout->addWidget(equalButton,5,4,2,1);

	layout->setSizeConstraint(QLayout::SetFixedSize);

	// temporary variables for keep track of contents of numbers button.
	int tempNumber = 9;
	
	int strIndex = 0;

	strList.append("A");
	strList.append("B");
	strList.append("C");
	strList.append("D");
	strList.append("E");
	strList.append("F");

	// Create number buttons
	for(int i = 0; i<4; i++) {
		for(int j = 0; j<4; j++) {
			int index = i * 4 + j;
			if(tempNumber >= 0) {
				QString s = QString::number(tempNumber);
				button[index] = new QPushButton(s);
				layout->addWidget(button[index],i+3,j,1,1);
				buttonGroup->addButton(button[index],tempNumber);
				tempNumber--;
			} else {
				QString s = strList.at(strIndex);
				button[index] = new QPushButton(s);
				// Again, default mode is decimal mode, so A-F buttons
				// should not be enabled.
				button[index]->setEnabled(false);
				layout->addWidget(button[index],i+3,j,1,1);
				buttonGroup->addButton(button[index],strIndex+10);
				strIndex++;
			}
		}
	}

	// make connection between signals and slots
	
	connect(buttonGroup,SIGNAL(buttonClicked(int)),this,SLOT(numberClicked(int)));
	connect(addButton,SIGNAL(clicked()),this,SLOT(addOrSubClicked()));
	connect(subsButton,SIGNAL(clicked()),this,SLOT(addOrSubClicked()));
	connect(multButton,SIGNAL(clicked()),this,SLOT(multOrDivClicked()));
	connect(divButton,SIGNAL(clicked()),this,SLOT(multOrDivClicked()));
	connect(equalButton,SIGNAL(clicked()),this,SLOT(equalButtonClicked()));
	connect(clearButton,SIGNAL(clicked()),this,SLOT(clearButtonClicked())); 
	connect(hexModeButton,SIGNAL(clicked()),this,SLOT(modeButtonClicked()));
	connect(decModeButton,SIGNAL(clicked()),this,SLOT(modeButtonClicked()));

	setLayout(layout);
}
// Called when number buttons(0-9 and A-F) are clicked.
// i is the value of the number button.(A-> 10 B->11 etc.)
void Cal::numberClicked(int i){
	if(display->text() == "0" && i == 0) {
		return;
	}
	// After equal button pressed, if we receive number input from user
	// we should clear the display screen.
	if(afterEqualPressed){
		display->clear();
		afterEqualPressed = false;
	}
	// waitingForOperand is true when user is pressed any of the operator button.
	// Therefore, clear the display and wait for next operand.
	if(waitingForOperand) {
		display->clear();
		waitingForOperand = false;
	}
	// If number's value is greater that 10, means we are in the hexadecimal mode
	// we should display numbers in hexadecimal format.
	if(i<10){
		display->setText(display->text() + QString::number(i));
	} else {
		display->setText(display->text() + strList.at(i-10));
	}
}
// Called when user clicked addition or subtraction button.
void Cal::addOrSubClicked(){
	// If calculator waits for operand, and user still try
	// to enter a input, just do nothing.
	if(waitingForOperand){
		return;
	}
	// Find clicked button
	QPushButton *clicked = qobject_cast<QPushButton *>(sender());
	
	// integer variable that holds the number that in the 
	// screen before the button is pressed.
	int number; 
	if(isDecimalMode){
		number = display->text().toInt();
	} else {
		bool ok;
		number = display->text().toLower().toInt(&ok,16);
	}
	
	// Firstly, take care of multiplication or division if we have any, before the
	// continue with the addition or subtraction.
	if(prevMultDiv != NONE){
		//calculates previous multiplication or/and division
		if(!calculate(number, prevMultDiv)){
			raiseError();
			return;
		}
		
		// display according to selected mode.
		if(isDecimalMode){
			display->setText(QString::number(factorSoFar));
		} else {
			display->setText(QString::number(factorSoFar,16));
		}
		
		
		number = factorSoFar;
		factorSoFar = 0;
		prevMultDiv = NONE;
	}

	// Then take care of addition or/and subtraction.
	if(prevAddSub != NONE){
		if(!calculate(number, prevAddSub)){
			raiseError();
			return;
		}
		// Display time!
		if(isDecimalMode){
			display->setText(QString::number(sumSoFar));
		} else {
			display->setText(QString::number(sumSoFar,16));
		}

	} else {
		sumSoFar = number;
	}

	// set the current pressed button as a
	// previous button.
	QString clickedButton;
	if(clicked->text() == "+") {
		prevAddSub = ADD;
	} else {
		prevAddSub = SUB;
	}

	// Finally, wait for the next operand if any.
	waitingForOperand = true;

}

// In this method, we have the same logic as the addOrSubClicked() method.
// Again, if calculator waits for operand and user try to press operator button, 
// do nothing.Then find the pressed button.If we have previous multiplication or 
// division operators do the operation.In this case, we don't have to check for
// the previous addition or subtraction because multiplication and division
// have the greater precedence.(It is also ensures that we have left to right precedence)
// Finally, set the current pressed button as a previos button.
void Cal::multOrDivClicked(){

	if(waitingForOperand){
		return;
	}

	QPushButton *clicked = qobject_cast<QPushButton *>(sender());
	
	int number;
	if(isDecimalMode){
		number = display->text().toInt();
	} else {
		bool ok;
		number = display->text().toLower().toInt(&ok,16);
	}
	
	if(prevMultDiv != NONE){
		if(!calculate(number, prevMultDiv)){
			raiseError();
			return;
		}
		if(isDecimalMode){
			display->setText(QString::number(factorSoFar));
		} else {
			display->setText(QString::number(factorSoFar,16));
		}
		
	} else {
		factorSoFar = number;
	}
	
	QString clickedButton;
	if(clicked->text() == "*") {
		prevMultDiv = MULT;
	} else {
		prevMultDiv = DIV;
	}

	waitingForOperand = true;
}
	
// Again, also this method similar to addOrSubClicked() method.Check if we have previous 
// operator, and displays the output.If calculator waits for operand and user try to press
// equal button, displays the same number.
void Cal::equalButtonClicked(){
	int number;
	if(isDecimalMode){
		number = display->text().toInt();
	} else {
		bool ok;
		number = display->text().toInt(&ok,16);
	}

	if(waitingForOperand){
		number = 0;
	}
	
	if(prevMultDiv != NONE){
		if(!calculate(number, prevMultDiv)){
			raiseError();
			return;
		}
		
		number = factorSoFar;
		factorSoFar = 0;
		prevMultDiv = NONE;
	}

	if(prevAddSub!= NONE){
		if(!calculate(number, prevAddSub)){
			raiseError();
			return;
		}
		
		prevAddSub = NONE;
	} else {
		sumSoFar = number;
	}
	
	if(isDecimalMode){
		display->setText(QString::number(sumSoFar));
	} else {
		display->setText(QString::number(sumSoFar,16).toUpper());
	}

	
	waitingForOperand = false;
	afterEqualPressed = true; // sets true so we can keep track of when user pressed equal button.	
}

//  Clears the output and returns all the variables to their initial values.
void Cal::clearButtonClicked() {
	
	display->setText("0");
	sumSoFar = 0;
	factorSoFar = 0;
	waitingForOperand = true;
	afterEqualPressed = false;	
}
// Do calculations according to given oper parameter, and returns true.
// Except, user try to divide any number to 0.In this case, returns false.
bool Cal::calculate(int number,Operators oper){
	if(oper == ADD) {
		sumSoFar += number;
	}else if(oper == SUB) {
		sumSoFar -= number;
	} else if(oper == MULT){
		factorSoFar *= number;
	} else if(oper == DIV){
		if(number == 0){
			return false;
		}
		factorSoFar /= number;
	}

	return true;
}

// returns all the variables to their initial values, and give error to the user.
void Cal::raiseError(){
	sumSoFar = 0;	
	factorSoFar = 0;
	prevMultDiv = NONE;
	prevAddSub = NONE;
	waitingForOperand = true;	
	display->setText("Error!");
}

// Called, when the user try to change the mode of the calculator.(Hex and Dec buttons)
void Cal::modeButtonClicked(){

	clearButtonClicked(); // Clear, then continue
	
	// Find the clicked button
	QPushButton *clicked = qobject_cast<QPushButton *>(sender());
	
	// And changed the background of the clicked button.
	// Also changed the mode of the calculator via setting isDecimalMode.
	if(clicked->text()== "Hex"){
		hexModeButton->setStyleSheet("background : orange");
		decModeButton->setStyleSheet("background : white");
		isDecimalMode = false;
	} else {
		hexModeButton->setStyleSheet("background : white");
		decModeButton->setStyleSheet("background : orange");
		isDecimalMode = true;
	}

	// If current mode is decimal mode, disabled the A-F button and vice versa.
	for(int i = 0; i<4; i++) {
		for(int j = 0; j<4; j++) {
			int index = i * 4 + j;
			if(index >= 10){
				button[index]->setEnabled(!isDecimalMode);
			}
		}
	}	
}









