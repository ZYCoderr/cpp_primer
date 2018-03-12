#include "stdafx.h"
#include "copy_assignment.h"
namespace copy_assignment 
{
	using std::vector;
	using std::string;
	using std::shared_ptr;
	using std::make_shared;
	using std::initializer_list;
	using std::out_of_range;
	using std::unique_ptr;
	using std::weak_ptr;

	Foo& Foo::operator=(const Foo&) = default;
	HasPtr& HasPtr::operator=(const HasPtr& rhs)
	{
		auto newp = new string(*rhs.ps); // 拷贝底层string
		delete ps; // 释放旧内存
		ps = newp; // 从右侧运算对象拷贝数据到本对象
		i = rhs.i;
		return *this; // 返回本对象
	}
	/*
		编写一个赋值运算符时，一个好的模式是现将右侧运算对象拷贝到一个临时对象中
		当拷贝完成后，销毁左侧运算对象的现有成员就是安全的了
		一旦左侧运算对象的资源被销毁了，就只剩下将数据从临时对象拷贝到左侧运算对象的成员中了

		下面这中做法是错误做：
		{
			delete ps; // 释放对象指向的string
			// 如果rhs和*this是同一个对象，我们就将从已释放的内存中拷贝数据！
			ps = new string(*rhs.ps);
			i = rhs.i;
			return *this;
		}
	*/

	HasPtr_pt& HasPtr_pt::operator=(const HasPtr_pt& rhs)
	{
		++*rhs.use;
		if (--*use == 0) {
			delete ps;
			delete use;
		}
		ps = rhs.ps;
		use = rhs.use;
		i = rhs.i;
		return *this;
	}

	HasPtr_pt::~HasPtr_pt()
	{
		if (--*use == 0) { //  如果引用计数变为0
			delete ps; // 释放string内存
			delete use; // 释放计数器内存
		}
	}
	void Folder::addMsg(Message* msg)
	{
		msgs.insert(msg);
	}
	void Folder::remMsg(Message* msg)
	{
		msgs.erase(msg);
	}

	void Message::save(Folder& f)
	{
		folders.insert(&f);
		f.addMsg(this);
	}
	void Message::remove(Folder& f)
	{
		folders.erase(&f);
		f.remMsg(this);
	}

	void Message::move_Folders(Message *m)
	{
		folders = std::move(m->folders);
		for (auto f : folders) {
			f->remMsg(m);
			f->addMsg(this);			
		}
		m->folders.clear();
	}

	void Message::add_to_Folders(const Message& m)
	{
		for (auto f : m.folders) { // 对每个包含m的Folder
			f->addMsg(this); // 向该Folder添加一个指向本Message的指针
		}
	}

	Message::Message(const Message& m)
		: contents(m.contents), folders(m.folders)
	{
		add_to_Folders(m);
	}
	
	void Message::remove_from_Folders()
	{
		for (auto f : folders)
		{
			f->remMsg(this);
		}
	}

	Message::~Message()
	{
		remove_from_Folders();
	}

	Message& Message::operator=(const Message& rhs)
	{
		remove_from_Folders();
		this->contents = rhs.contents;
		folders = rhs.folders;
		add_to_Folders(rhs);	
		return *this;

	}

	Message::Message(const Message&& m) 
		: contents(std::move(m.contents))
	{}

	Message& Message::operator=(Message&& rhs)
	{
		if (this != &rhs) { // 直接检查自赋值的情况
			remove_from_Folders();
			contents = std::move(rhs.contents);
			move_Folders(&rhs);
		}
		return *this;
	}

	void swap(Message& lhs, Message& rhs)
	{
		using std::swap;

		// 将每个消息的指针从它原来所在Folder中删除
		for (auto f : lhs.folders) f->remMsg(&lhs);
		for (auto f : rhs.folders) f->remMsg(&rhs);

		// 交换contents和folders指针set
		swap(lhs.folders, rhs.folders);
		swap(lhs.contents, rhs.contents);

		// 将每个Message的指针添加到它的新Folder中
		for (auto f : lhs.folders) f->addMsg(&lhs);
		for (auto f : rhs.folders) f->addMsg(&rhs);
	}

