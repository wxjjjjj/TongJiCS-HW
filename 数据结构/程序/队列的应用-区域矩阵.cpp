#include <iostream>
#include <vector>
 
using namespace std;
 
int line = 0;
int row = 0;
int flag = 0;
int total = 0;
int result = 0;
 
class Node_1 {
public:
 
    int m = 0;
    int n = 0;
 
};
 
bool region[1010][1010] = { 0 };  /* 区域矩阵 */
 
vector <Node_1> ones; 
 
void Seek(int x, int y) {
 
    region[x][y] = 0;   // 将经过的点变成0
 
    if ((x != 0) && (x != (line - 1)) && (y != 0) && (y != (row - 1))) {
        flag = 1;
    }
 
    /* 判断是否在上边界 */
    if (x != 0) {
        if (region[x - 1][y] == 1) {
            if (((x - 1) != 0) && ((x - 1) != (line - 1)) && (y != 0) && (y != (row - 1))){
                flag = 1;
            }
            Seek(x - 1, y);
        }
    }
 
 
    /* 判断是否在下边界 */
    if (x != line - 1) {
        if (region[x + 1][y] == 1) {
            if (((x + 1) != 0) && ((x + 1) != (line - 1)) && (y != 0) && (y != (row - 1))) {
                flag = 1;
            }
            Seek(x + 1, y);
        }
    }
 
 
    /* 判断是否在左边界 */
    if (y != 0) {
        if (region[x][y - 1] == 1) {
            if ((x != 0) && (x != (line - 1)) && ((y - 1) != 0) && ((y - 1) != (row - 1))) {
                flag = 1;
            }
            Seek(x, y - 1);
        }
    }
 
 
    /* 判断是否在右边界 */
    if (y != row - 1) {
        if (region[x][y + 1] == 1) { 
            if ((x != 0) && (x != (line - 1)) && ((y + 1) != 0) && ((y + 1) != (row - 1))) {
                flag = 1;
            }
            Seek(x, y + 1);
        }
    }
 
}
 
 
 
int main() {
 
    /* 输入行和列 */
    cin >> line >> row;
 
    bool temp = 0;  // 临时存储输入的值
 
    /* 输入区域矩阵 */
    for (int i = 0; i <= line - 1; i++) {
        for (int k = 0; k <= row - 1; k++) {
            
            cin >> region[i][k];
            temp = region[i][k];
            
            /* 存储值为1的节点，避免后面遍历，节约查找1的时间 */
            if (temp == 1) {
                ones.resize(total + 1);
                ones[total].m = i;
                ones[total].n = k;
                total++;
            }
 
        }
    }
    
    /* 寻找四领域联通区域 */
    for (int num = 1; num <= total; num++) {
 
        int x = ones[num - 1].m;
        int y = ones[num - 1].n;
 
        /* 判断是否为1 */
        if (region[x][y] == 1) {
 
            flag = 0;  // 标记恢复为0
            result++;
            Seek(x, y);
            if (flag == 0) {
                result--;
            }
 
        }
 
    }
 
    cout << result << endl;
 
    return 0;
}
