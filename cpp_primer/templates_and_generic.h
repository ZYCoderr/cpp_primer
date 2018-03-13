#pragma once

#include <vector>
#include <memory>
#include <string>
#include <type_traits>
#include <iostream>
// 模板与泛型编程
namespace templates_and_generic
{
	/*-------------------------------  类模板和模板参数    -----------------------------------------*/

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

	/*
	当我们使用一个模板类型时必须提供模板参数，但这一规则有一个例外。
	在类模板自己的作用域中，我们可以直接使用模板名而不提供实参
	*/
	template <typename T>
	class BlobPtr
	{
		// 在新标准中，我们可以将模板类型参数声明为友元
		// 此后，用来实例化BlobPtr模板的类型将会成为这个类模板实例的友元
		// 即 Foo将成为BlobPtr<Foo>的友元
		friend T;
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

	// 我们可以定义一个typedef来引用实例化的类
	// 由于模板不是一个类型，我们不能定义一个typedef引用一个模板
	// 即无法定义一个typedef引用Blob<T>
	typedef BlobPtr<std::string> StrBlob;

	// 但是新标准允许我们为类模板定义一个类型别名
	template <typename T> using twin = std::pair<T, T>;
	twin<std::string> authors; // authors是一个pair<string, string>

	// 模板的Static成员
	// 每个Foo实例都有自己的static成员实例
	// 即对任意给定类型X，都有一个Foo<X>::ctr和一个Foo<X>::count成员
	// 所有Foo<X>类型的对象共享相同的ctr对象和count函数
	template <typename T>
	class Foo
	{
	public:
		static std::size_t count() { return ctr; }

	private:
		static std::size_t ctr;
	};

	// 默认情况下，C++语言假定通过作用域运算符访问的名字不是类型
	// 因此吴国我们希望使用一个模板类型参数的类型成员，就必须显示告诉编译器
	// 改名字是一个类型。我们使用typename来实现这一点
	// 下面的top函数期待一个容器类型的实参
	// 它使用typename指明其返回类型并在c没有元素时生成一个初始化的元素返回给调用者
	// 当我们希望通知编译器一个名字表示类型时，必须用关键字typename而不能使用class
	template <typename T>
	typename T::value_type top(const T& c)
	{
		if (!c.empty()) {
			return c.back();
		} else {
			return typename T::value_type();
		}
	}

	/*-------------------------------  默认模板实参    -----------------------------------------*/

	// 就像我们能为函数提供默认实参一样，我们也可以提供默认模板实参
	// 在新标准中，我们可以为函数和类模板提供默认实参
	// 而更早的C++标准只允许为类模板提供实参
	template <typename T, typename F = std::less<T>>
	int compare(const T& v1, const T& v2, F f = F())
	{
		// 我们为此模板参数提供了默认实参，并为其对应的函数也提供了默认实参
		if (f(v1, v2)) return -1;
		if (f(v2, v2)) return 1;
		return 0;
	}

	// 如果要使用类模板的默认实参没需要在末班明之后跟一个空尖括号对
	template <typename T = int> 
	class Numbers // T默认为int
	{
	public:
		Numbers(T v = 0) : val(v) {}
		template <typename It> 
		Numbers(It b, It e);
	private:
		T val;

		
	};
	/*
		Numbers<double> a;
		Numbers<> b; // 空<>表示我们希望使用默认类型 
	*/

	// 与模板类的普通成员不同，成员模板是函数模板
	// 当我们在类模板外定义一个成员模板时
	// 必须同时为类模板和成员模板提供模板参数列表
	// 类模板的参数列表在前，后面跟成员自己的模板参数列表
	template <typename T> template <typename It>
	Numbers<T>::Numbers(It a, It b) {}



	/*-------------------------------  模板实参推断    -----------------------------------------*/

