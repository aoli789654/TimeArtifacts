/**
 * WebSocketServer.cpp
 * 
 * WebSocket服务器实现
 */

#include "WebSocketServer.h"
#include "SimpleWebSocketServer.h"
#include "APIHandler.h"
#include "../../include/core/GameEngine.h"
#include <iostream>

WebSocketServer::WebSocketServer(GameEngine* engine, uint16_t serverPort) 
    : gameEngine(engine), port(serverPort), running(false) {
    std::cout << "[WebSocket] 正在创建WebSocket服务器，端口: " << port << std::endl;
    
    // 创建SimpleWebSocketServer实例
    server = std::make_unique<SimpleWebSocketServer>();
    
    // 创建API处理器
    apiHandler = std::make_unique<APIHandler>();
    
    // 设置消息处理器
    setupMessageHandlers();
}

WebSocketServer::~WebSocketServer() {
    std::cout << "[WebSocket] 正在销毁WebSocket服务器" << std::endl;
    stop();
}

bool WebSocketServer::start() {
    if (running) {
        std::cout << "[WebSocket] 服务器已经在运行" << std::endl;
        return true;
    }
    
    std::cout << "[WebSocket] 正在启动WebSocket服务器，端口: " << port << std::endl;
    
    try {
        if (server->start(port)) {
            running = true;
            std::cout << "[WebSocket] WebSocket服务器启动成功，端口: " << port << std::endl;
            return true;
        } else {
            std::cerr << "[WebSocket] WebSocket服务器启动失败" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "[WebSocket] WebSocket服务器启动异常: " << e.what() << std::endl;
        return false;
    }
}

void WebSocketServer::stop() {
    if (!running) {
        return;
    }
    
    std::cout << "[WebSocket] 正在停止WebSocket服务器" << std::endl;
    
    try {
        if (server) {
            server->stop();
        }
        running = false;
        std::cout << "[WebSocket] WebSocket服务器已停止" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[WebSocket] 停止WebSocket服务器时发生异常: " << e.what() << std::endl;
    }
}

void WebSocketServer::handleMessage(const std::string& message) {
    if (!apiHandler) {
        std::cerr << "[WebSocket] API处理器未初始化" << std::endl;
        return;
    }
    
    try {
        // 使用API处理器处理消息
        std::string response = apiHandler->handleMessage(message);
        
        // 广播响应给所有客户端
        broadcastMessage(response);
        
    } catch (const std::exception& e) {
        std::cerr << "[WebSocket] 处理消息时发生异常: " << e.what() << std::endl;
        
        // 发送错误响应
        std::string errorResponse = R"({"type":"error","message":"服务器内部错误"})";
        broadcastMessage(errorResponse);
    }
}

void WebSocketServer::broadcastMessage(const std::string& message) {
    if (!server) {
        std::cerr << "[WebSocket] 服务器未初始化，无法广播消息" << std::endl;
        return;
    }
    
    try {
        server->broadcastMessage(message);
    } catch (const std::exception& e) {
        std::cerr << "[WebSocket] 广播消息时发生异常: " << e.what() << std::endl;
    }
}

void WebSocketServer::setupMessageHandlers() {
    if (!server) {
        std::cerr << "[WebSocket] 服务器未初始化，无法设置消息处理器" << std::endl;
        return;
    }
    
    // 设置消息接收回调
    server->setMessageHandler([this](const std::string& message, int clientId) {
        this->handleMessage(message);
    });
    
    std::cout << "[WebSocket] 消息处理器设置完成" << std::endl;
}