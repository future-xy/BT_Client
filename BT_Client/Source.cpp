#include<iostream>
#include<fstream>

#include<string>
#include<vector>
#include<map>

#include<thread>
#include<mutex>

#include<WinSock2.h>
#include<stdlib.h>

#include"hash.h"
#include"torrent.h"

#define WSVERS MAKEWORD(2,0)

#pragma comment(lib,"ws2_32.lib")

using namespace std;

const int bufflen = 20000;
const string port = "10086";
const int max_threads = 3;


int download_num = 0;
mutex mu;

int download(torrent_file);

int main()
{
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);

	while (true)
	{
		string order;
		cout << "�������" << endl
			<< "1.���������ļ�" << endl
			<< "2.�����ļ�" << endl
			<< "q.�˳�" << endl;
		cin >> order;
		if (order == "1")
		{
			cout << "�����ļ�����";
			string filename;
			cin >> filename;
			cout << "�����������ַ��";
			string server;
			cin >> server;
			string t_name = make_torrent(filename, server);
			//
			cout << t_name << "�����ɹ���" << endl;
		}
		else if (order == "2")
		{
			cout << "�����ļ�����";
			string filename;
			cin >> filename;
			torrent_file t_file = read_torrent(filename);
			download(t_file);
			//����
		}
		else if (order == "q")
		{

		}
	}


	return 0;
}

int download(torrent_file tf)
{
	struct sockaddr_in sin;
	SOCKET sock;

	char buff[bufflen];

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(tf.announce.c_str());
	sin.sin_port = htons((u_short)stoi(port));
	int ret = connect(sock, (struct sockaddr*)&sin, sizeof(sin));

	if (ret == 0)
	{
		send(sock, tf.name.c_str(), tf.name.length(), 0);
		recv(sock, buff, bufflen, 0);
		map<string, string> target;
		//...

	/*	//�ָ���(3)���߳�����
		int download_task = (tf.pieces.size() + max_threads - 1) / max_threads;
		int index = 0;
		for (int i = 0; i < tf.pieces.size(); index++, i += download_task)
		{
			thread temp(download_t, i, download_task, index, target);
			mu.lock();
			download_num++;
			mu.unlock();
			temp.detach();
		}
		//����æ�ȴ�
		while (download_num)
			this_thread::yield();
		ofstream targetfile(tf.name, ios::binary);
		for (int i = 0; i < index; i++)
		{
			string tempfilename = to_string(i) + ".temp";
			ifstream tempfile(tempfilename, ios::binary);

			while (!tempfile.eof())
			{
				tempfile.read(buff, bufflen);
				targetfile.write(buff, tempfile.gcount());
			}
			tempfile.close();
		}
		targetfile.close();
		cout << "������ϣ�\n";*/
	}

	return 0;
}