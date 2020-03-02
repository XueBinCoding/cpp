### 如何编写CMakeLists

#### CMake需要解决的问题

- 源文件在哪
- 头文件在哪
- 怎么生成动态库和静态库
- 程序连接的动态库和静态库在哪
- 如果工程的代码存放的地方很多，如何去找到他们

#### CMakeList的一个例子

假设文件的目录结构如下

```
.
├──  bin						//可执行文件输出目录
├──  build						//编译目录
├── CMakeLists.txt    			//本文重点
├── include						//extra目录
├── lib							//静态库目录
└── src							//源码目录
     ├── CMakeLists.txt			//二级重点
     ├── MyThread.cpp			//以下都是源文件
     ├── MyThread.hpp
     └── test.cpp

```

根文件目录下的CMakeLists.txt 的内容如下

```
cmake_minium_required(VERSION 3.5)

project(projectname)   #括号内为项目名称
	set(CMAKE_C_COMPILER "gcc")
	set(CMAKE_CXX_COMPILER "g++")
	set(CMAKR_OPTION "$ENV{MJOY_OPTION} -Werror -rdynamic")
	set(LIB_DIR ${pro_SOURCE_DIR}/lib)
    set(SRC_DIR ${pro_SOURCE_DIR}/src)
    set(PRO_DIR ${pro_SOURCE_DIR})
    set(INCLUDE_DIR ${pro_SOURCE_DIR}/include)
    link_directories(
    	$(INCLUDE_DIR)
    	$(PRO_DIR)
    	${LB_DIR}
    )
	set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})  #可执行输出目录
	set(LIBRARY_OUTPUT_PATH ${LIB_DIR})  #静态库输出目录
	    message("")
    message("====================================")
    message("   编译路径:")
    message("src        = Loading")
    add_subdirectory( src )						#增加一个子编译目录

    message("   编译选项:")
    message(" BUILD_TYPE    =" ${CMAKE_BUILD_TYPE})
    message(" CMAKE_OPTION  =" ${CMAKE_OPTION})
if (CCACHE_PROGRAM)
    message(" CCACHE_PROGRAM    = ON")
else()
    message(" CCACHE_PROGRAM    = OFF")
if (${CMAKE_MAKE_PROGRAM} MATCHES "make")
    message(" Generator <Ninja> = OFF")
    message(" ")
	
	
```

对于src下的子编译目录src/CMakeLists.txt

```
add_definitions(-std=c++11 ${CMAKE_OPTION})          #为源文件的编译添加由-D定义的标志                                                                   
set(CMAKE_EXE_LINKER_FLAGS "-Wall -lpthread")		 #源文件链接参数

aux_source_directory(. SOURCE)						 #源码路径
include_directories(								 #引用文件目录
        ${pro_SOURCE_DIR}/include
        )   
add_executable(src ${SOURCE})						 #执行文件名
target_link_libraries(src ${CMAKE_EXE_LINKER_FLAGS}) #链接文件,如lib库中还有需要链接的可以继续接后面写

```

