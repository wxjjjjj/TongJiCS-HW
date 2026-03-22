#include <iostream>
#include <string>
#include <stack>
#include <map>
 
using namespace std;
 
stack <char> Operation;      // 用于存储运算符的栈
stack <bool> Value;          // 用于存储值的栈
map <char, int> Priority;    // 用于存储运算符的优先级
 
bool result = 0;             // 记录最终结果
int total = 0;               // 记录表达式的数量
 
string expression[100];
 
bool Calculation(char temp) {
 
    bool num_1 = 0, num_2 = 0;
 
    if (temp == '|') {
 
        num_1 = Value.top();    // 获取栈顶元素
        Value.pop();            // 弹出栈顶元素
        num_2 = Value.top();    // 获取栈顶元素
        Value.pop();            // 弹出栈顶元素
 
        return (num_1 || num_2);
 
    }
 
    if (temp == '&') {
 
        num_1 = Value.top();    // 获取栈顶元素
        Value.pop();            // 弹出栈顶元素
        num_2 = Value.top();    // 获取栈顶元素
        Value.pop();            // 弹出栈顶元素
 
        return (num_1 && num_2);
 
    }
 
    if (temp == '!') {
 
        num_1 = Value.top();    // 获取栈顶元素
        Value.pop();            // 弹出栈顶元素
        return (!num_1);
 
    }
 
    return 0;
 
}
 
 
void Judgement_1(char temp) {
 
    char top = 0;
 
    /* 判断是否为空 */
    if (Operation.empty() == 1) {
        Operation.push(temp);
        return;
    }
 
    if (temp == '(') {
        Operation.push(temp);
        return;
    }
 
    top = Operation.top();  // 查看栈顶元素
 
    /* 如果将入栈的元素符优先级高于栈顶元素，则入栈 */
    if (Priority[top] >= Priority[temp]) {
        Operation.push(temp);
    }
    else {
        while (1) {
 
            /* 将栈顶元素弹出 */
            result = Calculation(top);
            Value.push(result);
            Operation.pop();
 
            /* 判断栈是否为空 */
            if (Operation.empty() == 1) {
                Operation.push(temp);
                break;
            }
 
            /* 判断现在栈顶元素和将入栈的元素优先级关系*/
            top = Operation.top();
            if (Priority[top] >= Priority[temp]) {
                Operation.push(temp);
                break;
            }
            
        }
    }
 
 
 
}
 
 
void Judgement_2(char temp) {
 
    char top = 0;
    
    while (1) {
 
        /* 查看栈顶元素是否为左括号，这是退出的条件 */
        top = Operation.top();
        if (top == '(') {
            Operation.pop();
            break;
        }
 
        /* 将栈顶元素 弹出*/
        result = Calculation(top);
        Value.push(result);
        Operation.pop();
 
    }
 
}
 
 
 
 
int main() {
 
    /* 定义运算的优先级（数字越小，优先级越高）*/
    Priority['('] = 3;
    Priority['|'] = 2;
    Priority['&'] = 1;
    Priority['!'] = 0;
 
    char temp;   // 临时存储输入的值
 
    /* 读入操作 */
    while (1) {
 
        getline(cin, expression[total]);
        if (expression[total].empty() == 1) {
            break;
        }
 
        total++;    // 表达式数量增加
        
    }
 
    /* 判断及输出操作 */
    for (int i = 0; i <= total - 1; i++) {
        
        int length = expression[i].length();   // 获取表达式长度
 
        /* 清空栈 */
        while (Operation.empty() != 1) {
            Operation.pop();
        }
 
        while (Value.empty() != 1) {
            Value.pop();
        }
 
 
        /* 输入操作 */
        for (int k = 0; k <= length - 1; k++) {
 
            temp = expression[i][k];
 
            /* 以换行符作为退出的条件 */
            if (temp == ' ') {
                continue;
            }
 
            if (temp == 'V') {
                Value.push(1);
            }
 
            else if (temp == 'F') {
                Value.push(0);
            }
 
            else if ((temp == '(') || (temp == '!') || (temp == '&') || (temp == '|')) {
                Judgement_1(temp);
            }
 
            else if (temp == ')') {
                Judgement_2(temp);
            }
 
        }
 
        bool judge = 0;   // 判断栈是否为空
 
        /* 确保所有元素出栈 */
        while (1) {
 
            /* 判断操作符的栈是否为空 */
            judge = Operation.empty();
            if (judge == 1) {
                result = Value.top();
                if (result == 1) {
                    cout << "Expression " << i + 1 << ": V" << endl;
                }
                else {
                    cout << "Expression " << i + 1 << ": F" << endl;
                }
                break;
            }
 
            temp = Operation.top();
            Operation.pop();
            result = Calculation(temp);
            Value.push(result);
 
        }
 
    }
 
    return 0;
 
}
