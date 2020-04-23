## B4860 

## Reference

> [libuv documentation](http://docs.libuv.org/en/v1.x/index.html)
>
> [C++单元测试工具——doctest](https://blog.csdn.net/liao20081228/article/details/76984975)
>
> []()

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

