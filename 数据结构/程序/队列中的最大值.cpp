#include <iostream>
#include <queue>
#include <deque>
 
using namespace std;
 
int Queue[10010] = { 0 };     /* 存储数据的队列 */
int maxsize = 0;
int total = 0;
 
 
 queue<int> que;       // 主队列，用于存储元素
 deque<int> deq;       // 辅助队列，用于存储当前最大值
 
   
/* 返回当前队列的最大值，如果队列为空则返回 -1 */
void Getmax() {
 
    if (que.empty() == 1) {
        cout << "Queue is Empty" << endl;
    }
    else {
        cout << deq.front() << endl;
    }
 
}
 
 
/* 在队列末尾添加一个新元素 */
void Enqueue(int value) {
 
    if (total == maxsize) {
        cout << "Queue is Full" << endl;
    }
    else {
 
        que.push(value); // 将新元素加入主队列
 
        /* 维护 deq，移除所有小于新值的元素 */
        while ((deq.empty() != 1) && (deq.back() < value)) {
            deq.pop_back();
        }
 
        deq.push_back(value); // 将新值加入 deq
 
        total++;
 
    }
 
}
 
 
/* 移除并返回队列的前面元素 */
void Dequeue() {
 
    if (que.empty() == 1) {
        cout << "Queue is Empty" << endl;
        return;
    }
 
    int val = que.front(); // 获取队列前面的元素
 
    /* 如果移除的元素是当前最大值，则也从 deq 中移除 */
    if (val == deq.front()) {
        deq.pop_front();
    }
 
    cout << que.front() << endl;
    que.pop(); // 移除前面的元素
 
    total--;
 
}
 
 
void Quit() {
 
    while(que.empty() != 1) {
        cout << que.front() << " ";
        que.pop();
    }
    cout << endl;
}
 
int main() {
 
    cin >> maxsize;
 
    string Operation;
    int value = 0;
 
    while (1) {
 
        cin >> Operation;
        if (Operation[0] == 'q') {
            Quit();
            break;
        }
 
        if (Operation[0] == 'm') {
            Getmax();
        }
        else if (Operation[0] == 'd') {
            Dequeue();
        }
        else {
            cin >> value;
            Enqueue(value);
        }
 
    }
 
 
}
