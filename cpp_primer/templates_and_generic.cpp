#include "stdafx.h"
#include "templates_and_generic.h"

namespace templates_and_generic
{
	using std::vector;
	using std::string;
	using std::shared_ptr;
	using std::make_shared;
	using std::initializer_list;
	using std::out_of_range;
	using std::unique_ptr;
	using std::weak_ptr;
	using std::size_t;

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

	// 模板类的每个static数据成员必须有且仅有一个定义
	// 但是类模板的每个实例都有一个独有的static对象
	// 因此预定义模板的成员函数类似，我们将static数据成员也定义为模板
	template <typename T>
	size_t Foo<T>::ctr = 0;

};






