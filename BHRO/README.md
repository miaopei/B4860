> Title：B4860 项目开发记录
> Author:  苗沛
> Email：miaopei@baicells.com
> Data：2020-04-23

## B4860 

> [markdown 高级画流程图方法](https://www.runoob.com/markdown/md-advance.html)

## Reference

> [深入浅出现代Web编程](https://fullstackopen.com/zh/)

> [libuv documentation](http://docs.libuv.org/en/v1.x/index.html)
>
> [C++单元测试工具——doctest](https://blog.csdn.net/liao20081228/article/details/76984975)
>
> [JSON for Modern C++](https://www.jianshu.com/p/69e57f2af904)
>
> [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
>
> [手把手教你实现自定义的应用层协议](https://segmentfault.com/a/1190000008740863)
>
> [手把手教你实现自定义的应用层协议](https://blog.csdn.net/ACb0y/article/details/61421006)
>
> [C++ TCP socket心跳包示例程序](https://blog.csdn.net/qq_19004627/article/details/79871665)

> [interview](https://github.com/huihut/interview)

> 测试：
>
> [**如何定义变长的TLV结构体？**](http://www.cppblog.com/aa19870406/archive/2012/06/14/178803.html)
>
> [结构体中最后成员为一个零长数组与一个指针](http://wenboo.site/2017/09/05/结构体中最后成员为一个零长数组与一个指针/)
>
> [解析变长结构体的用法和优点](https://blog.csdn.net/Move_now/article/details/71929225)
>
> [深入浅出变长结构体](https://blog.csdn.net/wojiushiwo987/article/details/11908731)
>
> [可变长的结构体](https://blog.csdn.net/wasd6081058/article/details/7216266)
>
> [Dale工作学习笔记](https://www.cnblogs.com/anker/p/3744127.html)
>
> [第2篇:C/C++ 结构体及其数组的内存对齐](https://www.jianshu.com/p/666852837034)
>
> [linux下jsoncpp的编译安装](https://blog.csdn.net/x2017x/article/details/92761750)

### 工程编译

#### Yocto 基本编译命令：

1 编译之前，首先生成bitbake 以及相关编译脚本（注：终端关闭后需要重新生成）  在poky目录下执行

```shell
$ source ./fsl-setup-poky -m <machine> 
# machine  : b4860qds  b4860qds-64b  分别生成4860的 32位  64位编译目录
```

2 编译镜像

在编译目录 （`poky/build_<machine>_release` ）下执行 

```shell
$ bitbake fsl-image-core
# 镜像生成目录在 poky/build_<machine>_release/tmp/deploy/images/<machine>/fitImage
```

这里包含文件系统  uboot  内核 rcw 镜像

3 编译单独软件包

在编译目录 （``poky/build_<machine>_release`）下执行 

```shell
$ bitbake -c compile <package-name> 
```

也可以 clean 单独软件包 以及重新生成编译代码

```shell
$ bitbake -c cleansstate <package-name> 
$ bitbake -c patch <package-name> 
```

#### 软件包的添加

1 首先确认yocto项目是否支持你要添加的开源项目，根据如下命令来搜索

在编译目录下执行 

```shell
$ bitbake -s | grep <package-name>  
```

这里的package-name可以模糊匹配

如果 yocto 不支持此开源软件包，则需要手动添加

2 手动先加开源软件包

目前版本支持从本地获取软件包，可将本地软件包放在sources中

如果不想打patch也可以直接解压缩到编译规则目录

开源软件包的编译规则可统一放在 `meta-fsl-ppc/recipes-baicellspkt` 下



脚本位置：

```shell
$ ..../poky/meta/recipes-core/initscripts/initscripts-1.0/rrumngr_start
```

软件包存放位置：

```shell
$ ..../poky/meta/recipes-core/base-files/base-files/
# bb 文件要加入 软件包的存放位置
```



### image烧录

```shell
$ scp miaopei@192.168.2.76:/home/miaopei/fitImage .
$ flash_eraseall /dev/mtd3
$ nandwrite -p /dev/mtd3 fitImage
```



## RRU


[ftp client lib github](https://github.com/mkulke/ftplibpp)

[ti-processor-sdk](http://software-dl.ti.com/processor-sdk-linux/esd/AM335X/06_00_00_07/index_FDS.html)





## 编译安装jsoncpp
下载 jsoncpp 源码

```shell
$ wget https://github.com/open-source-parsers/jsoncpp/archive/0.10.1.zip
```

解压缩源码文件

```shell
$ mv 0.10.1.zip jsoncpp-0.10.1.zip
$ unzip jsoncpp-0.10.1.zip
```

编译安装

```shell
$ cd jsoncpp-0.10.1
$ mkdir -p ./build/debug
$ cd ./build/debug
$ cmake \
-D CMAKE_BUILD_TYPE=debug \
-D BUILD_STATIC_LIBS=ON \
-D BUILD_SHARED_LIBS=OFF \
-D CMAKE_INSTALL_PREFIX=/usr/local/jsoncpp \
-D CMAKE_C_COMPILER=/usr/local/bin/gcc \
-D CMAKE_CXX_COMPILER=/usr/local/bin/g++ \
-G "Unix Makefiles" ../..
$ make && make install
```

```shell
$ mkdir -p build/debug
$ cd build/debug
$ cmake -DCMAKE_BUILD_TYPE=debug -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=ON -DARCHIVE_INSTALL_DIR=. -G "Unix Makefiles" ../..
$ make
// 查看生成的静态和动态链接库
$ find . -name *.a
./src/lib_json/libjsoncpp.a
$ find . -name *.so
./src/lib_json/libjsoncpp.so
```

