ksock.so
总体功能：实现socket收发数据 支持TCP UDP两种协议 支持多线程使用（这可能是一句废话）
尽可能做成一个动态库
本库不提供消息协议，由调用者自行约定
适用环境：Linux macOS

具体功能


收发信息队列，支持IPv6


TCP协议可能随时要让调用者获取它的状态

UDP的每次发送都要返回状态

每创建一个socket连接，返回的可能不是Linux提供的socket文件描述符fd，有可能是本库自己
fd集合的一个句柄，目前不通过改变系统环境配置，能影响socket连接量的是改变socket buffer
size，本库可能会考虑提供相关方面的接口加以控制

上述的fd集合需要维护的是一个包含fd的一个结构体，该结构体包含所有操作所有需要使用的东西，
甚至包括这个fd目前的状态，一些状态信息可能需要用位信息来表示，如果是这样，将提供一些宏
操作，用来捡取信息

待定：
本库的依赖，两种选择，
1）依赖文件  缺点： 意味着有文件IO问题需要解决，可能会影响性能， 在多线程中异步IO是不安全
的。  优点：使用简单，使用者并不关心库里头的具体情况，即调用只需使用但不用维护本库
2）依赖内存  优点：性能可能略高（有待验证） 缺点：一旦使用本库，就要担负起维护本库的责任 
3)正在考虑，是否提供一种消息消息协议，若提供，暂定google protobuf
4)本库最好配合日志系统一起使用，方便调试，查看一些特定的错误
5）本库像Linux系统一样，如果发生错误，同时设置错误信息
6）当一方主动关闭连接后，另一方如何及时通过node获取到该信息。

本库是否需要定期清理坏的fd（难度在于定期清理的条件），抑或由调用者来自行清理 （一定程度来说
这并不是调用者需要关系的，或者说调用者无暇关心）

创建监听：
建立一个本库设计的struct，初始化完成，加入本库维护的fd集合中，把句柄抛出，提供开启监听的方法
（这个过程任何异步出现异常，皆抛出，并设置errorno）

创建连接：
建立一个本库设计的struct，初始化完成，加入本库维护的fd集合中，把句柄抛出，提供连接的方法
（这个过程任何异步出现异常，皆抛出，并设置errorno）


（以下每一个方法除了要完成相应的操作，更要更新struct状态，这个甚至比
比完成任务更重要，任务可以不完成，一旦状态没有对应更新，意味着本库这个struct处于坏的状态，
那么后续的工作将无法进行）
监听：

连接：
accept  调用者只需启动接收接连即可，随后要定时去取出已接收到的socket fd，否则accept缓存
        队列满了将不再接收链接，并且给client socket返回指定错误后断开链接

connect 与原生socket不同，这里连接可能会被服务端socket主动关闭，原因是服务端程序没有及时
        处理连接，见accept

发送：

接收：
recv    本函数将不再阻塞，改成从一个队列中去获取，当队列满了，消息将会堵塞，但不会丢失。

关闭：
close   关闭连接，涉及到的资源都会销毁或者回收，hd将会被回收。

待调整：
不再将ksock_connect_node暴露给使用者并让使用者去维护这个node，而是返回一个句柄hd，调用者通过
hd去做所有的操作，ksock_connect_node改为由ksocket维护。
目前已经完成这部分工作，但是貌似还不太稳定，有出现段错误的可能性，需要后去去排查原因



期望：
尽量将ksock.c代码量控制在1000行以内