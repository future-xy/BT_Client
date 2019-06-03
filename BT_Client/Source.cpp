#include<fstream>

int main()
{
	std::ofstream myofs("测试文件.txt");

	myofs << "测试文件";

	return 0;
}