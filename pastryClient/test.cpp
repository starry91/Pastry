#include <bits/stdc++.h>
#include <string>
// #include "../include/md5.h"
// #include <openssl/md5.h>
using namespace std;
struct node{
    int x,y;
};
int main(){
    vector<shared_ptr<node> > x(5, NULL);
    for(auto y:x)cout<<y<<endl;
    set<int> tt;
    // auto z = *tt.begin();
    unordered_map<int,int>m;
    m[0]=4;
    m[5]=6;
    for(auto it:m)cout<<it.first<<" "<<it.second<<endl;
    return 0;
}