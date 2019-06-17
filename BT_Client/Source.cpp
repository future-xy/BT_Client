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
#include<cstdio>

#include"hash.h"
#include"torrent.h"

#define WSVERS MAKEWORD(2,0)

#pragma comment(lib,"ws2_32.lib")

using std::string;
using std::thread;
using std::ofstream;
using std::ifstream;
using std::cin;
using std::cout;
using std::endl;
using std::mutex;
using std::map;
using std::vector;
using std::istringstream;
using std::cerr;
using std::min;
using std::ios;
using std::to_string;

const int bufflen = 1000000;
const int templen = 10;
const string server_port = "10086";
const string lis_port = "50520";
const int piece_length = 2;

mutex mu;

void download(torrent_file);
void download_t(string ip, string port, string filename, int start, int num, int index);
void upload();
void upload_t(SOCKET sock);

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
			cout << "���ֳɹ���\n";
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
			char tempbuff[templen];
			send(sock, "0", 1, 0);
			recv(sock, tempbuff, templen, 0);
			send(sock, filename.c_str(), filename.size(), 0);
			recv(sock, tempbuff, templen, 0);
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

	cout << "����������˳�..." << endl;
	getchar();
	getchar();

	return 0;
}

void download(torrent_file tf)
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
		recv(sock, buff, bufflen, 0);
		send(sock, tf.name.c_str(), tf.name.length(), 0);
		int cc = recv(sock, buff, bufflen, 0);
		buff[cc] = 0;
		map<string, string> target;
		//�������ص��ļ�ӵ�����б�
		istringstream ifs(buff);
		int n;
		ifs >> n;
		mu.lock();
		cout << "�ҵ�" << n << "��ӵ���ߣ�" << endl;
		mu.unlock();
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
		int num = tf.length;
		vector<thread*> t_pool;
		for (auto item : target)
		{
			/*���һ�����ܻ��в���*/
			t_pool.push_back(new thread(download_t, item.first, item.second, tf.name, index*task_num*tf.piece_length,
				min(task_num*tf.piece_length, num), index));
			++index;
			num -= index * task_num*tf.piece_length;
			if (num <= 0)
				break;
		}

		for (auto item : t_pool)
		{
			item->join();
			delete item;
		}

		mu.lock();
		cout << "������ϣ����ںϲ��ļ���" << endl;
		mu.unlock();
		ofstream myofs(tf.name, ios::binary);
		char* tempbuff = new char[tf.piece_length];

		for (int i = 0, j = 0; i < index; i++)
		{
			string tempname = tf.name + to_string(i) + ".temp";
			ifstream tempifs(tempname, ios::binary);
			cout << "���ںϲ���ʱ�ļ�" << i << "��" << endl;
			while (!tempifs.eof())
			{
				tempifs.read(tempbuff, tf.piece_length);
				int a = tempifs.gcount();
				if (a == 0)
					break;
				tempbuff[a] = 0;
				string tem(tempbuff);
				//���յ����ļ������hashУ��
				if (tf.pieces[j++] != SHA1(tempbuff, tempifs.gcount()))
				{
					cerr << "У��ʧ�ܣ�\n";
					tempifs.close();
					myofs.clear();
					myofs.close();
					return;
				}
				myofs.write(tempbuff, tempifs.gcount());
			}
			tempifs.close();
			remove(tempname.c_str());
			cout << "��ʱ�ļ�" << i << "�ϲ��ɹ���" << endl;
		}
		myofs.close();
		mu.lock();
		cout << tf.name << "���سɹ���" << endl;
		mu.unlock();
		//֪ͨ������
		send(sock, "0", 1, 0);
		recv(sock, buff, bufflen, 0);
		send(sock, tf.name.c_str(), tf.name.size(), 0);
		recv(sock, buff, bufflen, 0);
		send(sock, lis_port.c_str(), lis_port.size(), 0);
	}

	return;
}

void download_t(string ip, string port, string filename, int start, int num, int index)
{
	ofstream myofs(filename + to_string(index) + ".temp", ios::binary);

	char buff[bufflen];

	SOCKET sock;
	struct sockaddr_in sin;

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(ip.c_str());
	sin.sin_port = htons((u_short)stoi(port));
	int ret = connect(sock, (struct sockaddr*)&sin, sizeof(sin));
	//�����ļ���
	if (ret != 0)
	{
		cout << "��" << ip << "����ʧ�ܣ�" << endl;
		return;
	}
	mu.lock();
	cout << "��" << ip << "���ӳɹ���" << endl;
	mu.unlock();
	send(sock, filename.c_str(), filename.size(), 0);
	recv(sock, buff, bufflen, 0);
	//������ʼλ��
	string temp = to_string(start);
	send(sock, temp.c_str(), temp.size(), 0);
	recv(sock, buff, bufflen, 0);
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
	cout << "��" << index << "���ֽ�����ϣ�" << endl;
	mu.unlock();

	return;
}

void upload()
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
		mu.lock();
		cout << "�յ�" << inet_ntoa(fsin.sin_addr) << "����������" << endl;
		mu.unlock();
		thread temp(upload_t, sock);
		temp.detach();
	}

	return;
}

void upload_t(SOCKET sock)
{
	char buff[bufflen];
	//�ļ���
	int cc = recv(sock, buff, bufflen, 0);
	buff[cc] = 0;
	ifstream myifs(string(buff), ios::binary);
	//sendֻ��ͬ������
	send(sock, "y", 1, 0);
	//��ʼλ��
	cc = recv(sock, buff, bufflen, 0);
	buff[cc] = 0;
	int start = stoi(string(buff));
	send(sock, "y", 1, 0);
	//�ֽ���
	cc = recv(sock, buff, bufflen, 0);
	buff[cc] = 0;
	int num = stoi(string(buff));
	//�ƶ��ļ�ָ��
	myifs.seekg(start, ios::beg);
	mu.lock();
	cout << "���ڷ����ļ�!" << endl;
	mu.unlock();
	while (!myifs.eof() && num > 0)
	{
		myifs.read(buff, min(bufflen, num));
		send(sock, buff, myifs.gcount(), 0);
		num -= myifs.gcount();
	}
	myifs.close();
	mu.lock();
	cout << "�ļ����ͳɹ���" << endl;
	mu.unlock();

	return;
}
