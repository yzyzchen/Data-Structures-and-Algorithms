// Project identifier: 1CAEF3A0FEDD0DEC26BA9808C69D4D22A9962768

#include "evaluate.hpp"
#include <stack>
#include <cstdint>
#include <string>
#include <cctype>
#include <iostream>
using namespace std;

std::int64_t evaluate(std::string const& expression) 
{
    // Your implementation goes here

    std::stack<int64_t> operands;
    std::stack<char> operators;
    auto operations = [&]()
    {
        if (operands.size() < 2 || operators.size() < 1) return;
        //extract two operands
        int64_t rhs = operands.top(); operands.pop();
        int64_t lhs = operands.top(); operands.pop();
        //extract one operator
        char op = operators.top(); operators.pop();
        //analyize operator
        switch (op)
        {
            case '+': operands.push(lhs + rhs); break;
            case '-': operands.push(lhs - rhs); break;
            case '*': operands.push(lhs * rhs); break;
            case '/': operands.push(lhs / rhs); break;
            default: break;
        }
        //cout << lhs << op << rhs << " = " << operands.top() << endl;
    };
    for (char ip : expression)
    {
        if (isdigit(ip)) operands.push(ip - '0');
        else operators.push(ip);
        if(operators.size() > 0 && operands.size() > 1) operations();
    }
    return operands.top();
}
