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
		auto newp = new string(*rhs.ps);
		delete ps;
		ps = newp;
		i = rhs.i;
		return *this;
	}
}
