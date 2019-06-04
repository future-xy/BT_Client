#include<iostream>
#include<fstream>

#include<string>
#include<vector>

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
const int piece_len = 20000;
const string port = "10086";

int num = 0;
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
			string t_name = make_torrent(filename, piece_len, server);
			cout << t_name << "�����ɹ���" << endl;
		}
		else if (order == "2")
		{
			cout << "�����ļ�����";
			string filename;
			cin >> filename;
			torrent_file t_file = read_torrent(filename);
			thread t(download, t_file);
			mu.lock();
			num++;
			mu.unlock();
			t.detach();
		}
		else if (order == "q")
		{
			cout << "�ȴ������������..." << endl;
			while (num);
		}
	}


	return 0;
}

int download(torrent_file tf)
{
	struct sockaddr_in sin;
	SOCKET sock;

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(tf.announce.c_str());
	sin.sin_port = htons((u_short)stoi(port));
	int ret = connect(sock, (struct sockaddr*)&sin, sizeof(sin));

	mu.lock();
	cout << "�������ӷ�����...\n";
	mu.unlock();

	if (ret == 0)
	{
		mu.lock();
		cout << "���ӳɹ�!\n";
		mu.unlock();
		send(sock, tf.name.c_str(), tf.name.length(), 0);
		//recv


	}

	return 0;
}