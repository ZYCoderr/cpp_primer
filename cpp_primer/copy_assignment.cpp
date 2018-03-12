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
		auto newp = new string(*rhs.ps); // �����ײ�string
		delete ps; // �ͷž��ڴ�
		ps = newp; // ���Ҳ�������󿽱����ݵ�������
		i = rhs.i;
		return *this; // ���ر�����
	}
	/*
		��дһ����ֵ�����ʱ��һ���õ�ģʽ���ֽ��Ҳ�������󿽱���һ����ʱ������
		��������ɺ�������������������г�Ա���ǰ�ȫ����
		һ���������������Դ�������ˣ���ֻʣ�½����ݴ���ʱ���󿽱�������������ĳ�Ա����

		�������������Ǵ�������
		{
			delete ps; // �ͷŶ���ָ���string
			// ���rhs��*this��ͬһ���������Ǿͽ������ͷŵ��ڴ��п������ݣ�
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
		if (--*use == 0) { //  ������ü�����Ϊ0
			delete ps; // �ͷ�string�ڴ�
			delete use; // �ͷż������ڴ�
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
		for (auto f : m.folders) { // ��ÿ������m��Folder
			f->addMsg(this); // ���Folder���һ��ָ��Message��ָ��
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
		if (this != &rhs) { // ֱ�Ӽ���Ը�ֵ�����
			remove_from_Folders();
			contents = std::move(rhs.contents);
			move_Folders(&rhs);
		}
		return *this;
	}

	void swap(Message& lhs, Message& rhs)
	{
		using std::swap;

		// ��ÿ����Ϣ��ָ�����ԭ������Folder��ɾ��
		for (auto f : lhs.folders) f->remMsg(&lhs);
		for (auto f : rhs.folders) f->remMsg(&rhs);

		// ����contents��foldersָ��set
		swap(lhs.folders, rhs.folders);
		swap(lhs.contents, rhs.contents);

		// ��ÿ��Message��ָ����ӵ�������Folder��
		for (auto f : lhs.folders) f->addMsg(&lhs);
		for (auto f : rhs.folders) f->addMsg(&rhs);
	}

	// ���� const ��һЩ֪ʶ
	void const_test()
	{
		// ָ������ָ��
		const double pi1 = 3.14;
		const double *ptr1 = &pi1;
		const double *ptr2 = &pi1;
		ptr1 = ptr2;
		double dval = 3.14;
		// ����һ������ָ��ָ��һ���ǳ�������
		ptr2 = &dval; // ��ȷ��������ͨ��ptr2�ı�dval��ֵ
		/*
		��νָ������ָ������ã�������ָ�������"����Ϊ��"����
		���Ǿ����Լ�ָ���˳����������Ծ��ز�ȥ�ı���ָ������ֵ
		*/

		// const ָ��
		/*
			ָ���Ƕ�������ò��ǣ���˾�����������һ���������ָ�뱾��λ����
			����ָ������ʼ��������һ����ʼ����ɣ�������ֵ��Ҳ���Ǵ����ָ���е��Ǹ���ַ��
			�Ͳ����ٸı��ˡ� ��*����const�ؼ���֮ǰ��˵��ָ����һ������
			��������д��ʽ������һ����Ϊ�����������ָ�뱾���ֵ����ָ����Ǹ�ֵ
		*/
		int errNumb = 0;
		int * const curErr = &errNumb; // curErr��һֱָ��errNumb
		const double pi2 = 3.14159;
		const double * const pip = &pi2; // pip��һ��ָ��������ĳ���ָ��

		// ����const
		/*
			��ǰ���������汾����һ���������ֿ���ָ������һ������
			��ˣ�ָ�뱾���ǲ��ǳ����Լ�ָ����ָ���ǲ���һ�����������������������������
			�����ʶ���const��ʾָ�뱾���Ǹ�����
			�������ʵײ�const��ʾָ����ָ��������һ������
			��һ��ģ�����const���Ա�ʾ����Ķ����ǳ������ײ�const����ָ������õȸ������͵Ļ������Ͳ����й�
			ָ��ȿ����Ƕ���constҲ�����ǵײ�const
		*/
		int i = 0;
		int * const p1 = &i; // ���ܸı�p1ֵ������һ������const
		const int ci = 42; // ���ܸı�ciֵ������һ������const
		const int di = 43;
		const int *p2 = &ci; // ����ı�p2ֵ������һ���ײ�const
		p2 = &di;
		const int * const p3 = p2; // ���ҵ�const�Ƕ���const��������ǵײ�const
		const int &r = ci; // �����������õ�const���ǵײ�const

	}

	// ��ֵ����
	/*
		��ν��ֵ���þ��Ǳ���󶨵���ֵ�����á�
		����ͨ��&&������&�������ֵ���á�
		��ֵ������һ����Ҫ�����ǣ�ֻ�ܰ󶨵�һ����Ҫ���ٵĶ���
		������ǿ������ɵؽ�һ����ֵ���õ���Դ"�ƶ�"����һ��������

		һ����ԣ�һ����ֵ���ʽ��ʾ����һ���������ݣ���һ����ֵ���ʽ��ʾ���Ƕ����ֵ

		�����κ����ã�һ����ֵ����Ҳ������ĳ�������һ�����ֶ��ѡ�
		���ǿ��Խ�һ����ֵ���ð󶨵�Ҫ��ת���ı��ʽ�����泣�����Ƿ�����ֵ�ı��ʽ
		�����ܽ�һ����ֵ����ֱ�Ӱ󶨵�һ����ֵ��
	*/
	void rvalue_reference()
	{
		int i = 12;
		int &r = i;
		//int &&r = i; // ���󣺲��ܽ�һ����ֵ���ð󶨵�һ����ֵ��
		//int &r2 = i * 42; // ���� i * 42��һ����ֵ
		const int &r3 = i * 42; // ��ȷ�����ǿ��Խ�һ��const�������ð󶨵�һ����ֵ��
		int &&r2 = i * 42; // ��ȷ����r2�󶨵��˷������

		/*
			��ֵ�г־õ�״̬������ֵҪô�����泣����Ҫô�Ǳ��ʽ��ֵ�����д�������ʱ����
			������ֵ�������ܰ󶨵���ʱ�������ǵ�֪��
			1. �����õĶ���Ҫ������
			2. �ö���û�������û�
			������������ζ�ţ�ʹ����ֵ���õĴ���������ɵؽӹ������õĶ������Դ
		*/

		// ��׼��move����
		/*
			���ǲ��ܽ�һ����ֵ����ֱ�Ӱ󶨵�һ����ֵ��
			�����ǿ��Ե���һ����Ϊmove���±�׼������ð󶨵���ֵ�ϵ���ֵ����
			move���߱�������������һ����ֵ��������ϣ����һ����ֵһ��������
			���Ǳ�����ʶ��������move����ζ�ų�ŵ�����˶�rr1��ֵ����������
			���ǽ�����ʹ�������ڵ���move֮�����ǲ��ܶ��ƺ�Դ�����ֵ���κμ���
			���ǿ�������һ���ƺ�Դ����Ҳ���Ը�������ֵ��������ʹ��һ���ƺ�Դ�����ֵ
		*/
		int &&rr1 = 42; // ��ȷ�����泣������ֵ
		// int &&rr2 = rr1; // ���󣺱��ʽrr1����ֵ
		int &&rr3 = std::move(rr1); // ok
	
	}


}
