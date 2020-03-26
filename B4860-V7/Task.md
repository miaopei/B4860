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



- [x] BBU HUB 时延数据上报消息通道完成，BBU 记录 HUB 九个端口TBdelayUL，TBdelayDL，T14并进行维护，如果hub掉线需要对HUB 延时测量数据map进行对应删除。HUB 时延测量数据map key用 level+type+port（hub层级+延时类别(TBdelayUL,TBdelayDL,T14)+端口号） 作为索引。
- [x] arraycomm接口获取 BBU T14 代码熟悉，目前代码基本实现，待测试。
- [x] RRU 时延测量上报
- [ ] BBU 侧时延补偿算法实现，提供最大时延配置接口。目前算法麻烦的地方是级联情况的补偿即计算，根据刘丙洋反馈，BBU 侧需要实现RRU接入后更新RRU上一级HUB的所有时延信息，RRU在收到BBU的connect success消息后需要 延迟500ms 再发送时延测量请求。
- [ ] 查询 RRU 完整链路信息接口实现
- [ ] HUB RRU 升级消息定义 接口实现
- [ ] OAM 适配层实现 
- [ ] 项目概设文档更新





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

