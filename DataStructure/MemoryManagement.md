#### C++的内存管理

##### C++的内存分配方式

在了解C++内存池之前我们要先了解C++是如何对内存进行分配的。
我们知道C++的内存存储区分为5个，分别是

  1. 静态存储区：全局变量和静态变量被分配到同一块内存中，在以前的C语言中，全局变量又分为初始化的和未初始化的，在C++里面没有这个区分了，他们共同占用同一块内存区。
  2. 堆：使用new分配的内存块。
  3. 栈：在执行函数时，函数内局部变量的存储单元都可以在栈上创建，函数执行结束时这些存储单元自动被释放。
  4. 自由存储区：由malloc等分配的内存块，他和堆是十分相似的，用free来结束自己的生命的。
  5. 常量存储区：里面存放的是常量，不允许修改。



在这里简述一下堆和自由存储区的区别
自由存储是C++中通过new与delete动态分配和释放对象的抽象概念，而堆（heap）是C语言和操作系统的术语，是操作系统维护的一块动态分配内存。
附上一篇说明自由存储区与堆的区别的文章

https://www.cnblogs.com/QG-whz/p/5060894.html

假设执行函数
```C++
void f() { int* p=new int[5]; }
//在栈内存中存放了一个指向一块堆内存的指针p。在程序会先确定在堆中分配内存的大小，然后调用operator new分配内存，然后返回这块内存的首地址，放入栈中
```
##### 堆和栈的区别

  1. 内存管理方式不同：栈是由编译器自动管理，无需手工控制；堆的释放工作由程序员控制，容易造成内存泄露。
  2. 生长方向不同：堆的生长方向是向着内存地址增加的方向，而栈是向着内存地址减小的方向。
  3. 空间大小不同：在32位系统下，堆内存可以达到4G的空间。但是对于栈来讲，一般都是有一定的空间大小，且远小于堆的内存空间大小。

##### 内存泄漏的案例

对于堆来说，大量的使用new和delete运算符会导致内存的泄露，用以下的例子来说明内存泄漏的一些基本情况。

  1. 没有匹配的调用new和delete
  2. 没有正确的清除嵌套的对象指针
  3. 没有配套的使用new[]和delete[]
  4. 缺少拷贝构造函数让两个对象拥有指向同一个动态分配的内存空间的指针，析构的时候对同一个对象释放两次。
  5. 没有将基类的析构函数定义为虚函数
  6. 野指针的相关问题，比如未判定是否成功分配指针、未初始化指针变量、指针超过了变量的作用范围。

##### 重载new和delete

我们先来理解一下new的含义
假设有这样的代码`A* p = new A();`
编译器会将上述的代码理解为

```c++
A* p = nullptr;
void *mem = operator new(sizeof(A)); // 1 alloc 调用对应的重载版本
p = static_cast<A*>(mem);            // 2 cast
p->A:A();                            // 3 construct
```
所以可以了解到new实际上完成了三件事

  1. 分配内存
  2. 转换指针类型
  3. 执行构造函数

new和delete的重载有两种重载方式，分别是单个类的重载new/delete与全局new/delete运算符的重载。
  1. 类的new/delete运算符重载：使用new运算符(系统自带)建立一个类的对象时，new首先分配足以保证该类的一个对象所需要的内存，然后自动调用构造函数来初始化这块内存，再返回这个对象的地址。通过重载new 和 delete 操作符的方法，你可以自由地采用不同的分配策略，从不同的内存池中分配不同的类对象
```c++
class TestClass{
	public:
		void *operator new(size_t size);
		void *operator delete(void *p);
};

void *TestClass::operator new(size_t size){
	void *p = malloc(size);
	return (p);
}

void TestClass::delete(void *p){
	free(p);
}
```

##### 全局new/delete运算符的重载**：**

这种重载方式主要是在debug的时候对内存泄露进行跟踪、获得内存使用的统计数据

```c++
#include<iostream>
using namespace std;
//---------------------------------------------------------------
// 内存记录
//---------------------------------------------------------------
class MemInfo {
private:
  void* ptr;
  const char* file;
  unsigned int line;
  MemInfo* link;
  friend class MemStack;
};
//---------------------------------------------------------------
// 内存记录栈 
//---------------------------------------------------------------
class MemStack {
private:
  MemInfo* head;
public:
  MemStack():head(NULL) { }
  ~MemStack() { 
    MemInfo* tmp;
    while(head != NULL) {
      free(head->ptr); // 释放泄漏的内存 
      tmp = head->link;
      free(head);
      head = tmp;
    }
  }
  void Insert(void* ptr, const char* file, unsigned int line) {
    MemInfo* node = (MemInfo*)malloc(sizeof(MemInfo));
    node->ptr = ptr; node->file = file; node->line=line;
    node->link = head; head = node;    
  }
  void Delete(void* ptr) {
    MemInfo* node = head;
    MemInfo* pre = NULL;
    while(node != NULL && node->ptr!=ptr) {
      pre = node;
      node = node->link;
    }
    if(node == NULL)
      cout << "删除一个没有开辟的内存" << endl;
    else {
      if(pre == NULL) // 删除的是head 
        head = node->link;
      else 
        pre->link = node->link;
      free(node);
    }
  }
  void Print() {
    if(head == NULL) {
      cout << "内存都释放掉了" << endl; 
      return;
    }
    cout << "有内存泄露出现" << endl; 
    MemInfo* node = head;    
    while(node != NULL) {
      cout << "文件名: " << node->file << " , " << "行数: " << node->line << " , "
        << "地址: " << node->ptr << endl; 
      node = node->link;
    }
  }
};
//---------------------------------------------------------------
// 全局对象 mem_stack记录开辟的内存 
//---------------------------------------------------------------
MemStack mem_stack;
//---------------------------------------------------------------
// 重载new,new[],delete,delete[] 
//---------------------------------------------------------------
void* operator new(size_t size, const char* file, unsigned int line) {
  void* ptr = malloc(size);
  mem_stack.Insert(ptr, file, line);
  return ptr;
}
void* operator new[](size_t size, const char* file, unsigned int line) {
  return operator new(size, file, line); // 不能用new 
}
void operator delete(void* ptr) {
  free(ptr);
  mem_stack.Delete(ptr);
}
void operator delete[](void* ptr) {
  operator delete(ptr);
}
//---------------------------------------------------------------
// 使用宏将带测试代码中的new和delte替换为重载的new和delete 
//---------------------------------------------------------------
#define new new(__FILE__,__LINE__)
//---------------------------------------------------------------
// 待测试代码 
//---------------------------------------------------------------
void bad_code() {
  int *p = new int;
  char *q = new char[5];
  delete []q;
} 

void good_code() {
  int *p = new int;
  char *q = new char[5];
  delete p;
  delete []q;
} 
//---------------------------------------------------------------
// 测试过程 
//---------------------------------------------------------------
int main() {
  good_code();
  bad_code();
  mem_stack.Print();
  system("PAUSE");
  return 0;
}
```
##### 指针指向动态内存

