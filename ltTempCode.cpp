#include<iostream>
#include<vector>
#include<queue>
#include<stack>
#include<map>
using namespace std;
// 给定长度分别为 m 和 n 的两个数组，其元素由 0-9 构成，表示两个自然数各位上的数字。现在从这两个数组中选出 k (k <= m + n) 个数字拼接成一个新的数，要求从同一个数组中取出的数字保持其在原数组中的相对顺序。

// 求满足该条件的最大数。结果返回一个表示该最大数的长度为 k 的数组。

// 说明: 请尽可能地优化你算法的时间和空间复杂度。

// 示例 1:

// 输入:
// nums1 = [3, 4, 6, 5]
// nums2 = [9, 1, 2, 5, 8, 3]
// k = 5
// 输出:
// [9, 8, 6, 5, 3]
// 示例 2:

// 输入:
// nums1 = [6, 7]
// nums2 = [6, 0, 4]
// k = 5
// 输出:
// [6, 7, 6, 0, 4]
// 示例 3:

// 输入:
// nums1 = [3, 9]
// nums2 = [8, 9]
// k = 3
// 输出:
// [9, 8, 9]

// dp[i][j][k] => pick k from [0...i] of num1 and [0...j] of num2
// dp[i][j][k] = {
//     0 (k > i+j+2);
    
// }
// k长度的数 => num1 选 i 位， num2 选k-i位，然后拼接在一起
class Solution {
public:
    vector<int> maxNumber(vector<int>& nums1, vector<int>& nums2, int k) {
        if(k == 0) return {};

    }
};

// 给定一个以字符串表示的非负整数 num，移除这个数中的 k 位数字，使得剩下的数字最小。

// 注意:

// num 的长度小于 10002 且 ≥ k。
// num 不会包含任何前导零。
// 示例 1 :

// 输入: num = "1432219", k = 3
// 输出: "1219"
// 解释: 移除掉三个数字 4, 3, 和 2 形成一个新的最小的数字 1219。
// 示例 2 :

// 输入: num = "10200", k = 1
// 输出: "200"
// 解释: 移掉首位的 1 剩下的数字为 200. 注意输出不能有任何前导零。
// 示例 3 :

// 输入: num = "10", k = 2
// 输出: "0"
// 解释: 从原数字移除所有的数字，剩余为空就是0。









class Solution {
public:
    string trim(string s){
        int i = 0;
        while(i < s.size() && s[i] == '0'){
            i += 1;
        }
        return s.substr(i);
    }
    string stack2str(stack<char> s){
        string res = "";
        while(!s.empty()){
            res = s.top() + res;
        }
        return res;
    }
    string removeKdigits(string num, int k) {
        if(k == num.size()) return "0";

        int delNum = 0, index = 0;
        stack<char> s;
        s.push(num[index++]);

        while(index < num.size()){
            while(!s.empty() || s.top() > num[index]){
                s.pop();
                delNum += 1;
                if(delNum == k){
                    return stack2str(s) + num.substr(index);
                }
            }
            s.push(num[index++]);
        }
        cout<<"Main"<<endl;
        string res = stack2str(s);
        if(delNum < k){
            res = num.substr(0, num.size()-k);
        }
        return trim(res);
    }
};

class Solution {
public:
    string removeDuplicateLetters(string s) {
        vector<int> cnt(26, 0), used(26, 0);
        stack<char> st;
        int index = 0;
        for(auto &c:s){
            cnt[c-'a']++;
        }
        cout<<"main"<<endl;
        for(int i=0; i<s.size(); i++){
            char c = s[i];
            int encode = c - 'a';
            if(used[encode] == 0){
                while(!st.empty() && c < st.top() && cnt[st.top()-'a'] > 0)
                    st.pop();
                st.push(c);
            }
        }
        cout<<"main"<<endl;
        string res;
        while(!st.empty()){
            res = st.top() + res;
            st.pop();
        }
        cout<<"main"<<endl;
        return res;
    }
};

class Solution {
public:
    vector<int> nextGreaterElements(vector<int>& nums) {
        vector<int> res(nums.size(), 0);
        for(int i=0; i<nums.size(); i++){
            
        }
        return res;
    }
};

