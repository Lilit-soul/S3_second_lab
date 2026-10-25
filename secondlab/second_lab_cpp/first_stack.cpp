#include <iostream>
#include <string>

using namespace std;

struct StackNode {
    int data;
    StackNode* next;
};

struct MyStack {
    StackNode* topNode;
};

void initStack(MyStack& s) { s.topNode = nullptr; }
bool isEmpty(MyStack& s) { return s.topNode == nullptr; }

void push(MyStack& s, int val) {
    StackNode* newNode = new StackNode;
    newNode->data = val;
    newNode->next = s.topNode;
    s.topNode = newNode;
}

int pop(MyStack& s) {
    if (isEmpty(s)) return -1;
    int val = s.topNode->data;
    StackNode* temp = s.topNode;
    s.topNode = s.topNode->next;
    delete temp;
    return val;
}

int top(MyStack& s) {
    if (isEmpty(s)) return -1;
    return s.topNode->data;
}

void clearStack(MyStack& s) {
    while (!isEmpty(s)) pop(s);
}

int applyOp(char op, int a, int b = 0) {
    if (op == '&') return a & b;
    if (op == '|') return a | b;
    if (op == '^') return a ^ b;
    if (op == '!') return !a;
    return 0;
}

int precedence(char op) {
    if (op == '!') return 3;
    if (op == '&') return 2;
    if (op == '|' || op == '^') return 1;
    return 0;
}

bool evaluateExpression(const string& expr, int& result) {
    MyStack values, ops;
    initStack(values);
    initStack(ops);
    
    bool expectOperand = true;

    for (int i = 0; i < expr.length(); i++) {
        if (expr[i] == ' ') continue;

        if (isdigit(expr[i])) {
        int num = 0;
        while (i < expr.length() && isdigit(expr[i])) {
            num = num * 10 + (expr[i] - '0');
            i++;
        }
        i--;
        
        if (num != 0 && num != 1) {
            cout << "Error: operands must be 0 or 1" << endl;
            clearStack(values); clearStack(ops);
            return false;
        }
        
        if (!expectOperand) {
            cout << "Error: missing operator between operands" << endl;
            clearStack(values); clearStack(ops);
            return false;
        }
        push(values, num);
        expectOperand = false;
        }
        else if (expr[i] == '(') {
            if (!expectOperand) {
                cout << "Error: missing operator before '('" << endl;
                clearStack(values); clearStack(ops);
                return false;
            }
            push(ops, expr[i]);
        } 
        else if (expr[i] == ')') {
            if (expectOperand) {
                cout << "Error: missing operand before ')'" << endl;
                clearStack(values); clearStack(ops);
                return false;
            }
            while (!isEmpty(ops) && top(ops) != '(') {
                char op = pop(ops);
                if (isEmpty(values) || values.topNode->next == nullptr) {
                    cout << "Error: not enough operands" << endl;
                    clearStack(values); clearStack(ops);
                    return false;
                }
                int val2 = pop(values), val1 = pop(values);
                push(values, applyOp(op, val1, val2));
            }
            if (isEmpty(ops)) {
                cout << "Error: mismatched parentheses" << endl;
                clearStack(values); clearStack(ops);
                return false;
            }
            pop(ops);
            expectOperand = false;
        } 
        else if (expr[i] == '&' || expr[i] == '|' || expr[i] == '^') {
            if (expectOperand) { 
                cout << "Error: missing operand before operator" << endl;
                clearStack(values); clearStack(ops);
                return false;
            }
            while (!isEmpty(ops) && precedence(top(ops)) >= precedence(expr[i])) {
                char op = pop(ops);
                if (isEmpty(values) || values.topNode->next == nullptr) {
                    cout << "Error: not enough operands" << endl;
                    clearStack(values); clearStack(ops);
                    return false;
                }
                int val2 = pop(values), val1 = pop(values);
                push(values, applyOp(op, val1, val2));
            }
            push(ops, expr[i]);
            expectOperand = true;
        } 
        else if (expr[i] == '!') {
            if (!expectOperand) {
                cout << "Error: missing operator before '!'" << endl;
                clearStack(values); clearStack(ops);
                return false;
            }
            while (!isEmpty(ops) && precedence(top(ops)) >= precedence(expr[i])) {
                char op = pop(ops);
                if (op == '!') {
                    if (isEmpty(values)) {
                        cout << "Error: not enough operands" << endl;
                        clearStack(values); clearStack(ops);
                        return false;
                    }
                    push(values, applyOp(op, pop(values)));
                } else {
                    if (isEmpty(values) || values.topNode->next == nullptr) {
                        cout << "Error: not enough operands" << endl;
                        clearStack(values); clearStack(ops);
                        return false;
                    }
                    int val2 = pop(values), val1 = pop(values);
                    push(values, applyOp(op, val1, val2));
                }
            }
            push(ops, expr[i]);
        } 
        else {
            cout << "Error: invalid character" << endl;
            clearStack(values); clearStack(ops);
            return false;
        }
    }

    if (expectOperand) {
        cout << "Error: missing operand at the end" << endl;
        clearStack(values); clearStack(ops);
        return false;
    }

    while (!isEmpty(ops)) {
        char op = pop(ops);
        if (op == '(') {
            cout << "Error: mismatched parentheses" << endl;
            clearStack(values); clearStack(ops);
            return false;
        }
        if (op == '!') {
            if (isEmpty(values)) {
                cout << "Error: not enough operands" << endl;
                clearStack(values); clearStack(ops);
                return false;
            }
            push(values, applyOp(op, pop(values)));
        } else {
            if (isEmpty(values) || values.topNode->next == nullptr) {
                cout << "Error: not enough operands" << endl;
                clearStack(values); clearStack(ops);
                return false;
            }
            int val2 = pop(values), val1 = pop(values);
            push(values, applyOp(op, val1, val2));
        }
    }
    
    result = pop(values);
    if (!isEmpty(values)) {
        cout << "Error: too many operands" << endl;
        clearStack(values); clearStack(ops);
        return false;
    }
    
    clearStack(values); clearStack(ops);
    return true;
}

int main() {
    string expr;
    cout << "Введите логическое выражение (0, 1, &, |, ^, !): ";
    getline(cin, expr);
    
    bool hasDigit = false;
    for (char c : expr) {
        if (c == '0' || c == '1') hasDigit = true;
    }
    
    if (expr.empty() || !hasDigit) {
        cout << "Error: empty or invalid expression" << endl;
        return 1;
    }
    
    int result;
    if (evaluateExpression(expr, result)) {
        cout << "Результат: " << result << endl;
    }
    return 0;
}