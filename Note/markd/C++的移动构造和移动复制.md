# C++的移动构造和移动复制

C++ 11的标准中提供了一种新的构造方法--移动构造（Move constructor） 和移动赋值运算符(Move Assignment operator)

## 使用场景

C++11 之前，如果要将源对象的状态转移到目标对象只能通过复制。在某些情况下，我们有没有必要复制对象----只需要移动他们

移动语义：源对象资源的控制权全部交给目标对象

在临时对象被复制之后就不再被利用了，我们完全可以把临时对象的资源直接移动，避免了多余的复制操作。

##### 拷贝构造和移动构造

```c++
//拷贝构造函数
Trace()
{
    if(t.text  != nullptr)
    {
        int len = strlen(t.text);
        text = new char[len+1];
        strcpy(text,t.text);
    }
}
//移动构造函数
Trace(Trace&& t)
{
    if(t.text != nullptr)
    {
        text = t.text;
        t.text = nullptr;
    }
}
```

```c++
//拷贝赋值运算符
Trace& operator=(const Trace& rhs)
{
    if(this != &rhs)
    {
        free();
        if(rhs.text != nullptr)
        {
            int len = strlen(rhs.text);
            text = new char[len+1];
            strcpy(text,rhs.text);
        }
    }
    return *this;
}

//移动赋值运算符
Tracer& operator=(Tracer&& rhs) noexcept
{
    if(this !=&rhs)
    {
        free();
        text = rhs.text;
        rhs.text = nullptr;
    }
    return *this;
}

//使用实例
#define STDMOVE
int main()
{
    DWORD start = GetTickCount();
    Tracer t1("Happy new year!");
    for(int i=0;i<10000;i++)
    {
     #ifndef STDMOVE
        Tracer t2(t1);
        Tracer t3;
        t3 = t2;
     #else
        Tracer t2(std::move(t1));
        Tracer t3;
        t3 =std::move(t2);
     #endif
    }
    DWORD end = GetTickCount();
    cout<<end-start<<endl;
    retrun 0;
}
```