	// 当我们希望用户确定返回类型时，用显式模板实参表示模板函数的返回类型时很有效的
	// 但在其他情况下，要求显式指定模板实参会给用户添加额外的负担
	// 如果我们不知道返回结果的准确类型，但知道所需类型是所处理的序列的元素类型
	// 我们可以用decltype来获取此表达式的类型
	// 但是在编译器遇到函数的参数列表之前，参数都是不存在的。
	// 为了定义此函数，我们必须使用尾置返回类型
	// 由于位置返回出现在参数列表之后，它可以使用函数的参数列表
	template <typename It>
	auto fcn(It beg, It end) -> decltype(*beg)
	{
		// 处理序列
		return *beg; // 返回序列中第一个元素的引用
	}

	// 还可以用标准标准库的类型转换模板获得元素类型。这些模板定义在头文件type_traits中
	// 这个头文件中的类通常用于模板元程序设计
	// 如果我们用一个引用类型实例化remove_reference, 则type将表示被引用的类型
	// decltype(*beg) 返回元素类型的引用
	// remove_reference::type 脱去引用，剩下元素类型本身
	// 注意type是一个类的成员，而该类依赖于一个模板。
	// 因此我们必须在返回类型的声明中使用typename来告知编译器，type是一个类型
	template <typename It>
	auto fun2(It beg, It end) ->
		typename std::remove_reference<decltype(*beg)>::type
	{
		// 处理序列
		return *beg;
	}

	
	// 当我们用一个函数模板初始化一个函数指针或为一个函数指针赋值时
	// 编译器使用指针的类型来推断模板实参
	template <typename T> 
	int compare(const T& a, const T& b) { return a < b; }
	int(*pf1)(const int&, const int&) = compare;



	/*-------------------------------  引用折叠和右值引用参数    -----------------------------------------*/

	template <typename T> void f3(T&& val)
	{
		T t = val; 
	}
	// 当我们将一个左值传递给函数的右值引用参数
	// 而且此右值引用指向模板类型参数(如T&&)时
	// 编译器推断模板类型参数为实参的左值引用类型
	// 如果我们间接创建一个引用的引用，则这些引用形成了折叠
	// 在所有情况下（出了一个例外），引用会折叠成一个普通的左值引用类型
	// 只有有种特殊情况下引用会折叠为右值引用：右值引用的右值引用
	// 即对于一个给定类型X
	// X& &, X& &&和X&& &都折叠成类型X&
	// 类型X&& &&折叠成X&&
	// 如果将引用折叠规则和右值引用的特殊类型推断规则组合在一起
	// 则意味着我们可以对一个左值调用f3
	// 当我们将一个左值传递给f3的(右值引用)函数参数时
	// 编译器推断T为一个左值引用类型
	// f3(i): 实参是一个左值，模板参数T是int&
	// f3(ci): 实参是一个左值，模板参数T是const int&
	// 这两个规则导致了两个重要结果：
	// 1. 如果一个函数参数是一个指向模板类型参数的右值引用（如T&&），则它可以被绑定到一个左值
	// 2. 如果实参是一个左值，则推断出的模板实参类型将是一个左值引用
	// 这两条规则暗示我们可以将任意类型的实参传递给T&&类型的函数
	// 实际应用中，右值引用通常用于两种情况：模板转发其实参，或模板被重载




	/*-------------------------------  理解std::move()    -----------------------------------------*/
	
