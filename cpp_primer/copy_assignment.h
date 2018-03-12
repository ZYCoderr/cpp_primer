#pragma once
#include <vector>
#include <memory>
#include <string>
#include <set>
/*
	��13��  ��������
*/
namespace copy_assignment
{
	// ���һ�����캯����һ�����������������͵����ã�
	// ���κζ����������Ĭ��ֵ����˹��캯���ǿ������캯��
	// �������캯����һ������������һ���������ͣ�������������޵ݹ���
	// �������캯���ڼ�������¶��ᱻ��ʽ��ʹ�ã���˿������캯��ͨ����Ӧ����explicit��
	class Foo
	{
	public:
		Foo() = default; // Ĭ�Ϲ��캯��
		Foo(const Foo&) = delete; // ���ÿ������캯��
		Foo& operator=(const Foo&); // ��ֵ�������ͨ��Ӧ�÷���һ��ָ�������������������
		// ���һ����δ�����Լ��Ŀ�����ֵ���������������Ϊ������һ���ϳɿ�����ֵ�����

		// �������������ܲ�������������ܱ����ء�һ��������ֻ����Ψһһ����������
		// ��������������ֱ�����ٳ�Ա����Ա��������������֮�������������׶��б����ٵ�
		// �������������ٹ����У�������������Ϊ��Ա���ٲ���֮�����һ���ֶ����е�
		// ���ǲ���ɾ����������������һ��ɾ�����������������ͣ���������������������͵ı����򴴽������͵���ʱ������ 
		// �������һ������ĳ����Ա������ɾ������������������Ҳ���ܶ������ı�������ʱ����
		// ����ɾ�����������������ͣ�����Ȼ���ǲ��ܶ����������͵ı������Ա�������Զ�̬�����������͵Ķ���
		// ���ǣ������ͷ���Щ����(������delete)
		~Foo() = default;

		/*
			��/�巨��
			���������������Կ�����Ŀ����������������캯�������������������������
			���±�׼�£�һ���໹���Զ���һ���ƶ����캯����һ���ƶ���ֵ����

			�������ܾ���һ�����Ƿ�Ҫ�������Լ��汾�Ŀ������Ƴ�Աʱ��
			һ������ԭ��������ȷ��������Ƿ���Ҫһ����������
			����������Ҫһ�������������������Կ϶���Ҳ��Ҫһ���������캯����һ��������ֵ����

		*/

	};

	// ��Ϊ��ֵ����
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


	// ��Ϊ��ָ�����
	// ����Ӧ�ü������ƣ����������������ڶ�̬�ڴ��С�
	// ������һ������ʱ������Ҳ����һ���µļ�����
	// ��������ֵ����ʱ�����ǿ���ָ���������ָ��
	// ʹ�����ַ�����������ԭ���󶼻�ָ����ͬ�ļ�����
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

	// ��������ʾ��	
	class Message;
	class Folder
	{
	public:
		void addMsg(Message* msg);
		void remMsg(Message* msg);

	private:
		std::set<Message*> msgs;
	};


	class Message
	{
		friend class Folder;
		friend void swap(Message& lhs, Message& rhs);
	public:
		// floders����ʽ��ʼ��Ϊ�ռ���
		explicit Message(const std::string& str = "")
			: contents(str) {}
		Message(const Message&); 
		Message(const Message&&);
		Message& operator=(const Message&);		
		Message& operator=(Message&&);
		~Message();
		// �Ӹ�����Folder��������ӡ�ɾ����Message
		void save(Folder&);
		void remove(Folder&);
		void move_Folders(Message *m);

	private:
		std::string contents; // ʵ����Ϣ�ı�
		std::set<Folder*> folders; // ������Message��Folder
		// �������캯����������ֵ�������������������ʹ�õĹ��ߺ���
		// ����Message��ӵ�ָ�������Folder��
		void add_to_Folders(const Message&);
		// ��folers�е�ÿ��Folder��ɾ����Message
		void remove_from_Folders();
	};





};

