### 1.C++设计模式之单例模式

单例模式：只能实例化一个类的模式，通过将构造函数私有和使用static来实现该功能

```c++
1.	/* 
2.	* 关键代码：构造函数是私有的，不能通过赋值运算，拷贝构造等方式实例化对象。 
3.	*/  
4.	  
5.	//懒汉式一般实现：非线程安全，getInstance返回的实例指针需要delete  
6.	class Singleton  
7.	{  
8.	public:  
9.	    static Singleton* getInstance();  
10.	    ~Singleton(){}  
11.	  
12.	private:  
13.	    Singleton(){}                                        //构造函数私有  
14.	    Singleton(const Singleton& obj) = delete;            //明确拒绝  
15.	    Singleton& operator=(const Singleton& obj) = delete; //明确拒绝  
16.	      
17.	    static Singleton* m_pSingleton;  
18.	};  
19.	  
20.	Singleton* Singleton::m_pSingleton = NULL;  
21.	  
22.	Singleton* Singleton::getInstance()  
23.	{  
24.	    if(m_pSingleton == NULL)  
25.	    {  
26.	        m_pSingleton = new Singleton;  
27.	    }  
28.	    return m_pSingleton;  
29.	} 

```

#### 2.C++的智能指针

智能指针实际上是一个栈对象，并非是指针类型，在栈对象生命周期即将结束时，智能指针通过析构函数释放由它管理的堆内存。

##### 2.1不适用智能指针会造成的问题

- 忘记delete内存。忘记释放动态内存常会导致“内存泄漏”问题，因为这种内存永远不可能归还给自由空间了。

- 使用已经释放掉的对象。通过在释放内存后将指针置为空，有时可以检测出这种错误。如果不置为空，那么会产生空悬指针（野指针）。这会造成灾难性的后果。

- 同一块内存释放两次，当有两个指针指向相同的动态分配对象时，可能发生这种错误。如果对其中一个指针进行了delete操作，对象的内存就被归还给自由空间了，如果随后又对第二个指针进行delete操作，自由空间就可能被破坏。

##### 2.2如何检测内存泄漏

###### windows下的内存泄漏检测

```C++
#define CRTDBG_MAP_ALLOC  //放在程序最前
#include <iostream>
#include <stdlib.h>    
#include <crtdbg.h> 
using namespace std;

int main()
{
	int *a = new int [10];
	int *p = new int[1000];
	_CrtDumpMemoryLeaks();  //放在程序最后
	system("pause");
	return 0;
}
```

###### Linux 下的内存泄漏检测

```c++
编译：g++ -g -o test test.cpp
使用：valgrind --tool=memcheck ./test
```

##### 2.3为什么C++11会放弃auto_ptr？

我们来看一段代码

```c++
void TestAutoPtr2() 
{
  std::auto_ptr<Simple> my_memory(new Simple(1));
  if (my_memory.get()) 
  {
    std::auto_ptr<Simple> my_memory2;   // 创建一个新的 my_memory2 对象
    my_memory2 = my_memory;             // 复制旧的 my_memory 给 my_memory2
    my_memory2->PrintSomething();       // 输出信息，复制成功
    my_memory->PrintSomething();        // 崩溃
  }
}
```

> 在上述的代码运行时，导致崩溃的原因是执行my_memory2 = my_memory后，my_memory2 取得了my_memory的内存管理权，导致my_memory悬空。

##### 2.4 shared_ptr

shared_ptr可以用来实现共享所有权的概念。多个智能指针可以引用同一个对象，当最后一个智能指针销毁时，对象销毁。每个shared_ptr对象在内部指向两个内存位置（1.指向对象的指针。2.用于控制引用计数的指针）

