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
			string server;
			cin >> server;
			string t_name = make_torrent(filename, server);
			//
			cout << t_name << "创建成功！" << endl;
		}
		else if (order == "2")
		{
			cout << "输入文件名：";
			string filename;
			cin >> filename;
			torrent_file t_file = read_torrent(filename);
			download(t_file);
			//传输
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

	/*	//分给多(3)个线程下载
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
		//避免忙等待
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
		cout << "下载完毕！\n";*/
	}

	return 0;
}