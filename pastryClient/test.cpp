#include <bits/stdc++.h>
#include <string>
#include "node.h"
#include <thread>
#include <memory>
using namespace std;
struct node{
    int x,y;
};
int main(){
    // vector<shared_ptr<node> > x(5, NULL);
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