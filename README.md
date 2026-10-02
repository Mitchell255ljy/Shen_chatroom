# 神的聊天室

## 项目简介

这是一个基于 **C++ / Qt** 的多人网络聊天室系统，采用 **客户端 / 服务器（C/S）** 架构。服务器端使用 **epoll + 线程池** 的高并发模型，客户端使用 **Qt Widgets** 构建无边框深色主题界面。支持用户注册、登录、公共聊天、私聊、在线用户列表、上下线通知等功能。

## 技术栈

### 客户端

| 分类       | 技术                                     |
| :--------- | :--------------------------------------- |
| 语言       | C++11                                    |
| GUI 框架   | Qt 6（QtWidgets）                        |
| 网络       | QTcpSocket（异步信号槽）                 |
| 数据序列化 | QJsonDocument / QJsonObject / QJsonArray |
| 构建       | CMake                                    |
| 界面风格   | QSS                                      |

### 服务器

| 分类         | 技术                                    |
| :----------- | :-------------------------------------- |
| 语言         | C++11                                   |
| 网络模型     | epoll（LT 模式）+ 非阻塞 IO             |
| 并发         | 自定义线程池                            |
| 线程通信     | eventfd + SendQueue（线程安全发送队列） |
| 数据库       | MySQL 8.0                               |
| 数据库连接池 | 自实现 MysqlPool（互斥锁 + 条件变量）   |
| JSON 库      | jsonhpp                                 |
| 配置         | 自定义 ConfigManager 读取 `.conf` 文件  |
| 构建         | CMake                                   |

## 通信协议

- 二进制帧头 + JSON 消息体
- 帧头结构（`#pragma pack(1)`）：

```
struct Header {
    uint32_t magic;       // 0x12345678
    uint8_t  type;        // 消息类型
    uint32_t bodyLength;  // body 字节数
};
```



- 消息类型（`protocol::MessageType`）：

| 值           | 类型                                | 说明         |
| :----------- | :---------------------------------- | :----------- |
| 1 / 2 / 3    | LOGIN_REQUEST / SUCCESS / FAILED    | 登录         |
| 4 / 5 / 6    | REGISTER_REQUEST / SUCCESS / FAILED | 注册         |
| 7 / 8        | CHAT_SEND / CHAT_RECV               | 公共聊天     |
| 9 / 10       | USER_JOIN / USER_LEAVE              | 用户上下线   |
| 11 / 12      | USER_LIST_REQUEST / RESPONSE        | 在线用户列表 |
| 13 / 14 / 15 | LOGOUT_REQUEST / SUCCESS / FAILED   | 退出登录     |
| 16 / 17      | PRIVATE_CHAT_SEND / RECV            | 私聊         |

- JSON 字段名由 `common/message_schema.h` 统一管理。

## 实现的功能

### 用户模块

- 用户注册（用户名 6 位数字、昵称、密码；用户名唯一校验）
- 用户登录（数据库校验密码、重复登录检测）
- 退出登录（服务端解绑 session，广播 USER_LEAVE）

### 聊天模块

- 公共聊天：消息广播给所有在线用户
- 私聊：点击在线用户列表进入私聊模式，消息点对点发送

### 在线状态

- 登录成功后广播 `USER_JOIN`，其他客户端实时更新列表
- 退出/断线自动广播 `USER_LEAVE`
- 在线用户列表实时刷新（带数量统计）

### 界面

- 登录 / 注册 / 聊天三窗口切换（WindowManager 管理）
- 无边框窗口 + 自定义标题栏（最小化 / 关闭）
- 深色圆角 QSS 主题
- 网络异常、登录/注册失败等消息提示

### 服务器

- epoll 高并发 IO 多路复用
- 线程池处理业务逻辑
- eventfd 唤醒 + 发送队列，实现线程安全异步发送
- MySQL 连接池，避免频繁建连
- SQL 转义防注入
- 支持 TCP 粘包/半包处理

## 环境要求

- Linux（服务器端依赖 epoll / eventfd）
- Qt 6.5.3
- MySQL 8.0
- CMake 3.10+
- g++ 支持 C++11

## 使用方式

### 客户端

* ./client/src是客户端源码，若安装了Qt，则可使用CMakeLists.txt构建项目,运行前先将server.conf中的服务端IP和端口号修改成自己的。
* ./client/exec是编译后的可执行文件以及依赖的库，运行前先修改./client/exec/config/server.conf。

### 服务端

1. 安装gcc、g++、cmake。

```
sudo yum install gcc
sudo yum install gcc-c++
sudo yum install cmake
```

2. 安装MySQL 8.0.25。
3. 安装MySQL连接库。

```
sudo yum install -y https://dev.mysql.com/get/mysql80-community-release-el7-10.noarch.rpm
sudo yum install -y mysql-community-devel
```

4. 创建数据库user，使用./sql/user_information.sql在user中创建一张表。
5. 将./server/config中的配置信息修改成自己的。
6. 在./server中执行以下命令

```
mkdir build
cd build
cmake ..
make
./Shen_chatroom
```



## 总结

本项目主要目的是熟悉c++和linux网络编程，目前还有很多不完善之处，例如数据库中直接存明文密码，网络上也是明文JSON，未做哈希与TLS加密，客户端无群组、好友等常见IM功能等，后期考虑优化。