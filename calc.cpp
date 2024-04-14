#include "calc.hpp"
#pragma warning(disable:4996)

#include <cstdio>

Calculator::Calculator(uint32_t lines_amt) : commands_amt_(lines_amt) {
    commands_ = new List<String>[lines_amt];
}

Calculator::~Calculator() { delete[] commands_; }

void Calculator::Process() {
    GetInput();
    ToRPN();
    ProcessRPN();
}

void Calculator::GetInput() {
    char token[10];

    for (uint32_t i = 0; i < commands_amt_; ++i) {
        while (true) {
            scanf("%s", token);

            if (token[0] == '.') {
                break;
            }

            commands_[i].push_back(token);
        }
    }
}

String StringAsInt(int32_t num) {
    if (num == 0) {
        return "0";
    }

    // max int is 10 chars + sign
    char result[12];
    uint32_t back_idx = 0;
    uint32_t shift = 0;

    if (num < 0) {
        result[back_idx++] = '-';
        shift = 1;
        num = -num;
    }

    while (num > 0) {
        result[back_idx++] = num % 10 + '0';
        num /= 10;
    }

    for (uint32_t i = shift; i < (back_idx - shift) / 2 + shift; ++i) { 
        result[i] ^= result[back_idx + shift - i - 1];                           
        result[back_idx + shift - i - 1] ^= result[i];
        result[i] ^= result[back_idx + shift - i - 1];
    }

    result[back_idx] = '\0';

    return result;
}

void Calculator::ToRPN() {
    for (uint32_t i = 0; i < commands_amt_; ++i) {
        Stack<String> op_stack;
        List<String> rpn;

        // auto == String
        for (const auto& token : commands_[i]) {
            if (IsNumber(token)) {
                rpn.push_back(token);
                continue;
            }

            if (IsFunction(token)) {
                op_stack.push(token);
                continue;
            }

            if (IsOperator(token)) {
                while (!op_stack.empty() &&
                    GetPriority(token) <= GetPriority(op_stack.top()) &&
                    op_stack.top() != "(") {
                    if (op_stack.top() != ",") {
                        rpn.push_back(op_stack.top());
                    }

                    op_stack.pop(); // getting from stack
                }
                op_stack.push(token);

                continue;
            }

            if (token == "(") {
                op_stack.push(token);
                continue;
            }

            if (token == ")") {
                uint32_t popped_amt = 0;

                while (!op_stack.empty() && op_stack.top() != "(") {
                    if (op_stack.top() != ",") {
                        rpn.push_back(op_stack.top()); // getting from stack
                    }
                    else {
                        ++popped_amt;
                    }

                    op_stack.pop();
                }

                if (op_stack.empty()) {
                    printf("Error: Unmatched parentheses\n");
                    return;
                }

                op_stack.pop();

                if (!op_stack.empty() && op_stack.top()[0] == 'M') {
                    String func = op_stack.top();
                    op_stack.pop();

                    op_stack.push(func + StringAsInt(popped_amt + 1)); //MAX(N)
                }

                continue;
            }

            if (token == ",") {
                uint32_t commas_popped_amt = 0;

                while (!op_stack.empty() && op_stack.top() != "(") { // MAX ( 1 , 6 * 5 + 2 , 2 )
                    if (op_stack.top() != ",") {
                        rpn.push_back(op_stack.top());
                    }
                    else {
                        ++commas_popped_amt;
                    }

                    op_stack.pop();
                }

                for (uint32_t j = 0; j <= commas_popped_amt; ++j) {
                    op_stack.push(token);
                }
            }
        }

        while (!op_stack.empty()) {
            rpn.push_back(op_stack.top());
            op_stack.pop();
        }

        commands_[i] = rpn;
    }
}

int32_t max(int32_t left_op, int32_t right_op) {
    return left_op > right_op ? left_op : right_op;
}

int32_t min(int32_t left_op, int32_t right_op) {
    return left_op < right_op ? left_op : right_op;
}

void Calculator::ProcessRPN() {
    for (uint32_t i = 0; i < commands_amt_; ++i) {
        for (const auto& token : commands_[i]) {
            printf("%s ", token.Data());
        }
        printf("\n");

        Stack<String> exec_stack;

        bool err = false;

        for (const auto& token : commands_[i]) {
            if (IsNumber(token)) {
                exec_stack.push(token);
                continue;
            }

            if (IsOperator(token)) {
                int32_t right_op = exec_stack.top().ParseInt32();
                exec_stack.pop();

                int32_t left_op = exec_stack.top().ParseInt32();
                exec_stack.pop();

                {
                    // print data
                    printf("%s %d %d ", token.Data(), right_op, left_op);

                    Stack<String> temp = exec_stack;
                    while (!temp.empty()) {
                        printf("%d ", temp.top().ParseInt32());
                        temp.pop();
                    }

                    printf("\n");
                }

                if (token == "+") {
                    exec_stack.push(StringAsInt(left_op + right_op));
                }
                else if (token == "-") {
                    exec_stack.push(StringAsInt(left_op - right_op));
                }
                else if (token == "*") {
                    exec_stack.push(StringAsInt(left_op * right_op));
                }
                else if (token == "/") {
                    if (right_op == 0) {
                        err = true;
                        break;
                    }

                    exec_stack.push(StringAsInt(left_op / right_op));
                }
            }

            if (IsFunction(token)) {
                {
                    Stack<String> temp = exec_stack;
                    printf("%s ", token.Data());

                    while (!temp.empty()) {
                        printf("%d ", temp.top().ParseInt32());
                        temp.pop();
                    }

                    printf("\n");
                }

                if (token == "N") {
                    int32_t operand = exec_stack.top().ParseInt32();
                    exec_stack.pop();

                    exec_stack.push(StringAsInt(-operand));
                }
                else if (token == "IF") {
                    String third_op = exec_stack.top();
                    exec_stack.pop();

                    String sec_op = exec_stack.top();
                    exec_stack.pop();

                    int32_t first_op = exec_stack.top().ParseInt32();
                    exec_stack.pop();

                    if (first_op > 0) {
                        exec_stack.push(sec_op);
                    }
                    else {
                        exec_stack.push(third_op);
                    }
                }
                else if (token[0] == 'M') {
                    // MAX or MIN

                    uint32_t argc = static_cast<uint32_t>(String(token.Data() + 3).ParseInt32()); // argument number
                    int32_t result = token[1] == 'A' ? INT32_MIN : INT32_MAX;

                    for (uint32_t j = 0; j < argc; ++j) {
                        int32_t operand = exec_stack.top().ParseInt32();
                        exec_stack.pop();

                        result = token[1] == 'A' ? max(result, operand) : min(result, operand);
                    }

                    exec_stack.push(StringAsInt(result));
                }
            }
        }

        if (err) {
            printf("ERROR\n");
            continue;
        }

        int32_t answer = exec_stack.top().ParseInt32();
        printf("%d\n\n", answer);
        // printf("Answer for line #%u: %d\n", i + 1, answer);
    }
}

uint8_t Calculator::GetPriority(const String& token) {
    if (token == "+" || token == "-") {
        return 1;
    }
    else if (token == "*" || token == "/") {
        return 2;
    }
    else if (IsFunction(token)) {
        return 3;
    }

    return 0;
}

bool Calculator::IsOperator(const String& token) {
    return token == "+" || token == "-" || token == "*" || token == "/";
}

bool Calculator::IsNumber(const String& token) {
    return token.Front() >= '0' && token.Front() <= '9';
}

bool Calculator::IsFunction(const String& token) {
    return token[0] == 'M' || token == "IF" || token == "N";
}