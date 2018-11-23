#include <iostream>
#include <string>
// #include "../include/md5.h"
#include <openssl/md5.h>
using namespace std;
int main(){
    string bytes = "hello";
    int len = 2;
    unsigned char hash_buff[MD5_DIGEST_LENGTH];
    MD5((const unsigned char *)bytes.c_str(), bytes.size(), hash_buff);
    string nodeID = "";
    auto n = *(unsigned long long *)hash_buff;
    for (int i = 0; i < 64; i += len)
    {
        int res = 0;
        for (int j = 0; j < len; j++)
        {
            res *= 2;
            if (n&(1<<(i+j))) res++;
        }
        nodeID += char(res+'0');
    }
    cout<<nodeID<<endl;
    return 0;
}