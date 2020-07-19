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


// https://blog.csdn.net/weixin_30299539/article/details/97154546
// check redis set expire是否设置为同一个原子操作
//