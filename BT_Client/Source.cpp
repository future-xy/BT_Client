#include<iostream>
#include<fstream>
#include<sstream>

#include<string>
#include<vector>
#include<map>
#include<algorithm>

#include<thread>
#include<mutex>

#define NOMINMAX 
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
const int piece_length = 20000;

int t_num = 0;
mutex mu;

int download(torrent_file);
int download_t(string ip, string port, string filename, int start, int num, int index);
int upload();
int upload_t(SOCKET sock);

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

			string t_name = make_torrent(filename, piece_length, server);
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
			send(sock, "0", 1, 0);
			send(sock, filename.c_str(), filename.size(), 0);
			send(sock, lis_port.c_str(), lis_port.size(), 0);

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
				send(sock, "2", 1, 0);
			}
			break;
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
	sin.sin_port = htons((u_short)stoi(server_port));
	int ret = connect(sock, (struct sockaddr*)&sin, sizeof(sin));

	if (ret == 0)
	{
		send(sock, "1", 1, 0);
		send(sock, tf.name.c_str(), tf.name.length(), 0);
		int cc = recv(sock, buff, bufflen, 0);
		buff[cc] = 0;
		map<string, string> target;
		//解析返回的文件拥有者列表
		istringstream ifs(buff);
		int n;
		ifs >> n;
		for (int i = 0; i < n; i++)
		{
			string ip, port;
			ifs >> ip >> port;
			target[ip] = port;
		}
		//计算文件的总块数
		int piece_num = (tf.length + tf.piece_length - 1) / tf.piece_length;
		//计算每个线程应该下载的块数
		int task_num = (piece_num + n - 1) / n;
		//一个线程向一个拥有者索要
		int index = 0;
		for (auto item : target)
		{
			/*最后一个可能会有不足*/
			thread temp(download_t, item.first, item.second, tf.name, index*task_num*tf.piece_length,
				min(task_num, piece_num)*tf.piece_length, index);
			++index;
			piece_num -= task_num;
			//访问临界区
			mu.lock();
			t_num++;
			mu.unlock();
			temp.detach();
		}
		//让权等待所有线程下载完毕
		while (t_num)
			std::this_thread::yield();

		ofstream myofs(tf.name, ios::binary);
		char* tempbuff = new char[tf.piece_length];
		for (int i = 0; i < n; i++)
		{
			ifstream tempifs(tf.name + to_string(i) + ".temp", ios::binary);
			while (!tempifs.eof())
			{
				tempifs.read(tempbuff, tf.piece_length);
				//对收到的文件块进行hash校验
				if (tf.pieces[i] != SHA1(tempbuff, tempifs.gcount()))
				{
					cerr << "校验失败！\n";
					tempifs.close();
					myofs.clear();
					myofs.close();
					return -1;
				}
				myofs.write(tempbuff, tempifs.gcount());
			}
			tempifs.close();
		}
		myofs.close();
		//通知服务器
		send(sock, "0", 1, 0);
		send(sock, tf.name.c_str(), tf.name.size(), 0);
		send(sock, lis_port.c_str(), lis_port.size(), 0);
	}

	return 0;
}

int download_t(string ip, string port, string filename, int start, int num, int index)
{
	ofstream myofs(filename + to_string(index) + ".temp", ios::binary);

	char buff[bufflen];

	SOCKET sock;
	struct sockaddr_in sin;

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(ip.c_str());
	sin.sin_port = htons((u_short)stoi(port));
	connect(sock, (struct sockaddr*)&sin, sizeof(sin));
	//发送文件名
	send(sock, filename.c_str(), filename.size(), 0);
	//发送起始位置
	string temp = to_string(start);
	send(sock, temp.c_str(), temp.size(), 0);
	//发送字节数
	temp = to_string(num);
	send(sock, temp.c_str(), temp.size(), 0);

	while (num > 0)
	{
		int cc = recv(sock, buff, bufflen, 0);
		myofs.write(buff, cc);
		num -= cc;
	}
	myofs.close();

	mu.lock();
	--t_num;
	mu.unlock();

	return num;
}

int upload()
{
	SOCKET msock, sock;
	struct sockaddr_in sin, fsin;
	int alen = sizeof(struct sockaddr_in);

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
		thread temp(upload_t, sock);
		//
		temp.detach();
	}

	return 0;
}

int upload_t(SOCKET sock)
{
	char buff[bufflen];
	//文件名
	int cc = recv(sock, buff, bufflen, 0);
	buff[cc] = 0;
	ifstream myifs(string(buff), ios::binary);
	//起始位置
	cc = recv(sock, buff, bufflen, 0);
	buff[cc] = 0;
	int start = stoi(string(buff));
	//字节数
	cc = recv(sock, buff, bufflen, 0);
	buff[cc] = 0;
	int num = stoi(string(buff));
	//移动文件指针
	myifs.seekg(start, ios::beg);
	while (!myifs.eof() && num > 0)
	{
		myifs.read(buff, min(bufflen, num));
		send(sock, buff, myifs.gcount(), 0);
		num -= myifs.gcount();
	}
	myifs.close();

	return num;
}
