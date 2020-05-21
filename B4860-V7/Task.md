---
Title：B4860 开发任务
Author： MiaoPei
Data：
---

## B4860 开发任务

- [x] BBU 工程项目整理，把libuv-v1.9.1 开源包编译到工程中，工程 Makefile 整理
- [x] BBU HUB 消息测试，根据测试情况将 设备type，rruid，port内容封装到消息中，不需要在dnsmasq中维护。
- [x] B4860 项目概设文档更新，去掉dnsmasq的相关操作描述，增加BBU Server系统架构，消息处理流程图，以及通道建立流程图。消息相关描述。Client信息拓扑表。
- [x] Packet 操作封装
- [x] BBU Client Connect map 封装
- [x] BBU Server OnMessage 消息解析处理，分发处理（**基本简单消息处理测试完成，还有很多细节需要处理**）
- [x] BBU server 消息头重新定义增加target port uport 增加 oam 消息定义，更新已连接hub rru oam socket存储；client info 信息存储，可以给 web 提供，展开可以实现完成的拓扑。hub 实现跟新消息头封装解析
- [x] HUB Client 消息解析，消息处理
- [x] BBU 代码打包，编译（程序已在板子上跑起来了，还需要实现守护进程，参数配置等功能）
- [x] HUB 代码打包编译（环境完成，rootfs已经生成还未烧到板子上调试）



- [x] HUB client 调用提供的API实现消息头rruid port uport封装（接口代码已经拿到，还需要根据提供的代码实现库，cmake编写学习）
- [x] BBU HUB RRU 设备联调，connect建立完成，网络拓扑map测试通过，设备掉线断网拓扑更新测试通过


