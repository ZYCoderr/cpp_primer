#pragma once
#include <vector>
#include <memory>
#include <string>
#include <set>
#include <algorithm>
/*
	第13章  拷贝控制
*/
namespace copy_assignment
{
	// 如果一个构造函数第一个参数是自身类类型的引用，
	// 且任何额外参数都有默认值，则此构造函数是拷贝构造函数
	// 拷贝构造函数第一个参数必须是一个引用类型，否则会陷入无限递归中
	// 拷贝构造函数在几种情况下都会被隐式地使用，因此拷贝构造函数通常不应该是explicit的
	class Foo
	{
	public:
		Foo() = default; // 默认构造函数
		Foo(const Foo&) = delete; // 禁用拷贝构造函数
		Foo& operator=(const Foo&); // 赋值运算符：通常应该返回一个指向其左侧运算对象的引用
		// 如果一个类未定义自己的靠背赋值运算符，编译器会为它生成一个合成拷贝赋值运算符

		// 析构函数不接受参数，因此它不能被重载。一个给定类只会有唯一一个析构函数
		// 析构函数自身并不直接销毁成员。成员是在析构函数体之后隐含的析构阶段中被销毁的
		// 在整个对象销毁过程中，析构函数是作为成员销毁步骤之外的另一部分而进行的
		// 我们不能删除析构函数。对于一个删除了析构函数的类型，编译器将不允许定义该类型的变量或创建该类型的临时变量。 
		// 而且如果一个类有某个成员的类型删除了析构函数，我们也不能定义该类的变量或临时对象
		// 对于删除了析构函数的类型，们虽然我们不能定义这种类型的变量或成员，但可以动态分配这种类型的对象
		// 但是，不能释放这些对象(不能用delete)
		~Foo() = default;

		/*
			三/五法则：
			三个基本操作可以控制类的拷贝操作：拷贝构造函数、拷贝复制运算和析构函数
			在新标准下，一个类还可以定义一个移动构造函数和一个移动赋值函数

			当我们能决定一个类是否要定义它自己版本的拷贝控制成员时，
			一个基本原则是首先确定这个类是否需要一个析构函数
			如果这个类需要一个析构函数，几乎可以肯定它也需要一个拷贝构造函数和一个拷贝赋值函数

		*/

	};

	// 行为像值的类
	class HasPtr
	{
	public:
		HasPtr() = default;
		HasPtr(const std::string& s = std::string()) : ps(new std::string(s)), i(0)
		{
		}
		HasPtr(const HasPtr& p) : ps(new std::string(*p.ps)), i(p.i)
		{

		}
		HasPtr& operator=(const HasPtr&);

		~HasPtr() 
		{
			delete ps;
		}
	private:
		std::string *ps;
		int	i;
	};


	// 行为像指针的类
	// 引入应用计数机制，并将计数器保存在动态内存中。
	// 当创建一个对象时，我们也分配一个新的计数器
	// 当拷贝或赋值对象时，我们拷贝指向计数器的指针
	// 使用这种方法，副本和原对象都会指向相同的计数器
	class HasPtr_pt
	{
	public:
		HasPtr_pt(const std::string& s = std::string())
			: ps(new std::string(s)), i(0), use(new std::size_t(1)) {}
		HasPtr_pt& operator=(const HasPtr_pt&);
		~HasPtr_pt();
	private:
		std::string *ps;
		int i;
		std::size_t* use;
	};

	// 拷贝控制示例	
	class Folder;
	class Message
	{
		friend class Folder;
		friend void swap(Message& lhs, Message& rhs);
	public:
		// floders被隐式初始化为空集合
		explicit Message(const std::string& str = "")
			: contents(str) {}
		Message(const Message&); 
		Message(const Message&&);
		Message& operator=(const Message&);		
		Message& operator=(Message&&);
		~Message();
		// 从给定的Folder集合中添加、删除本Message
		void save(Folder&);
		void remove(Folder&);
		void move_Folders(Message *m);

	private:
		std::string contents; // 实际消息文本
		std::set<Folder*> folders; // 包含本Message的Folder
		// 拷贝构造函数、拷贝赋值运算符和析构函数中所使用的工具函数
		// 将本Message添加到指向参数的Folder中
		void add_to_Folders(const Message&);
		// 从folers中的每个Folder中删除本Message
		void remove_from_Folders();
	};


	class Folder
	{
	public:
		void addMsg(Message* msg);
		void remMsg(Message* msg);

	private:
		std::set<Message*> msgs;
	};

	class StrVec
	{
	public:
		void push_back(const std::string&) {}//拷贝元素
		void push_back(std::string&&) {} // 移动元素
		// 引用限定符可以是&或&&，分别指出this可以指向一个左值或右值
		// 就像一个成员函数可以根据是否有重载const来区分其重载版本一样，引用限定符也可以区分重载版本
		// 而且我们可以综合引用限定符和const来区分一个成员的重载版本
		StrVec& operator=(const StrVec&) &; // 只能向可修改的左值赋值
		StrVec& operator=(const StrVec&) &&; // 只能向可修改的右值赋值
		StrVec sorted() && // 可以用于可以改变的右值
		{
			// 本对象为右值，因此可以原地址排序
			std::sort(data.begin(), data.end());
			return *this;
		}
		StrVec sorted() const &  // 可以用于任何类型的StrVec
		{
			// 本对象是const或一个左值，哪种情况我们都不能对其进行原地址排序
			StrVec ret(*this);
			std::sort(ret.data.begin(), ret.data.end());
			return ret;
		} 
	private:
		std::vector<int> data;

	};
	StrVec& StrVec::operator=(const StrVec& rhs) &
	{
		return *this;
	}
	StrVec& StrVec::operator=(const StrVec& rhs) &&
	{
		return *this;
	}
};

