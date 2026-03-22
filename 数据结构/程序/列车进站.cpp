#include <iostream>  
#include <stack>  
#include <string>  
#include <sstream> // 用于istringstream，但在这个特定例子中实际上没有用到
#include<algorithm>
 
using namespace std;
 
// 判断给定的出站序列是否合法  
bool isValidOutSequence(const string& inSequence, const string& outSequence) {
    stack<char> stk;
    int i = 0, j = 0;
    while (j < inSequence.length()) {
        stk.push(inSequence[j++]);
        while (!stk.empty() && stk.top() == outSequence[i]) {
            stk.pop();
            i++;
        }
    }
    return i == outSequence.length();
}
 
int main() {
    string inSequence, outSequence, result;
 
    // 读取第一行的入站序列  
    getline(cin, inSequence);
    // 移除可能的换行符（虽然getline通常不会保留换行符）  
    inSequence.erase(remove_if(inSequence.begin(), inSequence.end(), ::isspace), inSequence.end());
 
    // 读取后续的出站序列，直到EOF  
    string line;
    while (getline(cin, line)) {
        // 移除可能的换行符和空格（虽然getline通常不会保留换行符，但这里可以确保没有额外的空格）  
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
        outSequence = line; // 将每行内容赋值给出站序列变量  
        if (isValidOutSequence(inSequence, outSequence)) {
            result += "yes\n";
        }
        else {
            result += "no\n";
        }
    }
 
    // 输出结果  
    cout << result;
 
    return 0;
}
