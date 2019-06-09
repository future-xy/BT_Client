#ifndef HASH_H
#define HASH_H

#include<string>

using namespace std;

//对文件进行hash，返回20字节的哈希值
string SHA1(string filename, int bufflen);

#endif // !HASH_H
