# libevent使用

## 使用流程

1. 获取待监听的内容的fd；
2. 创建一个event_base；
3. 创建一个event,指定待监听的fd，待监听事件的类型，以及事件放生时的回调函数及传给回调函数的参数；
4. 将event添加到event_base的事件管理器中；
5. 开启event_base的事件处理循环；
6. （异步）当事件发生的时候，调用前面设置的回调函数。

## 事件处理框架`event_base`

### 基本操作

- 声明

  `struct event_base *base;`

- 创建

  `struct event_base* event_base_new(void);`

- 释放

  `event_base_free(struct event_base* base);`

- 循环监听base对应的事件, 等待条件满足

  `event_base_dispatch(struct event_base* base);`

## 事件`event`

### 创建事件

1. 创建事件

   ```c
   
   #define  EV_TIMEOUT         0x01    // 废弃
   #define  EV_READ            0x02
   #define  EV_WRITE           0x04
   #define  EV_SIGNAL          0x08
   #define  EV_PERSIST         0x10    // 持续触发
   #define  EV_ET              0x20    // 边沿模式
   
   typedef void(*event_callback_fn)(evutil_sockt_t,short,void *);
   
   struct event *event_new()
       struct event_base *base,
       evutil_socket_t fd,     // 文件描述符-int
       shord what,
       event_callback_fn cb,   // 事件处理动作
       void *arg
   );
   ```

2. 释放事件

   `void event_free(struct event *event);`

3. 设置未决事件（有资格但是没有被处理的事件）

   - 构造事件之后,在将其添加到 `event_base` 之前实际上是不能对其做任何操作的。使用`event_add()`将事件添加到`event_base`, 非未决事件 -> 未决事件.

     ```c
     int event_add(struct event *ev, const struct timeval *tv);
     ```

4. 设置非未决事件（还没有资格被处理的事件）

   `int event_del(struct event *ev);`

   对已经初始化的事件调用 event_del()将使其成为非未决和非激活的。如果事件不是未决的或者激活的,调用将没有效果。成功时函数返回 0,失败时返回-1。

## 事件循环



