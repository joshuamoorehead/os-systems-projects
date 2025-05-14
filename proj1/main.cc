// Written by Joshua Moorehead
// 2/5/23

#include "calculator.cc"


int main(int argc, char** argv) { //takes in command line arguments

    stack<char> operatorsStack;//need stacks to store args
    stack<double> numbersStack;
    
    populateStacks(argc, argv, numbersStack, operatorsStack); //separate arguments that are stored in arrays to number stack and operator stack

    double result = calculateAll(numbersStack, operatorsStack);//pop off stacks and calculate each operators result and end value

    cout << result << endl;//prints to cout

    return 0;
}