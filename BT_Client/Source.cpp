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
const string server_port = "10086";
const string lis_port = "50520";

mutex mu;

int download(torrent_file);
int upload();

int main()
{
	string server;
	struct sockaddr_in sin;
	SOCKET sock;
	bool isconnected = false;

	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);

	//�������û������ļ�
	thread up(upload);
	up.detach();

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
			cin >> server;

			string t_name = make_torrent(filename, server);
			//��������ϴ���Ϣ
			sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
			memset(&sin, 0, sizeof(sin));
			sin.sin_family = AF_INET;

			sin.sin_addr.s_addr = inet_addr(server.c_str());
			sin.sin_port = htons((u_short)stoi(server_port));
			int ret = connect(sock, (struct sockaddr *)&sin, sizeof(sin));
			if (ret == 0)
				isconnected = true;
			else
			{
				cerr << "ERROR!\n";
				continue;
			}


			cout << t_name << "�����ɹ���" << endl;
		}
		else if (order == "2")
		{
			cout << "���������ļ�����";
			string filename;
			cin >> filename;
			torrent_file t_file = read_torrent(filename);
			download(t_file);
		}
		else if (order == "q")
		{
			//���߷������뿪��
			if (isconnected)
			{
				send(sock, "FIN", 3, 0);
			}
			break;
		}
	}


	return 0;
}

int download(torrent_file tf)
{
	struct sockaddr_in sin, fsin;
	SOCKET sock, fsock;

	char buff[bufflen];

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(tf.announce.c_str());
	sin.sin_port = htons((u_short)stoi(server_port));
	int ret = connect(sock, (struct sockaddr*)&sin, sizeof(sin));

	if (ret == 0)
	{
		send(sock, tf.name.c_str(), tf.name.length(), 0);
		recv(sock, buff, bufflen, 0);
		map<string, string> target;
		//�������ص��ļ�ӵ�����б�

		if (target.size())
		{
			//ѡ��һ��ӵ���߽�������
			auto item = target.begin();
			memset(&fsin, 0, sizeof(fsin));
			sin.sin_family = AF_INET;
			sin.sin_addr.s_addr = inet_addr(item->first.c_str());
			sin.sin_port = htons((u_short)stoi(item->second));

			ret = connect(fsock, (struct sockaddr*)&fsin, sizeof(fsin));
			//��������
			send(fsock, tf.name.c_str(), tf.name.length(), 0);
			//������
			string endstr = tf.name + "!!EndFile@@";
			int endlen = endstr.length();

			ofstream myof(tf.name, ios::binary);
			memset(buff, 0, sizeof(buff));
			while (true)
			{
				int cc = recv(fsock, buff, sizeof(buff), 0);
				//����յ������ݳ���ǡ��Ϊ���������ȣ����ж��Ƿ�Ϊ������
				if (cc == endlen)
				{
					buff[cc] = 0;
					if (string(buff) == endstr)
						break;
				}
				if (cc > 0)
					myof.write(buff, cc);
				else
					break;
				memset(buff, 0, sizeof(buff));
			}
			myof.close();

			//	send(sock, "FIN", 3, 0);
		}
		else
		{
			cerr << "No !" << endl;
		}
	}

	return 0;
}

int upload()
{
	SOCKET msock, sock;
	struct sockaddr_in sin, fsin;
	int alen;

	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons((u_short)stoi(lis_port));
	bind(msock, (struct sockaddr *)&sin, sizeof(sin));

	listen(msock, 5);

	while (true)
	{
		sock = accept(msock, (struct sockaddr *)&fsin, &alen);
		char buff[bufflen];
		int n = recv(sock, buff, bufflen, 0);
		if (n > 0)
			buff[n] = 0;
		else
		{

		}
		string filename(buff);
		ifstream myifs(filename, ios::binary);
		while (!myifs.eof())
		{
			memset(buff, 0, sizeof(buff));
			myifs.read(buff, bufflen);
			send(sock, buff, myifs.gcount(), 0);
		}
		string endstr = filename + "!!EndFile@@";
		send(sock, endstr.c_str(), endstr.length(), 0);
		myifs.close();
	}

	return 0;
}