[libuv 简单使用](https://www.cnblogs.com/lsgxeva/p/9999135.html)

[map按键排序和按值排序](https://www.jianshu.com/p/809216c9240d)

[C++ STL中Map的按Key排序和按Value排序](https://blog.csdn.net/iicy266/article/details/11906189)

[C++中map按value排序](https://blog.csdn.net/theonegis/article/details/48549887)

[C++模块日志](https://juejin.im/post/5c04c54ae51d452209013f71)

[C/C++ 定制自己的 log 输出工具。使用宏命令配合 __VA_ARGS__ 完成](https://blog.csdn.net/afei__/article/details/82682857)


- [x] HUB 时延数据上报，消息封装，上报时延数据包括HUB 九个端口TBdelayUL，TBdelayDL，T14，通过一条消息全部上报。功能测试完成
- [x] BBU解析处理HUB上报时延测量数据，数据解析接口封装，数据存储为map，key为level+type+port（hub层级+延时类别(TBdelayUL,TBdelayDL,T14)+端口号） 作为索引。功能测试完成。
- [x] BBU 实现了HUB掉线删除对应HUB时延测量数据map。功能测试完成。
- [x] BBU 实现 RRUconnect请求时更新上级HUB延时测量数据，接口实现，未测试验证。实现根据RRU rruid查询上一级hub相关信息接口。
- [x] arraycomm接口获取 BBU T14 代码熟悉，目前代码基本实现，B4860 32位环境编译有些问题，还在处理环境编译问题。
- [x] RRU 时延测量数据上报（张云松已经完成，目前BBU暂时无法提供测试接口，无法联调）





- [x] SNMP项目 Jesse 关于SNMP 配置的问题查找资料解决。配置方面双方理解的有些偏差。
- [x] 网优宝项目 1.3版本发布测试问题解决。网优宝 release 版本存根 doc svn规划。
- [x] 完成 RRU Connect success，BBU更新 RRU上级 HUB 所有的delay信息。BBU HUB RRU 各端测试完成。BBU侧接口完成部分封装，HUB端还未进行封装。 
- [x] HUB RRU 参数支持确定，升级方案确定。
- [x] arraycomm接口获取 BBU T14 代码完成，测试单小区数据偏差比较大。
- [x] arraycomm接口获取 BBU T14 双小区操作实现。分小区测试T14，外环数据比较稳定，对接hub测试数据抖动较大，抖动问题还在沟通中。



- [x] BBU 时延补偿算法实现，按照同级只有一个hub实现了补偿值计算。按照之前的拓扑如果同级有多个hub，同级rru区分不了连接的是哪一个hub，刘丙洋提供了接口时延补偿算法还得重新实现。
- [x] BBU 头部消息增加设备 mac 地址，消息处理优化。 
- [x] BBU 消息粘包，残包问题解决。
- [x] rru 最大时延数据存储，实现排序获取最大时延。rru设备离线实现对应最大时延数据清空。
- [x] 优化不同级别rru接入更新上级hub时延数据。（这部分还需要优化，hub下边挂多个rru需要更新其他rru最大时延）
- [x] 实现 rru 完整链路描述，eg：RouteIndex = 0_3_0_1_0_2（rru port0 接 hub port3，hub port0 接级联hub port1，级联hub port0 接 bbu port2）这部分需要根据刘丙洋提供接口优化。还是同级别多个hub，rru无法区分的问题。
- [x] BBU HUB(3个) RRU(3个) 不同级别测试，rru上线下线 时延补偿动态更新。测试ok。



- [x] BBU 升级接口实现，实现hub 、 rru 设备升级，消息接口可以指定升级所有的还是指定升级某一个设备。实现md5校验封装 ，system封装，文件读写封装，优化消息框架。
- [x] oam_adapter 命令行工具实现：目前实现的有 通过oam_adapter 可以对 hub 、rru进行升级操作，可以获取当前整个网络的拓扑情况，设置rru 工具开启或者关闭。通过拓扑可以知道升级是否成功以及升级的状态。提供help指令查看当前支持得指令使用。
- [x] HUB 实现 ftp 功能，封装 ftp 编译成库，实现 md5 计算封装
- [x] HUB 实现升级消息处理，对升级错误进行反馈，升级消息功能测试ok，img 刘丙洋那边暂时不能提供，暂时无法测完整的升级。
- [x] BBU实现对于oam_adapter 过来的 hub 升级，rru升级消息处理。rru升级比较麻烦一些，bbu这边根据传入的目录，遍历目录下的所有问题件，代码封装，计算 文件md5，封包给 rru。



- [x] bbu rru 部分消息优化，rru升级部分特殊，单独处理。
- [x] bbu hub rru 优化批量配置和单独配置接口
- [x] bbu hub rru oam_adapter联调测试，测试各个消息、升级功能、功放功率设置，测试ok
- [x] oam_adapter 使用说明文档编写
- [x] bbu 程序启动部分优化，实现网卡ip地址获取封装
- [x] hub 程序启动部分优化，实现网卡 ip 地址获取封装，mac获取优化。
- [x] hub 升级迭代问题和小罡确定，应用层这边提供hub 升级文件以及升级脚本操作，release_hubapp说明。小罡那边根据升级脚本做相应的copy。
- [x] hub 应用程序升级问题， 由小罡那边提供img 脚本解决应用程序升级的问题。产线出来的板子统一再升级一次



- [x] BBU doctest 实现单元测试，项目做大功能比较多的时候使用单元测试可以很好的避免很多不必要的问题。目前 doctest 只用在 utilities api测试。
- [x] oam 适配层讨论实现方案，确定 提供 oam适配层操作库，代码还在构建中，需要实现req通道和rsp通道。后续告警部分也需要实现通道。
- [x] BBU log 系统实现，封装默认输出log时间，函数和行数，可对log进行分类设置。
- [x] HUB log 系统实现，后边可以根据这个log接口实现 hub 日志提取。
- [x] BBU、HUB 所有的日志输出使用 log系统封装api包装。
- [x] 目前消息data 自定义key=value&key=value封装，打包和解包封装使用起来不是很灵活，这部分打算使用jsoncpp重写。已经完成基本的测试，可根据开发时间重构代码使用。







- [x] 网优宝 V1.4 版本功能开发确定，V1.4 版本计划完成市场需求，和现阶段的bug。 
- [x] oam 适配层  req通道和 rsp 通道框架基本实现，框架按照封装所有的操作思路实现，oam上层不需要关系适配层只会的消息头数据的封装，统一由适配层完成，适配层只提供操作指令和操作的参数说明。为以后项目迁移实现很好的接口。





- [x] 讨论确定oam 和 bbu 数据同步问题，有新设备上来后需要把设备的信息通过oam adapter 写入oam参数中，目前connect没有实现数据上报bbu的功能，这部分需要和ru hub 同步完善。
- [x] 完成 oam adapter 升级部分的实现。给oam提供封装好的类，初始化完之后可以使用里边的api，目前把软件包导入fap下边调试makefile编译。
- [x] 修改hub 传ip 启动，网关的获取通过代码实现，测试ok
- [x] hub 断线重连部分优化，重连重新获取网关ip进行连接，目前断连后定时500ms重连。
- [x] 实现 bbu 和 hub之间的心跳包，bbu 设置 40 秒的心跳超时时间，hub 30秒发送一次心跳包，如果bbu超过40秒没有收到hub的心跳包则断开和hub的连接。（断开连接的数据处理未实现）
- [x] JRC 升级失败问题定位
- [x] 网优宝 v1.4 测试版本发布，新增软usim配置和链路回传功能，首页增加ue个数显示。







- [x] 宽带3.6 版本 Jesse 增加的 SNMP 配置确定
- [x] 日本 JRC SNMP 升级失败问题处理
- [x] 解决了 fap oam 代码中编译 adapter 的问题，已经打包提供给海涛。
- [x] bbu 状态机中增加了socket断连的回调处理，设备异常退出bbu可以把设备掉线信息收集处理，可通过状态机将设备断连消息转到 adapter中处理。
- [x] bbu 实现支持多个 adapter， 并支持多个adapter消息处理，可动态扩展。
- [x] 完成 adapter 设备connect 消息接口、connect close 消息接口、设备数据update 消息接口。可根据 oam 提供的回调做不同的处理
- [x] 消息头重新封装，使用结构体构造可灵活的设置不同消息的头。
- [x] 网优宝 初始化配置问题讨论，app 增加自动配置初始化服务器功能，和日志导出功能。计划1.5 版本支持。1.4 release 版本发布。计划下周五发布1.5 debug版本。
- [x] bbu、 adapter、 hub 日志输出优化，各端收发数据包封装解封装输出格式优化。
- [x] 网优宝 改版讨论，计划 1.6 版本对网优宝进行页面改版，目前的状况是新增的设置标签在一个页面已经放不下，需要滑动才能看到，用户操作体验不太好。改版 需要李冬雪重新设计app页面，app底部增加三个标签，分别是状态，设置，关于。状态页可以更多的显示一些状态信息，设置页可以更多的扩展不同的操作标签，关于页显示app 的版本以及app的版本信息。
- [ ] rru 同级 topo 的问题，刘丙洋新增的接口进行处理，这部分改动需要在头文件中新增一个字段。改动的地方比较多。
- [ ] 工程从bsp迁移到oam，主要需要解决的是makefile编译的问题，以及软件包安装的问题。
- [ ] 设备connect 消息上报设备状态信息和配置信息，消息由bbu 转发给oam adapter进行设置mib
- [ ] 时延测量根据刘丙洋提供的新接口重新实现。（这部分代码比较繁琐，需要rru跟着一起改）
- [ ] 项目概设文档更新
- [ ] HUB 掉线，RRU如何处理？BBU 断开HUB下所有RRU的连接？如果有级联级联设备也需要断开连接





## OAM Adapter 对接 OAM 想到的一些问题

- [ ] oam需要提供设置参数接口，设备连接上来后需要设置oam，把设备相关的信息填充。

- [ ] 设备掉线的时候需要清除oam中相关设备的配置，这个目前实现起来难度有些大，BU这边的实现没有主动上报的功能，都是socket消息触发式的。

- [ ] oam通过适配层做升级或参数查询配置，数据是返回到接口，还是在适配层把返回设置到oam参数中？数据返回到接口需要oam做参数设置，这种容错性比较好。

- [ ] 适配层配置oam参数如果bu出问题，oam的数据会滞留，有可能会出现无效数据。

- [ ] 启动的问题，加了适配层后bu必须在oam之前启动。可能会出现的问题是设备连接后oam还没有初始化完成，导致数据不会写oam参数。

    



















1. rhub的hop号，rruid查询接口
2. rhub对应bbu的端口号，查询接口？
3. cpri 链路时延查询，rhub 级联口 T14  下联口 T14





HUB 时延测试量参数：

- Toffset
- TBdelay DL
- TBdelya UL
- DL/UL callbbu_max 







<i class="fab fa-weixin fa-lg"></i> wxmiaopei



<span class="fa-stack fa-2x">
  <i class="fas fa-square fa-stack-2x"></i>
  <i class="fas fa-terminal fa-stack-1x fa-inverse"></i>miaow</span>





Netconf 升级相关操作步骤：

1. 写升级的yang模型文件
2. 实现对应的rpc接口函数
3. 在netopeer中安装升级模型
4. 检查模块安装，启动netopeer服务
5. 准备对应操作的netconf框架的xml文件
6. netopeer-cli 测试相关





BBU升级：

1. 需要提供hub和rru升级命令行接口。-- 苗沛提供
2. ftp 服务提供  -- 张云松提供
3. ftp 服务初始下载路径  -- 张云松提供

RRU 升级：

1. IMG 提供  -- 刘丙洋提供
2. 升级命令  -- RRU收到 IMG 放在什么路径？ -- 刘丙洋提供
3. 存放升级状态 寄存器、存放升级版本 寄存器  -- 刘丙洋提供
4. 我们提供程序打包到IMG，程序启动脚本。系统每次重启都需要起这个程序。 -- 苗沛提供
5. 确定IMG必须有的软件。-- 付田确定

HUB升级：

1. IMG 提供  -- 刘丙洋提供
2. 升级命令  -- HUB收到 IMG 放在什么路径？-- 刘丙洋提供
3. 存放升级状态 、存放升级版本 掉电不丢失的路径。   ---  这部分应该规划一个目录来存放一些状态文件。  -- 刘丙洋提供
4. 我们提供程序打包到IMG，程序启动脚本。系统每次重启都需要起这个程序。-- 张云松提供
5. 确定IMG必须有的软件。-- 付田确定









1. 项目B4860 BBU IR 消息处理中心开发优化。
2. 项目B4860 OAM 适配层开发，对接OAM 实现对HUB和RRU的配置查询和设置。
3. HUB IR消息处理，事项对应消息功能。







- B4860 项目 IR 协议开发
    - BBU 消息处理中心开发优化
    - OAM 适配层开发， 对接OAM 实现对HUB和RRU的配置查询和设置。
    - HUB IR 消息处理，实现对应消息功能。

计划工作内容：

- 实现通过OAM 适配层实现对指定的HUB或RRU升级，也可以指定对所有的HUB或者RRU升级。
    - OAM适配层、BBU消息处理中心、HUB、RRU 升级消息处理，响应的功能实现。
    - HUB 升级功能开发，实现版本校验，升级状态记录，当前版本记录。升级完成后升级结果消息上报功能。
    - RRU 升级功能由张云松开发。
- 时延补偿实现了简单的拓扑模式，需要等刘丙洋实现同级多个hub区分的接口重新实现时延补偿算法。
- OAM适配层 - BBU消息处理中心 - HUB - RRU 实现消息查询设置。（目前已知需要查询设置参数不到20个） 





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
 	IP					MAC				Source	HOP		UpgradeState	RouteIndex
 	10.0.0.123:49260 	38D2693D70BB 	0 		1 		0 				0_0
 	10.0.0.30:49260 	38D2693D70CC 	1 		2 		0 				0_1_0_0
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



