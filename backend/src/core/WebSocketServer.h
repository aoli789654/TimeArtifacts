/**
 * WebSocketServer.h
 * 
 * WebSocket服务器 - 负责前后端通信
 */

#pragma once

#include <string>
#include <memory>
#include <functional>

// 前向声明
class APIHandler;
class SimpleWebSocketServer;
class GameEngine;

/**
 * WebSocket服务器类
 * 封装SimpleWebSocketServer，提供游戏相关的消息处理
 */
class WebSocketServer {
private:
    std::unique_ptr<SimpleWebSocketServer> server;
    std::unique_ptr<APIHandler> apiHandler;
    GameEngine* gameEngine;
    uint16_t port;
    bool running;

public:
    WebSocketServer(GameEngine* engine, uint16_t serverPort = 8080);
    ~WebSocketServer();
    
    // 禁用拷贝
    WebSocketServer(const WebSocketServer&) = delete;
    WebSocketServer& operator=(const WebSocketServer&) = delete;
    
    bool start();
    void stop();
    bool isRunning() const { return running; }
    uint16_t getPort() const { return port; }
    
    // 消息处理
    void handleMessage(const std::string& message);
    void broadcastMessage(const std::string& message);
    
private:
    void setupMessageHandlers();
};