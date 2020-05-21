> Title： EU、RU 升级工具 oam_adapter 使用说明
> Author: 苗沛
> Email：miaopei@baicells.com
> Data：2020-04-23


## oam_adapter 使用说明

```shell
# oam_adapter 使用介绍
# 启动 oam_adapter cli，ip为 ceth 网卡网关地址
$ oam_adapter 10.0.0.1
[OamAdapter Connect BBU Success]
OamAdapter >
# 启动 oam_adapter cli 失败的可能原因是 bbu 服务没有启动。 ps aux | grep bbu 查看一下服务是否没有启动。 bbu server 没有启动的 可能原因是 ceth 网卡没有激活，或者是arraycomm 4.9G的几个bin文件没有导入。
# 查看使用说明
OamAdapter > help
Cli Usage:
  [help]: print cli usage
  [upgrade device fileName md5]: upgrade all device
	 device: 0 - HUB, 1 - RRU
	 fileName: upgrade file name, include path
	 md5: file md5
	 note: rru upgrade fileName passes in the directory path, and md5 set any numbers
	 eg: 
		 hub upgrade: upgrade 0 rHUP.tar f119e2d0e5e273e9dd2e2557d57dac57
		 rru upgrade: upgrade 1 rru 123
  [upgrade device routeIndex fileName md5]: upgrade device
	 device: 0 - HUB, 1 - RRU
	 routeIndex: Obtained through get_topo command
	 note: rru upgrade fileName passes in the directory path, and md5 set any numbers
	 eg: 
		 hub upgrade: upgrade 0 0_1_0_0 rHUP.tar f119e2d0e5e273e9dd2e2557d57dac57
		 rru upgrade: upgrade 1 0_1_0_0 rru 123
  [get_topo]: get network topology
	 Source: 0 - HUB, 1 - RRU
	 HOP: Device level
	 UpgradeState: 
		 0 - normal
		 1 - upgrade success connect
		 2 - upgrade failure connect
		 3 - upgrade ing...
		 4 - ftp download failure
		 5 - upgrade command failure
  [RFTxStatus_set RFTxStatus]: Power amplifier, antenna power Settings
	 RFTxStatus: 0 - OFF, 1 - ON
  [RFTxStatus_set routeIndex RFTxStatus]: Power amplifier, antenna power Settings
	 routeIndex: Obtained through get_topo command
	 RFTxStatus: 0 - OFF, 1 - ON
  [exit]: exit program
# 升级说明
# ftp server路径为 /mnt/ftp/,升级前需要把所要升级的软件包放在/mnt/ftp/ 目录下。rru 的软件包需要解压到/mnt/ftp/ 目录下。比如 rru 的软件包解压为目录名叫 rru。
$ ls /mnt/ftp/
rHUP.zip 	rru
$ ls /mnt/ftp/rru
xxx.img   xxxx.img

# oam_adapter 升级 所有的 hub 设备
# md5值可以通过B4860系统命令行计算：
# $ md5sum /mnt/ftp/rHUP.tar
# f119e2d0e5e273e9dd2e2557d57dac57  /mnt/ftp/rHUP.tar
OamAdapter > upgrade 0 rHUP.tar f119e2d0e5e273e9dd2e2557d57dac57

# oam_adapter 升级 所有的 rru 设备, rru 为目录，md5输入任意数字，代码中会计算目录下文件的 md5
OamAdapter > upgrade 1 rru 123

# RouteIndex 通过 oam_adapter get_topo 指令获取
OamAdapter > get_topo
 	IP				  MAC			Source	HOP	 UpgradeState	RouteIndex
 	10.0.0.123:49260  38D2693D70BB 	0 		1 	 0 				0_0
 	10.0.0.30:49260   38D2693D70CC 	1 		2 	 0 				0_1_0_0

# oam_adapter 升级 指定的 hub 设备
OamAdapter > upgrade 0 0_0 rHUP.tar f119e2d0e5e273e9dd2e2557d57dac57

# oam_adapter 升级 指定的 rru 设备， rru 为目录，md5输入任意数字，代码中会计算目录下文件的 md5
OamAdapter > upgrade 1 0_1_0_0 rru 123

# oam_adapter 开功放，设置天线功率
# 设置所有 rru 设备 开功放
OamAdapter > RFTxStatus_set 1

# 设置所有 rru 设备 关功放
OamAdapter > RFTxStatus_set 0

# 指定 rru 设备 开功放
OamAdapter > RFTxStatus_set 0_1_0_0 1

# 指定 rru 设备 关功放
OamAdapter > RFTxStatus_set 0_1_0_0 
```