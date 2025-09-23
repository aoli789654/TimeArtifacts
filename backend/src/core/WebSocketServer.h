/**
 * WebSocketServer.h
 * 
 * WebSocket服务器 - 负责前后端通信（简化版本）
 * 
 * 这个类就像一个"电话总机"，处理前端和后端的实时通信
 * 注意：这是一个简化版本，用于学习和测试
 */

#pragma once  // 防止头文件被重复包含

// 引入需要的库
#include <string>
#include <thread>
#include <functional>
#include <iostream>

// 尝试包含nlohmann/json，如果失败则使用字符串处理
#ifdef __has_include
#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
#endif
#else
// 如果编译器不支持__has_include，尝试直接包含
#ifdef NLOHMANN_JSON_HPP
#include <nlohmann/json.hpp>
#endif
#endif

/**
 * WebSocket服务器类（简化版本）
 * 
 * 功能：
 * 1. 模拟启动服务器
 * 2. 输出调试信息
 * 3. 为将来的真实实现做准备
 */
class WebSocketServer {
private:
    // 私有成员变量（只有这个类内部能访问）
    std::thread serverThread;           // 服务器运行的线程
    bool isRunning;                     // 服务器是否正在运行
    uint16_t port;                      // 服务器端口
    
    // 回调函数 - 当收到消息时要调用的函数
    std::function<void(const std::string&)> messageHandler;

public:
    // 构造函数 - 创建对象时自动调用
    WebSocketServer();
    
    // 析构函数 - 销毁对象时自动调用
    ~WebSocketServer();
    
    /**
     * 启动WebSocket服务器（模拟版本）
     * @param port 端口号（默认8080）
     * @return 成功返回true，失败返回false
     */
    bool start(uint16_t port = 8080);
    
    /**
     * 停止WebSocket服务器
     */
    void stop();
    
    /**
     * 设置消息处理函数
     * 当收到前端消息时，会调用这个函数
     * @param handler 处理消息的函数
     */
    void setMessageHandler(std::function<void(const std::string&)> handler);
    
    /**
     * 向所有连接的客户端发送消息（模拟版本）
     * @param message 要发送的消息（JSON字符串）
     */
    void sendToAll(const std::string& message);
    
    /**
     * 检查服务器是否正在运行
     * @return 正在运行返回true，否则返回false
     */
    bool getIsRunning() const { return isRunning; }

private:
    // 私有方法（只有类内部能调用）
    
    /**
     * 模拟服务器运行循环
     */
    void simulateServerLoop();
};
