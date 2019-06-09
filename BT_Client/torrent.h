#ifndef TORRENT
#define TORRENT

#include<string>
#include<vector>

using namespace std;

typedef struct {
	string announce;
	int length;
	string name;
	int pieces;
}torrent_file;

//在torrent.cpp中实现下面的函数

//从给定的种子文件中读取信息，返回一个torrent_file对象
torrent_file read_torrent(string filename);
//根据给定的文件和信息做种子，如果成功返回种子文件名，否则返回空字符串
string make_torrent(string filename, string announce);


#endif // !TORRENT

