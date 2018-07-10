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
	using std::pair;
	using std::uninitialized_copy;


	/*-------------------------------  Blob<T> Implement   -----------------------------------------*/

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


	/*-------------------------------  StrVec Implement   -----------------------------------------*/
	template<typename... Args>
	inline void StrVec::emplace_back(Args&&... args)
	{
		chk_n_alloc();
		alloc.construct(first_free++, std::forward<Args>(args)...);
	}

	void StrVec::push_back(const string& s)
	{
		chk_n_alloc(); // 确保有空间容纳新元素
		// 在first_free指向的元素中构造s的副本
		// 用allocator分配内存时，必须记住内存是未构造的
		// 为了使用此原始内存，我们必须调用construct，在此内存中构造一个对象
		// 传递给construct的第一个参数必须是一个指针，指向调用allocate所分配的未构造的内存空间
		// 剩余参数确定用哪个构造函数来构造对象
		alloc.construct(first_free++, s);
	}

	// 此函数返回一个指针的pair，两个指针分别指向新空间开始位置和拷贝的尾后位置
	pair<string*, string*> StrVec::alloc_n_copy(const string* b, const string* e)
	{
		// 用尾后指针减去首元素指针来计算需要多少空间
		// 在分配内存后，它必须在此空间中构造给定元素的副本
		auto data = alloc.allocate(e - b);
		return { data, uninitialized_copy(b, e, data) };
	}

	void StrVec::free()
	{
		// 不能传递给deallocate一个空指针。如果elements为0，函数什么也不做
		if (elements) {
			// 逆序销毁旧元素
			// destory会运行string的析构函数，从而释放string自己分配的内存空间
			for (auto p = first_free; p != elements;) alloc.destroy(--p);

			// 一旦内存被销毁，我们就调用deallocate来释放本StrVec对象分配的内存空间
			// 我们传递给deallocate的指针必须是之前某次allocate调用所返回的指针
			// 因此在调用deallocate之前我们首先检查elements是否为空
			alloc.deallocate(elements, cap - elements);
		}
	}


	StrVec::StrVec(const StrVec& s)
	{
		auto newdata = alloc_n_copy(s.begin(), s.end());
		elements = newdata.first;
		first_free = cap = newdata.second;
	}
	StrVec::~StrVec()
	{
		free();
	}

	StrVec& StrVec::operator=(const StrVec& rhs)
	{
		auto data = alloc_n_copy(rhs.begin(), rhs.end());
		free();
		elements = data.first;
		first_free = cap = data.second;
		return *this;

	}

	void StrVec::reallocate()
	{
		// 我们将分配当前大小两倍的内存空间
		auto newcapacity = size() ? 2 * size() : 1;
		// 重新分配内存
		auto newdata = alloc.allocate(newcapacity);
		// 将旧数据从就内存移动到新内存
		auto dest = newdata; // 指向新数组中下一个空闲位置
		auto elem = elements; // 指向旧数组中下一个元素
		for (size_t i = 0; i != size(); ++i)
		{
			// 使用移动而不是拷贝构造函数
			// 这些string不会被拷贝
			// 构造的每个string都会从elem指向的string让那里接管内存的所有权
			alloc.construct(dest++, std::move(*elem++)); 
		}
		free();
		elements = newdata;
		first_free = dest;
		cap = elements + newcapacity;
	}
};






