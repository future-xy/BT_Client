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

//��torrent.cpp��ʵ������ĺ���

//�Ӹ����������ļ��ж�ȡ��Ϣ������һ��torrent_file����
torrent_file read_torrent(string filename);
//���ݸ������ļ�����Ϣ�����ӣ�����ɹ�����1�����򷵻�0
int make_torrent(string filename, int piece_length, string announce);


#endif // !TORRENT

