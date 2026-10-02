cmake_minimum_required(VERSION 3.16)

project(Shen_chatroom LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
)

# 客户端程序
add_executable(Shen_chatroom

    # 程序入口
    src/main.cpp

    # UI
    src/ui/Login.h
    src/ui/Login.cpp

    src/ui/Register.h
    src/ui/Register.cpp

    src/ui/Chat.h
    src/ui/Chat.cpp

    src/ui/WindowManager.h
    src/ui/WindowManager.cpp

    # Network
    src/network/Tcp.h
    src/network/Tcp.cpp

    src/network/Protocol.h
    src/network/Protocol.cpp

    # Controller
    src/controller/ClientController.h
    src/controller/ClientController.cpp

    # Model
    src/model/User.h
    src/model/User.cpp

    src/model/ChatMessage.h
    src/model/ChatMessage.cpp

   # Common
    ../common/protocol.h
    ../common/message_schema.h
    src/network/ConfigManager.h src/network/ConfigManager.cpp
)

set_target_properties(Shen_chatroom PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# 链接Qt
target_link_libraries(
    Shen_chatroom
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

# 头文件搜索路径
target_include_directories(
    Shen_chatroom
    PRIVATE
    src
    ../common
)