#ifndef TORRENT
#define TORRENT

#include<string>
#include<vector>

typedef struct {
	std::string announce;
	int length;
	std::string name;
	int piece_length;
	std::vector<std::string> pieces;
}torrent_file;

//��torrent.cpp��ʵ������ĺ���

//�Ӹ����������ļ��ж�ȡ��Ϣ������һ��torrent_file����
torrent_file read_torrent(std::string filename);
//���ݸ������ļ�����Ϣ�����ӣ�����ɹ����������ļ��������򷵻ؿ��ַ���
std::string make_torrent(std::string filename, int piece_length, std::string announce);


#endif // !TORRENT

