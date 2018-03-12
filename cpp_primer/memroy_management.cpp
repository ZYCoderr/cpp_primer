#include "stdafx.h"
#include "memroy_management.h"

namespace memroy_management
{
	using std::vector;
	using std::string;
	using std::shared_ptr;
	using std::make_shared;
	using std::initializer_list;
	using std::out_of_range;
	using std::unique_ptr;
	using std::weak_ptr;
	StrBlob::StrBlob() : data(make_shared<vector<string>>()) {}

	StrBlob::StrBlob(initializer_list<string> il) : data(make_shared<vector<string>>(il)) {}

	void StrBlob::check(size_type i, const string& msg) const
	{
		if (i > data->size())
			throw out_of_range(msg);
	}

	string& StrBlob::front()
	{
		check(0, "front on empty StrBlob");
		return data->front();
	}

	string& StrBlob::back()
	{
		check(0, "back on empty StrBlob");
		return data->back();
	}

	void StrBlob::pop_back()
	{
		check(0, "pop_back on empty StrBlob");
		data->pop_back();
	}

	void process(shared_ptr<int> ptr)
	{
		// 使用ptr

	} // ptr离开作用域，被销毁

	/*
		只能指针可以提供对动态分配的内存安全而又方便的管理， 但这建立在正确使用的前提下
		为了正确使用智能指针，我们必须坚持一些基本规范：
		1. 不提供相同的内置指针初始化（或reset）多个智能指针
		2. 不用delete get()返回的指针
		3. 不使用get()初始化或reset另一个智能指针
		4. 如果你使用了get()返回的指针，记住当最后一个对应的智能指针被销毁后，你的指针就变为无效了
		5. 如果你使用智能指针管理的资源不是new分配的内存，记住给它传递一个删除器

	*/

	void Test1()
	{
		int *x(new int(1024));
		// process(x); // 错误：不能将一个int*转换为一个shared_ptr<int>
		// process(x); // 错误：不能将一个int*转换为一个shared_ptr<int>
		process(shared_ptr<int>(x)); // 合法的，但内存会被释放！
		int j = *x; // 未定义的：x是一个空悬指针！

	}

	void Test2()
	{
		/*
		定义一个unique_ptr时， 需要将其绑定在一个new返回的指针上
		类似shared_ptr，初始化unique_ptr必须采用直接初始化的形式
		由于一个unique_ptr拥有它指向的对象，因此unique_ptr不支持普通的拷贝或赋值操作
		*/
		unique_ptr<double> p1; // 可以指向一个double的unique_ptr
		unique_ptr<int> p2(new int(42)); // p2指向一个值为42的int
		unique_ptr<string> p3(new string("Abc"));
		//unique_ptr<string> p4(p3); // 错误： unique_ptr不支持拷贝
		//unique_ptr<string> p5;
		//p5 = p3; // 错误： unique_ptr不支持赋值

		// 虽然我们不能拷贝或赋值unique_ptr， 但可以通过调用release或reset
		// 将指针的所有权从一个(非const)unique_ptr转移给另一个unique_ptr
		unique_ptr<string> p4(p3.release()); // release将p3所有权（指向Abc）转移给，并将p3置为空，
		unique_ptr<string> p5(new string("Text"));
		// 将所有权从p5转给p3
		p3.reset(p5.release()); // reset释放了p2原来指向的内存

		// release会切断unique_ptr和它原来管理的对象之间的联系
		// release返回的指针通常被用来初始化另一个只能指针或给另一个智能指针赋值

	}

	void Test3()
	{
		/*
			weak_ptr是一种不控制所指向对象生存期的智能指针
			它指向有一个shared_ptr管理的对象。将一个weak_ptr绑定到一个shared_ptr
			不会改变shared_ptr的引用计数。一旦最后一个指向对象的shared_ptr被销毁，
			对象就会被释放。即使有weak_ptr指向对象，对象也还是会被释放
		*/
		// 当我们创建一个weak_ptr时，要用一个shared_ptr来初始化它：
		auto p = make_shared<int>(42);
		weak_ptr<int> wp(p);

		// 由于对象可能不存在，我们不能使用weak_ptr直接访问对象，而必须调用lock
		// 此函数检查weak_ptr指向的对象是否存在。如果存在，lock返回一个指向共享对象的shared_ptr
		// 于其他任何shared_ptr类似， 只要此shared_ptr存在，它所指向的底层对象也就会一直存在
		if (shared_ptr<int> np = wp.lock()) { // 如果np不为空则条件成立
			// 在if中，np于p共享对象
		}

	}
}


