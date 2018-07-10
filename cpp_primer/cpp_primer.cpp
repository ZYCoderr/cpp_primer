// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include <iostream> 
class A { 
public: 
	A(const std::string& name)
	{ 
		std::cout << name << std::endl; 
	} 
}; 
int main1() 
{ 
	char szTmp[] = "Hello"; 
	A a(std::string(szTmp)); 
	return 0; 
}


auto func(int w) -> decltype(w)
{
	return w;
}

auto func(double w) -> decltype(w)
{
	return w;
}

template <typename T>
auto func(T w) -> decltype(T)
{
	int x = 3;
	const int cx = x;
	const int& fx = cx;
	auto ax = fx;
	decltype(auto) dx = fx;
	return w;
}




int _tmain(int argc, _TCHAR* argv[])
{
	//int x = 1;
	//int y = 1;
	//int z = 1;

	//int sum1{ x + y + z };

	int &&rr1 = 42; // 正确：字面常量是右值
					// int &&rr2 = rr1; // 错误：表达式rr1是左值
	int &&rr3 = std::move(rr1); // ok

	rr1 = 3;
	system("pause");
	return 0;
}