	// std::move的定义：
	/*
		template <typename T>
		typename remove_reference<T>::type&& move(T&& t) 
		{
			return static_cast<typename remove_reference<T>::type&&>(t);
		}
		函数参数是一个指向模板类型的右值引用。通过引用折叠，此参数可以与任何类型的实参匹配。既可以是左值，也可以是右值
		string s1("hi!"), s2;
		s2 = std::move(string("bye!")); // 正确，从一个右值移动数据
		s2 = std::move(s1); // 正确，但在赋值之后，s1的值是不确定的
		在第一个赋值中，传递给move的实参是string的构造函数的右值结果。因此在std::move(string("bye!"))中
		1. 推断出T的类型为string
		2. remove_reference用string进行实例化
		3. remove_reference<string>的type成员是string
		4. move返回的类型是string&&
		5. move参数t的类型为string&&
		函数体返回static_cast<string&&>(t)。t的类型已经是string&&，于是类型转换什么都不做。，此调用的结果就是它所接受的右值引用

		第二个赋值中传递给move的实参是一个左值。这样
		1. 推断出T的类型为string&
		2. remove_reference用string&进行实例化
		3. remove_reference<string&>的type成员是string
		4. move返回的类型认识string&&
		5. move的函数参数t实例化为string& &&，会折叠为string&
		这个实例的函数体返回static_cast<string&&>(t)

	*/



	/*-------------------------------  转发    -----------------------------------------*/

	// 如果一个函数参数时指向模板类型参数的右值引用(如T&&)
	// 它对应的实参的const属性和左值/右值属性将得到保持
	/*
		有些函数需要将其一个或多个实参联通类型不变地转发给其他函数
		在此情况下，我们需要保持被转发实参的所有性质
		包括实参类型是否是const，以及实参是左值还是右值
	*/
	// 接受一个可调用对象和另外两个参数的模板
	// 对"翻转"的参数调用给定的可调用对象
	// flip1是一个不完整的实现：顶层const和引用丢失了
	template <typename F, typename T1, typename T2>
	void flip1(F f, T1 t1, T2 t2)
	{
		
		f(t2, t1);
	}
	// 这个函数一般情况下工作得很好
	// 但当我们希望它调用一个接受引用参数的函数时就会出现问题
	void f(int v1, int &v2)
	{
		std::cout << v1 << " " << ++v2 << std::endl;
	}
	/*
		f(42, i); // f改变了实参i
		flip1(f, j, 42); // 通过flip调用f不会改变
		问题在与j被传递给flip1的参数t1，此参数是一个普通的、非引用的类型int
		而非int&。 因此这个flip1会实例化为
		void flip1(void(*fnc)(int, int&), int t1, int t2);
		j的值被拷贝到t1中。
		发的引用参数被绑定到t1而非j，从而其改变不会影响j
	*/

	// 为了能使参数保持给实参定的"左值性"以及它的const属性
	// 我们将函数参数定义为一个指向模板类型参数的右值引用。
	// 这样我们可以保持器对应实参的所有类型信息
	template <typename F, typename T1, typename T2>
	void flip2(F f, T1&& t1, T2&& t2)
	{
		f(t2, t1);
	}
	// 此时如果我们调用filp2(f, j, 42)， 将传递给参数t1一个左值j
	// 在flip2中，被推断出T1的类型为int&， 这意味着t1的类型会被折叠为int&
	// 由于是引用类型，t1被绑定到j上，当flip2调用f时，f中的引用参数v2被绑定到t1
	// 也就是绑定到j。当f递增v2是，它也同时 改变了j的值

	// 我们可以使用一个名为forward的新标准库设施来传递flip2的参数，它能保持原始实参的类型
	// forward定义在头文件utility中， 必须通过显式模板实参来调用
	// forward返回该显式实参类型的右值引用，即forward<T>返回的类型是T&&
	template <typename F, typename T1, typename T2>
	void flip(F f, T1&& t1, T2&& t2)
	{
		f(std::forward<T2>(t2), std::forward<T1>(t1));
	}




	/*-------------------------------  重载与模板    -----------------------------------------*/

	// 函数模板可以被另一个模板或一个普通非模板函数重载
	// 与往常一样，名字相同的函数必须具有不同数量或类型的参数
	template <typename T> std::string debug_rep(const T& t)
	{ // 打印任何我们不能处理的类型
		std::ostringstream ret;
		ret << t;
		return ret.str(); // 返回ret绑定的string的一个副本
	}
	template <typename T> std::string debug_ret(T* p)
	{
		std::ostringstream ret;
		ret << "pointer: " << p;
		if (p) {
			ret << " " << debug_rep(*p); // 打印p指向的值
		}
		else {
			ret << "nullptr";
		}
		return ret.str(); // 返回ret绑定的string的一个副本 
	}
	
