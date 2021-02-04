### cmake常用命令

#### 常用的API

##### cmake_minimum_required

```shell
#设置CMake最低版本号，如果当前版本的CMake低于所需的值，它将停止处理项目并报告错误，需要在<project>命令之前调用该命令
cmake_minimum_required(VERSION major.minor[.patch[.tweak]]
                       [FATAL_ERROR]); #命令格式
cmake_minimum_required(VERSION 2.8.5);
```

##### project

```shell
#指定工程名称，并可指定工程支持的语言，支持语言列表可以忽略，默认支持所有语言
project（projectName）
```

##### set

```shell
#定义变量
set(VAR [VALUE] [CACHE TYPE DOCSTRING [FORCE]]);
set(SRC_LIST main.c util.c reactor.c);
```

##### message

```shell
#向终端输出用户定义的信息或者变量的值
#SEND_ERROR, 产生错误,生成过程被跳过
#STATUS, 输出前缀为—的信息
#FATAL_ERROR, 立即终止所有cmake过程
message([SEND_ERROR | STATUS | FATAL_ERROR] “message to display” …)；
message(${SRC_LIST})；
message("This is a message");
```

##### add_exectuable

```shell
#生成可执行文件
#参数介绍：
#name：
#该命令调用列出的源文件来构建的可执行目标。 对应于逻辑目标名称，在项目中必须是全局唯一的。构建的可执行文件的实际文件名是基于本机平台的约定。
#WIN32：
#如果给出WIN32，则在创建的目标上设置属性WIN32_EXECUTABLE。
#MACOSX_BUNDLE:
#如果给定MACOSX_BUNDLE，将在创建的目标上设置相应的属性。
#EXCLUDE_FROM_ALL：
#如果给定EXCLUDE_FROM_ALL，将在创建的目标上设置相应的属性。
#source：
#源码列表
add_executable(<name> [WIN32] [MACOSX_BUNDLE][EXCLUDE_FROM_ALL] source1 [source2 ...]);
add_executable(bin_file_name ${SRC_LIST});
```

##### add_definitions

```shell
#为源文件的编译添加由-D引入的宏定义
add_definitions(-DFOO -DBAR ...)；#命令格式
add_definitions(-DWIN32); #使用示例
```

##### add_dependencies

```shell
#为顶级目标依赖于其他顶级目标，以确保它们在该目标之前构建。这里的顶级目标是由add_executable，add_library或add_custom_target命令之一创建的目标。
add_custom_target(mylib DEPENDS ${MYLIB})
add_executable(${APP_NAME} ${SRC_LIST})
add_dependencies(${APP_NAME} mylib)
```

##### add_library

```shell
#使用指定的源文件给项目添加一个库
#命令格式为
add_library(<name> [STATIC | SHARED | MODULE]
            [EXCLUDE_FROM_ALL]
            source1 [source2 ...]
#使用示例
add_library(HelloCMake hello_cmake.c)

```

##### add_subdirectory

```shell
#向构建中添加子目录
add_subdirectory(source_dir [binary_dir]
                 [EXCLUDE_FROM_ALL])
#使用示例
add_subdirectory(${SRC_ROOT})

```

##### **aux_source_directory**

```shell
#查找目录中的所有源文件
#查找指定目录dir中所有源文件的名称，并将列表存储在提供的variable中。
aux_source_directory(<dir> <variable>)
#使用示例
aux_source_directory(. DIR_SRCS)
add_executable(${APP_NAME} ${DIR_SRCS})
```

##### include_directory

```shell
#包含其他目录的CMakeLists.txt
#命令格式为
include(<file|module> [OPTIONAL] [RESULT_VARIABLE <VAR>]
                      [NO_POLICY_SCOPE])
#使用示例
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/cocos
  ${CMAKE_CURRENT_SOURCE_DIR}/cocos/platform
  ${CMAKE_CURRENT_SOURCE_DIR}/extensions
  ${CMAKE_CURRENT_SOURCE_DIR}/external
)
```

##### link_directory

```shell
#指定连接器查找库的路径。
#命令格式为
link_directories(directory1 directory2 ...)
#使用示例
link_directories(${PROJECT_SOURCE_DIR}/lib)
```

#### 常用的变量

```shell
#使用${}进行变量的引用。例如：message(${Hello_VERSION})，Hello为工程名。CMake提供了很多有用的变量。以下仅列举常用的变量：
CMAKE_BINARY_DIR：构建树的顶层路径
CMAKE_COMMAND：指向CMake可执行文件的完整路径
CMAKE_CURRENT_BINARY_DIR：当前正在被处理的二进制目录的路径。
CMAKE_CURRENT_SOURCE_DIR：指向正在被处理的源码目录的路径。
CMAKE_HOME_DIRECTORY：指向源码树顶层的路径。
CMAKE_PROJECT_NAME：当前工程的工程名。
CMAKE_ROOT：CMake的安装路径。
CMAKE_SOURCE_DIR：源码树的顶层路径。
CMAKE_VERSION：cmake的完整版本号。
PROJECT_BINARY_DIR：指向当前编译工程构建的全路径。
<PROJECT-NAME>_BINARY_DIR：指向当前编译工程构建的全路径。
<PROJECT-NAME>_SOURCE_DIR：指向构建工程的全路径。
PROJECT_SOURCE_DIR：指向构建工程的全路径。
PROJECT_NAME：project命令传递的工程名参数。
<PROJECT-NAME>_VERSION：项目的完整版本号。
```