```c++
1.	#include <iostream>  
2.	#include  <memory> // 需要包含这个头文件  
3.	  
4.	int main()  
5.	{  
6.	    // 使用 make_shared 创建空对象  
7.	    std::shared_ptr<int> p1 = std::make_shared<int>();  
8.	    *p1 = 78;  
9.	    std::cout << "p1 = " << *p1 << std::endl; // 输出78  
10.	  
11.	    // 打印引用个数：1  
12.	    std::cout << "p1 Reference count = " << p1.use_count() << std::endl;  
13.	  
14.	    // 第2个 shared_ptr 对象指向同一个指针  
15.	    std::shared_ptr<int> p2(p1);  
16.	  
17.	    // 下面两个输出都是：2  
18.	    std::cout << "p2 Reference count = " << p2.use_count() << std::endl;  
19.	    std::cout << "p1 Reference count = " << p1.use_count() << std::endl;  
20.	  
21.	    // 比较智能指针，p1 等于 p2  
22.	    if (p1 == p2) {  
23.	        std::cout << "p1 and p2 are pointing to same pointer\n";  
24.	    }  
25.	  
26.	    std::cout<<"Reset p1 "<<std::endl;  
27.	  
28.	    // 无参数调用reset，无关联指针，引用个数为0  
29.     p1.reset();  
30.	    std::cout << "p1 Reference Count = " << p1.use_count() << std::endl;  
31.	      
32.	    // 带参数调用reset，引用个数为1  
33.	    p1.reset(new int(11));  
34.	    std::cout << "p1  Reference Count = " << p1.use_count() << std::endl;  
35.	  
36.	    // 把对象重置为NULL，引用计数为0  
37.	    p1 = nullptr;  
38.	    std::cout << "p1  Reference Count = " << p1.use_count() << std::endl;  
39.	    if (!p1) {  
40.	        std::cout << "p1 is NULL" << std::endl; // 输出  
41.	    }  
42.	    return 0;  
43.	}  


```

##### shared_ptr 的实现

为了实现智能指针的效果必须借助一个计数器，以便随时获知有多少智能指针绑定在同一个对象上，但是这个智能指针不能是smartpointer的一部分，这是因为如果计数器是智能指针的一部分那么每次增加减少计数器的值都必须广播到每一个管理着目标对象的智能指针。这样的代价很大。

以下是智能指针的实现。

```c++
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

```

#### 3.Lambda 表达式

 Lambda 表达式用于定义并创建匿名的函数对象，以简化编程工作。

```c++
[函数对象参数] (操作符重载函数参数) mutable 或 exception 声明 -> 返回值类型 {函数体}
auto add = [](int a, int b) {return a + b};
```

##### 3.1函数对象

函数对象参数是传递给编译器自动生成的函数对象类的构造
函数的。函数对象参数只能使用那些到定义 Lambda 为止时 Lambda 所在作用范围内可见的局部变量(包括 Lambda 所在类的 this)。函数对象参数有以下形式：

```c++
空。没有任何函数对象参数。

=。函数体内可以使用 Lambda 所在范围内所有可见的局部变量（包括 Lambda 所在类的 this），并且是值传递方式（相
当于编译器自动为我们按值传递了所有局部变量）。

&。函数体内可以使用 Lambda 所在范围内所有可见的局部变量（包括 Lambda 所在类的 this），并且是引用传递方式
（相当于是编译器自动为我们按引用传递了所有局部变量）。

this。函数体内可以使用 Lambda 所在类中的成员变量。

a。将 a 按值进行传递。按值进行传递时，函数体内不能修改传递进来的 a 的拷贝，因为默认情况下函数是 const 的，要
修改传递进来的拷贝，可以添加 mutable 修饰符。

&a。将 a 按引用进行传递。

a，&b。将 a 按值传递，b 按引用进行传递。

=，&a，&b。除 a 和 b 按引用进行传递外，其他参数都按值进行传递。

&，a，b。除 a 和 b 按值进行传递外，其他参数都按引用进行传递。
```

##### 3.2操作符重载函数参数

标识重载的 () 操作符的参数，没有参数时，这部分可以省略。参数可以通过按值（如: (a, b)）和按引用 (如: (&a, &b)) 两种方式进行传递。

##### 3.3 mutable 或者exception 声明

这部分可以省略。按值传递函数对象参数时，加上 mutable 修饰符后，可以修改传递进来的拷贝（注意是能修改拷贝，而不是值本身）。exception 声明用于指定函数抛出的异常，如抛出整数类型的异常，可以使用 throw(int)。

##### 3.4返回值类型

标识函数返回值的类型，当返回值为 void，或者函数体中只有一处 return 的地方（此时编译器可以自动推断出返回值类型）时，这部分可以省略。

##### 3.5函数体

标识函数的实现，这部分不能省略，但函数体可以为空。

示例如下：

```c++
std::vector<int> some_list;
int total = 0;
for (int i = 0; i < 5; ++i) some_list.push_back(i);
std::for_each(begin(some_list), end(some_list), [&total](int x)
{
    total += x;
});
```



```c++
std::vector<int> some_list;
int total = 0;
int value = 5;
std::for_each(begin(some_list), end(some_list), [&, value, this](int x)
{
    total += x * value * this->some_func();
});
```

