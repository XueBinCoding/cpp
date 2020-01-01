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

