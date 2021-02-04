# 工作笔记

### 1.linux 下调试core dump

```shell
ulimit -c #如果显示为0，则表示core dump 没有被开启
ulimit -c #unlimited #开启core dump
ulimit -a #产生core文件时程序运行的shell 环境

#查看core dump
gdb 可执行文件名 core文件
bt

#需要定位出错程序到行
在编译的时候开启-g调试开关
```

### 2.git 的一些操作

```shell
#git 查看某个文件的变动
git log --pretty=oneline 文件名

#git 强制覆盖本地修改
git fetch --all
git reset --hard origin/master
git fetch
```

3.