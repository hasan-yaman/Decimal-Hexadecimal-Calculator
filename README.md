# Project Overview
The calculator has two modes one for decimal calculations and the other for hexadecimal calculations. I assumed that all input will be non-negative integers and the results will be an integer. The division operation is to be taken as integer division.

# Getting Started
To run project(Ubuntu 16.04), install libraries:
```
sudo apt-get install libqt4-dev
sudo apt-get install libqt5-dev
```
then run:
```
qmake -project 
qmake *.pro 
make
```
**(Put all the files to a directory, there should be no other non-project related files/codes there)**

# Built with
[Qt](https://www.qt.io)

# Implementation Details
- The default mode is decimal, and the selected mode has an orange background. Changing mode resets the calculator.
- I assumed that precedence order: multiplication = division > addition = subtraction
- For the operators that have the same precedence, firstly I do the leftmost operation.
- After an equality button pressed if a user press any of the number buttons (0-9 and A-Z) calculator acts like a new process is just begin.Also, if user press any of the operation button(+,-,*,/) calculator acts like a user wants to make changes on the current output.
