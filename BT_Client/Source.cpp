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

			string t_name = make_torrent(filename, piece_length, server);
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
			send(sock, "0", 1, 0);
			send(sock, filename.c_str(), filename.size(), 0);
			send(sock, lis_port.c_str(), lis_port.size(), 0);

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
		//�������ص��ļ�ӵ�����б�
		istringstream ifs(buff);
		int n;
		ifs >> n;
		for (int i = 0; i < n; i++)
		{
			string ip, port;
			ifs >> ip >> port;
			target[ip] = port;
		}
		//�����ļ����ܿ���
		int piece_num = (tf.length + tf.piece_length - 1) / tf.piece_length;
		//����ÿ���߳�Ӧ�����صĿ���
		int task_num = (piece_num + n - 1) / n;
		//һ���߳���һ��ӵ������Ҫ
		int index = 0;
		for (auto item : target)
		{
			/*���һ�����ܻ��в���*/
			thread temp(download_t, item.first, item.second, tf.name, index*task_num*tf.piece_length,
				min(task_num, piece_num)*tf.piece_length, index);
			++index;
			piece_num -= task_num;
			//�����ٽ���
			mu.lock();
			t_num++;
			mu.unlock();
			temp.detach();
		}
		//��Ȩ�ȴ������߳��������
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
				//���յ����ļ������hashУ��
				if (tf.pieces[i] != SHA1(tempbuff, tempifs.gcount()))
				{
					cerr << "У��ʧ�ܣ�\n";
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
		//֪ͨ������
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
	//�����ļ���
	send(sock, filename.c_str(), filename.size(), 0);
	//������ʼλ��
	string temp = to_string(start);
	send(sock, temp.c_str(), temp.size(), 0);
	//�����ֽ���
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
	//�ļ���
	int cc = recv(sock, buff, bufflen, 0);
	buff[cc] = 0;
	ifstream myifs(string(buff), ios::binary);
	//��ʼλ��
	cc = recv(sock, buff, bufflen, 0);
	buff[cc] = 0;
	int start = stoi(string(buff));
	//�ֽ���
	cc = recv(sock, buff, bufflen, 0);
	buff[cc] = 0;
	int num = stoi(string(buff));
	//�ƶ��ļ�ָ��
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