首先我们要了解一点，指针是可以指向任意类型的内存块，我们来看一下以下的代码

```c++
char a[] = “hello”;//常量字符串hello存储在静态存储区，本条语句实际上是对字符串进行了一次拷贝操作，并将字符串的副本存储在数组a中.
a[0] = ‘X’;
cout << a << endl;
char *p = “world”; // 注意p指向常量字符串,常量字符串无法更改
p[0] = ‘X’; // 编译器不能发现该错误
cout << p << endl;
```
这里需要注意的是，如果想要将字符串a的内容拷贝到p所指的内存，需要先用malloc申请长度为(strlen(a)+1)的内存，再用strcpy函数进行复制。
**注意**：C++/C语言没有办法知道指针所指的内存容量，除非在申请内存时记住它。用运算符sizeof可以计算出数组的容量（字节数）。如下示例中，sizeof(a)的值是12（注意别忘了’’）。指针p指向a，但是sizeof(p)的值却是4。这是因为sizeof(p)得到的是一个指针变量的字节数，相当于sizeof(char*)，而不是p所指的内存容量。

```C++
char a[] = "hello world";
char *p = a;
cout<< sizeof(a) << endl; // 12字节
cout<< sizeof(p) << endl; // 4字节
```
注意当数组作为函数的参数进行传递时，该数组自动退化为同类型的指针。如下示例中，不论数组a的容量是多少，sizeof(a)始终等于sizeof(char *)。
```c++
void Func(char a[100]){
　cout<< sizeof(a) << endl; // 4字节而不是100字节
}
```
##### 指针参数传递内存

让我们来看一个经典的GetMemory问题

```c++
void GetMemory(char *p, int num){
	p = (char *)malloc(sizeof(char) * num);
}

void Test(void){
　char *str = NULL;
　GetMemory(str, 100); // str 仍然为 NULL
　strcpy(str, "hello"); // 运行错误
}
```
在上面的代码中Test函数的语句GetMemory(str, 200)并没有使str获得期望的内存，毛病出在函数GetMemory中。编译器总是要为函数的每个参数制作临时副本，指针参数p的副本是 _p，编译器使 _p = p。如果函数体内的程序修改了_p指向的内容，就导致参数p指向的内容作相应的修改（二者指向同样的内存）。这就是指针可以用作输出参数的原因。在本例中，_p申请了新的内存，只是把_p所指的内存地址改变了，但是p丝毫未变。
事实上，每执行一次GetMemory就会泄露一块内存，因为没有用free释放内存。对上述的函数有以下两种方式进行修改。
**使用指向指针的指针**

```C++
void GetMemory2(char **p, int num){
　*p = (char *)malloc(sizeof(char) * num);
}

void Test2(void){
　char *str = NULL;
　GetMemory2(&str, 100); // 注意参数是 &str，而不是str
　strcpy(str, "hello");
  cout<< str << endl;
　free(str);
}
```
**使用函数返回值传递动态内存**

```c++
char *GetMemory3(int num){
　char *p = (char *)malloc(sizeof(char) * num);
　return p;
}

void Test3(void){
　char *str = NULL;
　str = GetMemory3(100);
　strcpy(str, "hello");
　cout<< str << endl;
　free(str);
}
```
在使用函数返回值传递内存的时候注意不要使用"栈内存"的指针，因为该内存在函数结束时自动消亡，例如

```c++
char *GetString(void){
　char p[] = "hello world";//hello world存储在静态存储区，是一个只读的内存块，但此处相当于在函数栈内创建了一个数组并将字符串进行拷贝
　return p; // 返回的内存是栈内的数组内存，这段内存会在函数结束的时候释放掉
}


char *GetString2(void){
　char *p = "hello world";
　return p; //返回值是位于静态存储区内保存hello world的地址
}

void Test4(void){
　char *str = NULL;
　char *str2 = NULL;
　str = GetString(); // str 的内容是垃圾
　str2 = GetString2();//执行该条语句虽然不会报错，但是其设计概念是错误的，无论什么时候调用GetString2，它返回的始终是同一个“只读”的内存块。
　cout<< str << endl;
　cout<< str2 <<endl;
}
```

