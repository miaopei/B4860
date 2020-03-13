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
- [ ] HUB client 调用提供的API实现消息头rruid port uport封装（接口代码已经拿到，还需要根据提供的代码实现库，cmake编写学习）
- [ ] BBU HUB 时延测量算法实现，时延消息定义，消息解析封装
- [ ] BBU HUB 设备联调
- [ ] OAM 适配层实现 
- [ ] 项目概设文档更新





<i class="fab fa-weixin fa-lg"></i> wxmiaopei



<span class="fa-stack fa-2x">
  <i class="fas fa-square fa-stack-2x"></i>
  <i class="fas fa-terminal fa-stack-1x fa-inverse"></i>miaow</span>


