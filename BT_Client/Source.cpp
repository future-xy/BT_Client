#include<iostream>
#include<fstream>

#include<string>
#include<vector>

#include<process.h>
#include<WinSock2.h>
#include<stdlib.h>

#include"hash.h"
#include"torrent.h"

#define WSVERS MAKEWORD(2,0)

#pragma comment(lib,"ws2_32.lib")

using namespace std;

const int bufflen = 20000;
const int piece_len = 20000;

vector<HANDLE> thread_pool;


int main()
{
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
			make_torrent(filename, piece_len, server);
			cout<<""
		}
		else if (order=="2")
		{

		}
		else if(order=="q")
		{
			if (thread_pool.size())
			{
				cout << "";
			}
			else
			{
				break;
			}
		}
	}


	return 0;
}