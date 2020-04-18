## ,并B4860 开发任务

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
- [ ] BBU整个消息框架优化，使用结构体，这部分还在摸索研究。
- [ ] oam 消息对接实现，这部分需要讨论如何实现。
- [ ] 时延测量根据刘丙洋提供的新接口重新实现。（这部分代码比较繁琐，需要rru跟着一起改）
- [ ] oam 状态参数查询接口，参数配置接口实现。

- [ ] 项目概设文档更新



- [ ] HUB 掉线，RRU如何处理？BBU 断开HUB下所有RRU的连接？如果有级联级联设备也需要断开连接





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







