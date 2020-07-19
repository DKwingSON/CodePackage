#include<stdio.h>
#include<iostream>
#include<vector>
using namespace std;

//二分搜索框架
int binarySearch(vector<int> nums, int target) {
    int left = 0, right = nums.size();

    while(...) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) {
            ...;
        } else if (nums[mid] < target) {
            left = ...;
        } else if (nums[mid] > target) {
            right = ...;
        }
    }
    return ...;
}

//滑动窗口法
int slidingWindos(vector<int> s){
    int left = 0, right = 0;
    vector<int> window;
    auto isNotValid = [](vector<int> nums){
        //判断window内内容是否合法
        return false;
    };
    while(right < s.size()) {
        // 增大窗口
        window.push_back(s[right]);
        right++;

        while (isNotValid(window)) {
            // 缩小窗口
            window.erase(window.begin());
            left++;
        }
    }
}

//DFS搜索框架-递归
void dfs() 
{  
    if(到达终点状态)  
    {  
        ...//根据题意添加  
        return;  
    }  
    if(越界或者是不合法状态)  
        return;  
    if(特殊状态)//剪枝
        return ;
    for(扩展方式)  
    {  
        if(扩展方式所达到状态合法)  
        {  
            修改操作;//根据题意来添加  
            标记；  
            dfs（）；  
            (还原标记)；  
            //是否还原标记根据题意  
            //如果加上（还原标记）就是 回溯法  
        }  
 
    }  
} 

class UF {
private:
    // 记录连通分量个数
    int count;
    // 存储若干棵树
    vector<int> parent;
    // 记录树的“重量”
    vector<int> size;

    UF(int n) {
        count = n;
        parent = vector<int>(n, 0);
        size = vector<int>(n, 0);
        for (int i = 0; i < n; i++) {
            parent[i] = i;
            size[i] = 1;
        }
    }
    
public:
    /* 将 p 和 q 连通 */
    void _union(int p, int q) {
        int rootP = find(p);
        int rootQ = find(q);
        //判断是不是一个并查集里的
        if (rootP == rootQ)
            return;
        
        // 小树接到大树下面，较平衡
        if (size[rootP] > size[rootQ]) {
            parent[rootQ] = rootP;
            size[rootP] += size[rootQ];
        } else {
            parent[rootP] = rootQ;
            size[rootQ] += size[rootP];
        }
        count--;
    }

    /* 判断 p 和 q 是否互相连通 */
    bool connected(int p, int q) {
        int rootP = find(p);
        int rootQ = find(q);
        // 处于同一棵树上的节点，相互连通
        return rootP == rootQ;
    }

    /* 返回节点 x 的根节点 */
    int find(int x) {
        while (parent[x] != x) {
            // 进行路径压缩
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    }
    
    int count() {
        return count;
    }
};


// https://blog.csdn.net/weixin_30299539/article/details/97154546
// check redis set expire是否设置为同一个原子操作