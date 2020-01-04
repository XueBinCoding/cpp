#include <memory>
#include <iostream>

using namespace std;
template <typename T>
class SmartPtr
{
public:
	SmartPtr(T * ptr = nullptr):m_Ptr(ptr)
	{
		if (m_Ptr) 
		{
			m_ReferenceCount = new int(1);
		}
		else
		{
			m_ReferenceCount = new int(0);
		}
	}
	~SmartPtr() 
	{
		(*this->m_ReferenceCount)--;
		if (this->m_ReferenceCount == 0) 
		{
			delete this->m_Ptr;
			delete this->m_ReferenceCount;
		}
	}

	int use_count() 
	{
		return *this->m_ReferenceCount;
	}

	SmartPtr(const SmartPtr& ptr) 
	{
		if (this != &ptr) 
		{
			this->m_Ptr = ptr.m_Ptr;
			this->m_ReferenceCount = ptr.m_ReferenceCount;
			(*this->m_ReferenceCount)++;
		}
	}

	SmartPtr operator=(const SmartPtr& ptr) 
	{
		if (this->m_Ptr == ptr.m_Ptr) 
		{
			return *this;
		}
		if (this->m_Ptr) 
		{
			(*this->m_ReferenceCount)--;
			if (this->m_ReferenceCount == 0) 
			{
				delete this->m_Ptr;
				delete this->m_ReferenceCount;
			}
		}
		this->m_Ptr = ptr.m_Ptr;
		this->m_ReferenceCount = ptr.m_ReferenceCount;
		(*this->m_ReferenceCount)++;
		return *this;
	}

	T& operator*() 
	{
		if (this->m_Ptr)
		{
			return *(this->m_Ptr);
		}
	}

	T& operator->() 
	{
		if (this->m_Ptr) 
		{
			return this->m_Ptr;
		}
	}
private:
	T* m_Ptr;
	int *m_ReferenceCount;
};


int main() {
	SmartPtr<int> sm(new int(10));
	cout << "operator*():" << *sm << endl;
	cout << "reference counting:(sm)" << sm.use_count() << endl;
	SmartPtr<int> sm2(sm);
	cout << "copy ctor reference counting:(sm)" << sm.use_count() << endl;
	SmartPtr<int> sm3;
	sm3 = sm;
	cout << "copy operator= reference counting:(sm)" << sm.use_count() << endl;
	cout << &sm << endl;
	return 0;
}