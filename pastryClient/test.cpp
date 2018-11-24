#include <bits/stdc++.h>
#include<iostream>
#include <string>
#include "node.h"
#include <thread>
#include <memory>
using namespace std;
int main(){
    auto z = make_shared<int>();
    auto y = make_shared<int>(nullptr);
    if(y){
        cout<<"hello";
    }
    // if(x[0]){
    //     cout<<"hello";
    // }
    // for(auto y:x)cout<<y<<endl;
    // set<int> tt;
    // // auto z = *tt.begin();
    // unordered_map<int,int>m;
    // m[0]=4;
    // m[5]=6;
    // for(auto it:m)cout<<it.first<<" "<<it.second<<endl;
    auto x = make_shared<Node>();
    if(x) {
        cout << " true" << endl;
    }
    else {
        cout << "false" << endl;
    }
    // set<int> s;
    // s.erase(5);
    // cout<<"Yes"<<endl;
    return 0;
}