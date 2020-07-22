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

void PVop(){
    int in=0,out=0;
    semaphore empty,full,mutex;
    empty.value=n;    //空缓冲区资源信号量，初始状态有n个空的缓冲区
    full.value=0;    //满缓冲区资源信号量，初始状态有0个满缓冲区
    mutex.value=0;    //缓冲区互斥访问量

    void producer()    //生产者程序
    {
        while(true)
        {
            item=producerItem();    //生产一个产品
            p(empty);    //申请一个空缓冲区。如有，继续执行；否则，此处进程挂起
            p(mutex);    //如果没有其他进程访问缓冲区，继续执行；否则，进程挂起
            putItemIntoBuffer(item);    //将产品放入缓冲区
            in=(in+1)mod n;    //修改指针位置
            v(mutex);    //解锁，离开缓冲区，如果有其他进程等待进入，则唤醒
            v(full);    //增加一个满缓冲区，如果有消费者等待，则唤醒
        }
    }

    void consumer()    //消费者程序
    {
        while(true)
        {
            p(full);    //如果有满缓冲区，则继续执行；否则，挂起
            p(mutex);    //如果没有其他进程访问缓冲区，继续执行；否则，挂起
            item=removeItemFromBuffer();    //从缓冲区取走一个产品
            out=(out+1)mod n;    //修改指针位置
            consume(item);    //消费产品
            v(mutex);    //解锁，离开缓冲区，如果有其他进程等待进入，则唤醒
            v(empty);    //增加一个空缓冲区，如果有生产者等待，则唤醒
        }
    }
}