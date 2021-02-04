#### 关键字noexcept

该关键字告诉编译器，函数中不会发生异常,这有利于编译器对程序做更多的优化。
如果在运行时，noexecpt函数向外抛出了异常（如果函数内部捕捉了异常并完成处理，这种情况不算抛出异常），程序会直接终止，调用std::terminate()函数，该函数内部会调用std::abort()终止程序。

```c++
MemoryPool(const MemoryPool& memoryPool) noexcept;
constexpr initializer_list() noexcept
      : _M_array(0), _M_len(0) { }
```

##### C++异常处理

C++中的异常处理是在运行时而不是编译时检测的。为了实现运行时检测，编译器创建额外的代码，然而这会妨碍程序优化。
在实践中，一般两种异常抛出方式是常用的：

- 一个操作或者函数可能会抛出一个异常
- 一个操作或者函数不能抛出异常

后面这种方式在C++ 11之前常使用throw()表示，在C++11中已经被noexcept代替。

```c++
void swap(Type& x, Type& y) throw()   //C++11之前
{
	x.swap(y);
}
void swap(Type& x, Type& y) noexcept  //C++11
{
	x.swap(y);
}
```

##### 什么时候使用noexcept?

使用noexcept表明函数或操作不会发生异常，会给编译器更大的优化空间。然而，并不是加上noexcept就能提高效率,以下情形鼓励使用noexcept:

- 移动构造函数（move constructor）
- 移动分配函数（move assignment）
- 析构函数（destructor）:在新版本的编译器中，析构函数是默认加上关键字noexcept的。下面代码可以检测编译器是否给析构函数加上关键字noexcept。

```c++
struct X{
	~X() { };
};   
int main()
{
	X x;
	// This will not fire even in GCC 4.7.2 if the destructor is
	// explicitly marked as noexcept(true)
	static_assert(noexcept(x.~X()), "Ouch!");
}
```

