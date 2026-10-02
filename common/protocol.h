#pragma once

#include <cstdint>
#include <cstddef>
//客户端/服务端公共通信协议
namespace protocol
{
    //协议魔数
    constexpr std::uint32_t MAGIC = 0x12345678;
    //单个body允许的最大长度
    constexpr std::uint32_t MAX_BODY_LENGTH = 1024 * 1024;
    //消息类型
    enum class MessageType : std::uint8_t
    {
        UNKNOWN = 0,
        //登录
        LOGIN_REQUEST = 1,
        LOGIN_SUCCESS = 2,
        LOGIN_FAILED = 3,
        //注册
        REGISTER_REQUEST = 4,
        REGISTER_SUCCESS = 5,
        REGISTER_FAILED = 6,
        //公共聊天
        CHAT_SEND = 7,
        CHAT_RECV = 8,
        //用户上下线
        USER_JOIN = 9,
        USER_LEAVE = 10,
        //在线用户列表
        USER_LIST_REQUEST = 11,
        USER_LIST_RESPONSE = 12,
        //退出登录
        LOGOUT_REQUEST = 13,
        LOGOUT_SUCCESS = 14,
        LOGOUT_FAILED = 15,
        //私聊
        PRIVATE_CHAT_SEND = 16,
        PRIVATE_CHAT_RECV = 17
    };

#pragma pack(push, 1)
    struct Header
    {
        std::uint32_t magic;
        std::uint8_t type;
        std::uint32_t bodyLength;
    };
#pragma pack(pop)

    constexpr std::size_t HEADER_SIZE = sizeof(Header);
}