	// 一个特定文件所需要的所有模板的声明通常一起放置在文件开始的位置
	// 出现于任何使用这些模板的代码之前


/*-------------------------------  可变参数模板    -----------------------------------------*/

	// 可变参数模板就是一个接受可变数目参数的模板函数或模板类
	// 可变数目的参数被称为参数包。
	// 存在两种参数包：
	// 模板参数包表示零个或多个模板参数
	// 函数参数包表示零个或多个函数参数
	// 我们用一个省略号来指出一个模板参数或函数参数表示一个包
	// 在一个模板表参数列表中，class...或typename...指出接下来的桉树表示零个或多个类型的列表
	// 一个类型后面跟一个省略号表示零个或多个给定类型的非类型参数的列表
	// 在函数参数列表中，如果一个参数的类型是一个模板参数包，则此参数也是一个函数参数包
	// 例如
	template <typename T, typename... Args>
	void foo(const T& t, const Args&... rest) {};
	// 声明了foo是一个可变参数函数模板
	// 它有一个名为T的类型参数和一个名为Args的模板参数包
	// 这个包表示零个或多个额外的类型参数
	// foo的函数参数列表包含一个const&类型的参数，指向T的类型
	// 还包含一个名为rest的函数参数包，此包表示零个或多个函数参数
	/*
		int i = 0;
		double d = 3.14;
		string s = "abc";
		foo(i, s, 42, d); //  包中有三个参数
		foo(i, 42, "ai"); //  包中有两个参数
		foo(d, s); // 包中有一个参数
		foo("hi"); // 空包
		编译器会为foo实例化出四个不同的版本
		void foo(const int&, const string&, const int&, const double&);
		void foo(const string&, const int&, const char[3]&);
		void foo(const double&, const string&);
		void foo(const char[3]&)
	*/

	// 当我们需要知道包中有多少元素时，可以使用sizeof...运算符
	// 类似sizeof， sizeof...也返回一个常量表达式
	template <typename... Args> void g(Args... args)
	{
		std::cout << sizeof...(Args) << std::endl; // 类型参数的数目
		std::cout << sizeof...(args) << std::endl; // 函数参数的数目
	}

	
	// 用来终止递归并打印最后一个元素的函数
	// 此函数必须在可变参数版本的print定义之前声明
	template <typename T>
	std::ostream& print(std::ostream& os, const T& t)
	{
		return os << t; // 包中最后一个元素之后不打印分隔符
	}
	// 包中出了最后一个元素之外的其他元素都会调用这个版本的print
	template <typename T, typename... Args>
	std::ostream& print(std::ostream&os, const T& t, const Args... rest)
	{
		os << t << ", "; // 打印第一个输惨
		return print(os, rest...); // 递归调用，打印其他实参
	}
	// 这里的可变参数版本的print函数接受三个参数，而此调用只传递了两个实参
	// 其结果是rest中的第一个实参被绑定到t， 剩余实参形成下一个print调用的参数包
	// 因此在每个调用中，包的第一个实参被移除，成为绑定到t的实参


	/*-------------------------------  包扩展    -----------------------------------------*/

	// 对于一个参数包，除了获取其大小外，我们能对它做的唯一的事情就是【扩展】它
	// 当扩展一个包时，我们还要提供用于每个扩展元素的【模式】
	// 扩展一个包就是将它分解为构成的元素，对每个元素应用模式，获得扩展后的列表
	// 我们通过在模式右边放一个省略号(...)来触发扩展操作
	template <typename T, typename... Args>
	std::ostream& print1(std::ostream& os, const T& t, const Args&... rest) // 扩展Args
	{
		os << t << ", ";
		return print1(os, rest...); // 扩展rest
	}


};