	// 关于 const 的一些知识
	void const_test()
	{
		// 指向常量的指针
		const double pi1 = 3.14;
		const double *ptr1 = &pi1;
		const double *ptr2 = &pi1;
		ptr1 = ptr2;
		double dval = 3.14;
		// 允许一个常量指针指向一个非常量对象
		ptr2 = &dval; // 正确，但不能通过ptr2改变dval的值
		/*
		所谓指向常量的指针或引用，不过是指针或引用"自以为是"罢了
		他们觉得自己指向了常量，所以自觉地不去改变所指向对象的值
		*/

		// const 指针
		/*
			指针是对象而引用不是，因此就像其他对象一样，允许把指针本身定位常量
			常量指针必须初始化，而且一旦初始化完成，则它的值（也就是存放在指针中的那个地址）
			就不能再改变了。 把*放在const关键字之前以说明指针是一个常量
			这样的书写形式隐含着一层以为，即不变的事指针本身的值而非指向的那个值
		*/
		int errNumb = 0;
		int * const curErr = &errNumb; // curErr将一直指向errNumb
		const double pi2 = 3.14159;
		const double * const pip = &pi2; // pip是一个指向常量对象的常量指针

		// 顶层const
		/*
			如前所述，还真本身是一个对象，它又可以指向另外一个对象
			因此，指针本身是不是常量以及指针所指的是不是一个常量就是两个而互相独立的问题
			用名词顶层const表示指针本身是个常量
			而用名词底层const表示指针所指的随想是一个常量
			更一般的，顶层const可以表示任意的对象是常量，底层const则与指针和引用等复合类型的基本类型部分有关
			指针既可以是顶层const也可以是底层const
		*/
		int i = 0;
		int * const p1 = &i; // 不能改变p1值，这是一个顶层const
		const int ci = 42; // 不能改变ci值，这是一个顶层const
		const int di = 43;
		const int *p2 = &ci; // 允许改变p2值，这是一个底层const
		p2 = &di;
		const int * const p3 = p2; // 靠右的const是顶层const，靠左的是底层const
		const int &r = ci; // 用于声明引用的const都是底层const

	}

	// 右值引用
	/*
		所谓右值引用就是必须绑定到右值的引用。
		我们通过&&而不是&来获得右值引用。
		右值引用有一个重要的心智：只能绑定到一个将要销毁的对象
		因此我们可以自由地将一个右值引用的资源"移动"到另一个对象中

		一般而言，一个左值表达式表示的是一个对象的身份，而一个右值表达式表示的是对象的值

		类似任何引用，一个右值引用也不过是某个对象的一个名字而已。
		我们可以将一个右值引用绑定到要求转换的表达式、字面常量或是返回右值的表达式
		但不能将一个右值引用直接绑定到一个左值上
	*/
	void rvalue_reference()
	{
		int i = 12;
		int &r = i;
		//int &&r = i; // 错误：不能讲一个右值引用绑定到一个左值上
		//int &r2 = i * 42; // 错误： i * 42是一个右值
		const int &r3 = i * 42; // 正确，我们可以将一个const、的引用绑定到一个右值上
		int &&r2 = i * 42; // 正确，将r2绑定到乘法结果上

		/*
			左值有持久的状态，而右值要么是字面常量，要么是表达式求值过程中创建的临时对象
			由于右值引用智能绑定到临时对象，我们得知：
			1. 所引用的对象将要被销毁
			2. 该对象没有其他用户
			这两个特性意味着，使用右值引用的代码可以自由地接管所引用的对象的资源
		*/

		// 标准库move函数
		/*
			我们不能将一个右值引用直接绑定到一个左值上
			但我们可以调用一个名为move的新标准库来获得绑定到左值上的右值引用
			move告诉编译器，我们有一个左值，但我们希望像一个右值一样处理它
			我们必须意识到，调用move就意味着承诺：除了对rr1赋值或销毁它外
			我们将不再使用它。在调用move之后，我们不能对移后源对象的值做任何假设
			我们可以销毁一个移后源对象，也可以赋予它新值，但不能使用一个移后源对象的值
		*/
		int &&rr1 = 42; // 正确：字面常量是右值
		// int &&rr2 = rr1; // 错误：表达式rr1是左值
		int &&rr3 = std::move(rr1); // ok
	
	}


}
