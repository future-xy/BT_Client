#ifndef HASH_H
#define HASH_H

#include<string>

using namespace std;

//对buff进行hash，返回20字节的哈希值
string SHA1(char* buff, int bufflen);

#endif // !HASH_H
