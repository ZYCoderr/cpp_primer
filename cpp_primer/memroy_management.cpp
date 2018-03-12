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
		// ʹ��ptr

	} // ptr�뿪�����򣬱�����

	/*
		ֻ��ָ������ṩ�Զ�̬������ڴ氲ȫ���ַ���Ĺ��� ���⽨������ȷʹ�õ�ǰ����
		Ϊ����ȷʹ������ָ�룬���Ǳ�����һЩ�����淶��
		1. ���ṩ��ͬ������ָ���ʼ������reset���������ָ��
		2. ����delete get()���ص�ָ��
		3. ��ʹ��get()��ʼ����reset��һ������ָ��
		4. �����ʹ����get()���ص�ָ�룬��ס�����һ����Ӧ������ָ�뱻���ٺ����ָ��ͱ�Ϊ��Ч��
		5. �����ʹ������ָ��������Դ����new������ڴ棬��ס��������һ��ɾ����

	*/

	void Test1()
	{
		int *x(new int(1024));
		// process(x); // ���󣺲��ܽ�һ��int*ת��Ϊһ��shared_ptr<int>
		// process(x); // ���󣺲��ܽ�һ��int*ת��Ϊһ��shared_ptr<int>
		process(shared_ptr<int>(x)); // �Ϸ��ģ����ڴ�ᱻ�ͷţ�
		int j = *x; // δ����ģ�x��һ������ָ�룡

	}

	void Test2()
	{
		/*
		����һ��unique_ptrʱ�� ��Ҫ�������һ��new���ص�ָ����
		����shared_ptr����ʼ��unique_ptr�������ֱ�ӳ�ʼ������ʽ
		����һ��unique_ptrӵ����ָ��Ķ������unique_ptr��֧����ͨ�Ŀ�����ֵ����
		*/
		unique_ptr<double> p1; // ����ָ��һ��double��unique_ptr
		unique_ptr<int> p2(new int(42)); // p2ָ��һ��ֵΪ42��int
		unique_ptr<string> p3(new string("Abc"));
		//unique_ptr<string> p4(p3); // ���� unique_ptr��֧�ֿ���
		//unique_ptr<string> p5;
		//p5 = p3; // ���� unique_ptr��֧�ָ�ֵ

		// ��Ȼ���ǲ��ܿ�����ֵunique_ptr�� ������ͨ������release��reset
		// ��ָ�������Ȩ��һ��(��const)unique_ptrת�Ƹ���һ��unique_ptr
		unique_ptr<string> p4(p3.release()); // release��p3����Ȩ��ָ��Abc��ת�Ƹ�������p3��Ϊ�գ�
		unique_ptr<string> p5(new string("Text"));
		// ������Ȩ��p5ת��p3
		p3.reset(p5.release()); // reset�ͷ���p2ԭ��ָ����ڴ�

		// release���ж�unique_ptr����ԭ������Ķ���֮�����ϵ
		// release���ص�ָ��ͨ����������ʼ����һ��ֻ��ָ������һ������ָ�븳ֵ

	}

	void Test3()
	{
		/*
			weak_ptr��һ�ֲ�������ָ����������ڵ�����ָ��
			��ָ����һ��shared_ptr����Ķ��󡣽�һ��weak_ptr�󶨵�һ��shared_ptr
			����ı�shared_ptr�����ü�����һ�����һ��ָ������shared_ptr�����٣�
			����ͻᱻ�ͷš���ʹ��weak_ptrָ����󣬶���Ҳ���ǻᱻ�ͷ�
		*/
		// �����Ǵ���һ��weak_ptrʱ��Ҫ��һ��shared_ptr����ʼ������
		auto p = make_shared<int>(42);
		weak_ptr<int> wp(p);

		// ���ڶ�����ܲ����ڣ����ǲ���ʹ��weak_ptrֱ�ӷ��ʶ��󣬶��������lock
		// �˺������weak_ptrָ��Ķ����Ƿ���ڡ�������ڣ�lock����һ��ָ��������shared_ptr
		// �������κ�shared_ptr���ƣ� ֻҪ��shared_ptr���ڣ�����ָ��ĵײ����Ҳ�ͻ�һֱ����
		if (shared_ptr<int> np = wp.lock()) { // ���np��Ϊ������������
			// ��if�У�np��p�������
		}

	}
}


