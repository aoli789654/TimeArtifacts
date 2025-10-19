/**
 * WebSocketServer.h
 * 
 * WebSocket服务器 - 负责前后端通信
 */

#pragma once

#include <string>
<<<<<<< Updated upstream
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
=======
#include <memory>
#include <functional>

// 前向声明
class APIHandler;
class SimpleWebSocketServer;
class GameEngine;
>>>>>>> Stashed changes

/**
 * WebSocket服务器类
 * 封装SimpleWebSocketServer，提供游戏相关的消息处理
 */
class WebSocketServer {
private:
<<<<<<< Updated upstream
    // 私有成员变量（只有这个类内部能访问）
    std::thread serverThread;           // 服务器运行的线程
    bool isRunning;                     // 服务器是否正在运行
    uint16_t port;                      // 服务器端口
    
    // 回调函数 - 当收到消息时要调用的函数
    std::function<void(const std::string&)> messageHandler;
=======
    std::unique_ptr<SimpleWebSocketServer> server;
    std::unique_ptr<APIHandler> apiHandler;
    GameEngine* gameEngine;
    bool isRunning;
>>>>>>> Stashed changes

public:
    WebSocketServer();
    ~WebSocketServer();
    
    /**
     * 启动WebSocket服务器
     * @param port 端口号（默认8080）
     * @return 成功返回true，失败返回false
     */
    bool start(uint16_t port = 8080);
    
    /**
     * 停止WebSocket服务器
     */
    void stop();
    
    /**
     * 设置游戏引擎引用
     * @param engine 游戏引擎指针
     */
    void setGameEngine(GameEngine* engine);
    
    /**
     * 向所有连接的客户端发送消息
     * @param message 要发送的消息（JSON字符串）
     */
    void sendToAll(const std::string& message);
    
    /**
     * 向指定客户端发送消息
     * @param clientId 客户端ID
     * @param message 要发送的消息
     */
    void sendToClient(int clientId, const std::string& message);
    
    /**
     * 检查服务器是否正在运行
     * @return 正在运行返回true，否则返回false
     */
    bool getIsRunning() const { return isRunning; }

private:
    /**
     * 处理来自客户端的消息
     * @param message 消息内容
     * @param clientId 客户端ID
     */
    void handleMessage(const std::string& message, int clientId);
};
