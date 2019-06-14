#include <bits/stdc++.h>
#include "torrent.h"
using namespace std;

int main(){
//	char cs[]="Hello world!\0";
//	cout<<SHA1(cs, 10);
    cout<<make_torrent("fctest.txt",1, "localhost")<<endl;
    torrent_file torfil=read_torrent("fctest.torrent");
    cout<<torfil.announce<<" "<<torfil.length<<" "<<torfil.name<<" "<<torfil.piece_length<<endl;
    for (auto v:torfil.pieces)cout<<v<<endl;
//    string s; getline(cin, s);
//    cout<<s.size()<<s;
    return 0;
}
