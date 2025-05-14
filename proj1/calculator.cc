//Written by Joshua Moorehead Feb 5 2023

#include "calculator.h"


void populateStacks(int argc, char** argv, stack<double> &numbersStack, stack<char> &operatorsStack) {
    
    for (int i = 1; i < argc; ++i) //args are stored in arrays
    {
        if (atof(argv[i])) //use atof function to see if its a num

            numbersStack.push(atof(argv[i]));//store if num
        else 
        {
            if (argv[i][0] == 'x'|| argv[i][0] == '/') //order of operations pushes mult and div first
            { 
                operatorsStack.push(argv[i][0]);
            } 
            else if (argv[i][0] == '+'|| argv[i][0] == '-') //if true, need to check if mult or div is on top of stack
            {
                if (!operatorsStack.empty() && (operatorsStack.top() == 'x' || operatorsStack.top() == '/')) 
                {
                    char op = operatorsStack.top();
                    operatorsStack.pop();
                    double val1 = numbersStack.top();
                    numbersStack.pop();
                    double val2 = numbersStack.top();
                    numbersStack.pop();
                  
                    numbersStack.push(doMath(op, val1, val2));//pushing mult or div result to numbers stack
                }

                operatorsStack.push(argv[i][0]);
            }
        }
    }
}


double doMath(char op, double value1, double value2) {//call while we pop off stack
    
    double result;

    if (op == '+') 
        result = value2 + value1;
    else if (op == '-') 
        result = value2 - value1;
    else if (op == 'x') 
        result = value2 * value1;
    else if (op == '/') 
        result = value2 / value1;

    return result;
}

double calculateAll(stack<double> &numbersStack, stack<char> &operatorsStack) {
    
    double result = 0;
    while (!numbersStack.empty()) 
    {
        char op = operatorsStack.top();
        operatorsStack.pop();
        double val1 = numbersStack.top();
        numbersStack.pop();
        double val2 = numbersStack.top();
        numbersStack.pop();
        
        result = doMath(op, val1, val2);

        if (!numbersStack.empty())
            numbersStack.push(result);
    }

    return result;
}