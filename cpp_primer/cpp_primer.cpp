﻿// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <memory>
#include <string>


int _tmain(int argc, _TCHAR* argv[])
{
	int &&rr1 = 42; // 正确：字面常量是右值
					// int &&rr2 = rr1; // 错误：表达式rr1是左值
	int &&rr3 = std::move(rr1); // ok

	rr1 = 3;
	system("pause");
	return 0;
}


