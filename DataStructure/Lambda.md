#### 1.Lambda 表达式

 Lambda 表达式用于定义并创建匿名的函数对象，以简化编程工作。

```c++
[函数对象参数] (操作符重载函数参数) mutable 或 exception 声明 -> 返回值类型 {函数体}
auto add = [](int a, int b) {return a + b};
```

##### 1.1函数对象

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

##### 1.2操作符重载函数参数

标识重载的 () 操作符的参数，没有参数时，这部分可以省略。参数可以通过按值（如: (a, b)）和按引用 (如: (&a, &b)) 两种方式进行传递。

##### 1.3 mutable 或者exception 声明

这部分可以省略。按值传递函数对象参数时，加上 mutable 修饰符后，可以修改传递进来的拷贝（注意是能修改拷贝，而不是值本身）。exception 声明用于指定函数抛出的异常，如抛出整数类型的异常，可以使用 throw(int)。

##### 1.4返回值类型

标识函数返回值的类型，当返回值为 void，或者函数体中只有一处 return 的地方（此时编译器可以自动推断出返回值类型）时，这部分可以省略。

##### 1.5函数体

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

