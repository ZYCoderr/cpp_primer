// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <memory>
#include <string>

template <typename T>
class Blob
{
public:
	typedef T value_type;
	typedef typename std::vector<T>::size_type size_type;
	// 构造函数
	Blob();
	Blob(std::initializer_list<T> il);
	// Blob中的元素数目
	size_type size() const { return data->empty(); }
	bool empty() const { return data->empty(); }
	// 添加和删除元素
	void push_back(const T& t) { data->push_back(t); }
	// 移动版本
	void push_back(T&& t) { data->push_back(std::move(t)); }
	void pop_back();
	// 元素访问
	T& back();
	T& operator[](size_type i);

private:
	std::shared_ptr<std::vector<T>> data;
	// 若data[i]无效，则抛出msg
	void check(size_type i, const std::string& msg) const;
};

template <typename T>
Blob<T>::Blob() : data(std::make_shared<std::vector<T>>) {}

template <typename T>
Blob<T>::Blob(std::initializer_list<T> il)
	: data(std::make_shared<std::vector<T>>(il))
{}

template <typename T>
void Blob<T>::check(size_type i, const std::string& msg) const
{
	if (i >= data->size())
		throw std::out_of_range(msg);
}

template <typename T>
T& Blob<T>::back()
{
	check(0, "back on temp Blob");
	return data->back();
}

template <typename T>
T& Blob<T>::operator[](size_type i)
{
	check(o, "subscrip out of range");
	return (*data)[i];
}

template <typename T>
void Blob<T>::pop_back()
{
	check(0, "pop_back on empty Blob");
	data->pop_back();
}

/*
当我们使用一个末班类型时必须提供模板参数，但这一规则有一个例外。
在类模板自己的作用域中，我们可以直接使用模板名而不提供实参
*/
template <typename T>
class BlobPtr
{
public:
	BlobPtr() :curr(0) {};
	BlobPtr(Blob<T>& a, size_t ez = 0) : wptr(a.data), curr(sz) {}
	T& operator*() const
	{
		auto p = check(curr, "dereferencepast end");
		return (*p)[curr]; // (*p)为本对象指向的vector
	}
	// 递增和递减
	//BlobPtr& operator++(); // 前置运算符
	//BlobPtr& operator--();

private:
	// 若检查成功， check返回一个指向vector的shared_ptr
	std::shared_ptr<std::vector<T>> check(std::size_t, const std::string&) const
	{
		if (curr == 0) std::out_of_range(msg);
	}
	// 保存一个weak_ptr, 表示底层vector可能被销毁
	std::weak_ptr<std::vector<T>> wptr;
	std::size_t curr;
};


int _tmain(int argc, _TCHAR* argv[])
{
	int &&rr1 = 42; // 正确：字面常量是右值
					// int &&rr2 = rr1; // 错误：表达式rr1是左值
	int &&rr3 = std::move(rr1); // ok

	rr1 = 3;
	system("pause");
	return 0;
}


