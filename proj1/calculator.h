// Written by Joshua Moorehead Feb 5 2023
#ifndef _PROJ1_CALCULATOR_H_
#define _PROJ1_CALCULATOR_H_

#include <iostream>
#include <stack>
using namespace std;

void populateStacks(int argc, char** argv, stack<double> &numbersStack, stack<char> &operatorsStack);
double doMath(char op, double value1, double value2);
double calculateAll(stack<double> &numbersStack, stack<char> &operatorsStack);


#endif 
