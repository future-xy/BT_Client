#ifndef TORRENT
#define TORRENT

#include<string>
#include<vector>

using namespace std;

typedef struct {
	string announce;
	int length;
	string name;
	int piece_length;
	vector<string> pieces;
}torrent_file;

//在torrent.cpp中实现下面的函数

//从给定的种子文件中读取信息，返回一个torrent_file对象
torrent_file read_torrent(string filename);
//根据给定的文件和信息做种子，如果成功返回1，否则返回0
int make_torrent(string filename, int piece_length, string announce);


#endif // !TORRENT

