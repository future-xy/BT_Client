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

	//给其他用户传送文件
	thread up(upload);
	up.detach();

	while (true)
	{
		string order;
		cout << "输入命令：" << endl
			<< "1.制作种子文件" << endl
			<< "2.下载文件" << endl
			<< "q.退出" << endl;
		cin >> order;
		if (order == "1")
		{
			cout << "输入文件名：";
			string filename;
			cin >> filename;
			cout << "输入服务器地址：";
			cin >> server;

			string t_name = make_torrent(filename, server);
			//向服务器上传消息
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


			cout << t_name << "创建成功！" << endl;
		}
		else if (order == "2")
		{
			cout << "输入种子文件名：";
			string filename;
			cin >> filename;
			torrent_file t_file = read_torrent(filename);
			download(t_file);
		}
		else if (order == "q")
		{
			//告诉服务器离开了
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
		//解析返回的文件拥有者列表

		if (target.size())
		{
			//选择一个拥有者进行连接
			auto item = target.begin();
			memset(&fsin, 0, sizeof(fsin));
			sin.sin_family = AF_INET;
			sin.sin_addr.s_addr = inet_addr(item->first.c_str());
			sin.sin_port = htons((u_short)stoi(item->second));

			ret = connect(fsock, (struct sockaddr*)&fsin, sizeof(fsin));
			//传输数据
			send(fsock, tf.name.c_str(), tf.name.length(), 0);
			//结束符
			string endstr = tf.name + "!!EndFile@@";
			int endlen = endstr.length();

			ofstream myof(tf.name, ios::binary);
			memset(buff, 0, sizeof(buff));
			while (true)
			{
				int cc = recv(fsock, buff, sizeof(buff), 0);
				//如果收到的数据长度恰好为结束符长度，则判断是否为结束符
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