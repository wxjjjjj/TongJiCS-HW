#include <iostream>  
#include <stack>  
#include <string>  
 
using namespace std;
 
pair<int, int> longestValidParentheses(const string& s) {
    stack<int> stk; // 存储索引  
    stk.push(-1);   // 初始化为-1，方便处理第一个字符  
    int maxLength = 0;
    int startIndex = 0;
 
    for (int i = 0; i < int(s.size()); ++i) {
        if (s[i] == '(') {
            stk.push(i);
        }
        else {
            stk.pop();
            if (stk.empty()) {
                stk.push(i); // 非法右括号，记录位置  
            }
            else {
                int length = i - stk.top();
                if (length > maxLength) {
                    maxLength = length;
                    startIndex = stk.top() + 1; // 起始位置是栈顶索引的下一个位置  
                }
            }
        }
    }
 
    return { maxLength, startIndex };
}
 
int main() {
    string input;
    cin >> input;
 
    pair<int, int> result = longestValidParentheses(input);
 
    cout << result.first << " " << result.second << endl;
 
    return 0;
}
