/**
 * WebSocketServer.cpp
 * 
 * WebSocket服务器实现（简化版本）
 */

#include "WebSocketServer.h"
#include <iostream>
#include <chrono>

// 如果没有nlohmann/json，使用简单的字符串拼接
#ifndef NLOHMANN_JSON_VERSION_MAJOR
#define USE_SIMPLE_JSON 1
#endif

// 构造函数 - 创建WebSocket服务器对象时调用
WebSocketServer::WebSocketServer() : isRunning(false), port(8080) {
    std::cout << "[WebSocket] Creating WebSocket server (simplified version)" << std::endl;
}

// 析构函数 - 销毁对象时调用
WebSocketServer::~WebSocketServer() {
    if (isRunning) {
        stop();
    }
    std::cout << "[WebSocket] WebSocket server destroyed" << std::endl;
}

// 启动服务器
bool WebSocketServer::start(uint16_t serverPort) {
    if (isRunning) {
        std::cout << "[WebSocket] WARNING: Server already running" << std::endl;
        return true;
    }
    
    this->port = serverPort;
    
    try {
        std::cout << "[WebSocket] Starting WebSocket server (simulation mode), port: " << port << std::endl;
        
        // 在单独的线程中运行服务器模拟（不阻塞主程序）
        serverThread = std::thread([this]() {
            this->simulateServerLoop();
        });
        
        isRunning = true;
        std::cout << "[WebSocket] WebSocket server started successfully (simulation mode)!" << std::endl;
        std::cout << "[WebSocket] NOTE: This is a simplified version for learning and testing" << std::endl;
        std::cout << "[WebSocket] Real WebSocket functionality will be implemented later" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[WebSocket] Server startup failed: " << e.what() << std::endl;
        return false;
    }
}

// 停止服务器
void WebSocketServer::stop() {
    if (!isRunning) {
        return;
    }
    
    std::cout << "[WebSocket] Stopping WebSocket server..." << std::endl;
    
    // 停止服务器
    isRunning = false;
    
    // 等待服务器线程结束
    if (serverThread.joinable()) {
        serverThread.join();
    }
    
    std::cout << "[WebSocket] WebSocket server stopped" << std::endl;
}

// 设置消息处理函数
void WebSocketServer::setMessageHandler(std::function<void(const std::string&)> handler) {
    messageHandler = handler;
    std::cout << "[WebSocket] Message handler set" << std::endl;
}

// 向所有客户端发送消息（模拟版本）
void WebSocketServer::sendToAll(const std::string& message) {
    if (!isRunning) {
        std::cout << "[WebSocket] WARNING: Server not running, cannot send message" << std::endl;
        return;
    }
    
    std::cout << "[WebSocket] [SIMULATION] Sending message to clients: " << message.substr(0, 100);
    if (message.length() > 100) {
        std::cout << "...";
    }
    std::cout << std::endl;
}

// 模拟服务器运行循环
void WebSocketServer::simulateServerLoop() {
    std::cout << "[WebSocket] Server simulation loop started, port: " << port << std::endl;
    
    int connectionCount = 0;
    
    while (isRunning) {
        // 模拟每10秒有一个新的"连接"
        std::this_thread::sleep_for(std::chrono::seconds(10));
        
        if (!isRunning) break;
        
        connectionCount++;
        std::cout << "[WebSocket] [SIMULATION] New client connection #" << connectionCount << std::endl;
        
        // 模拟发送欢迎消息
#ifdef USE_SIMPLE_JSON
        std::string welcome = R"({
            "type": "welcome",
            "message": "欢迎来到时光信物游戏世界！",
            "data": {
                "currentLocation": "bookstore",
                "description": "你站在时光角落书店门前，温暖的灯光从窗户中透出...",
                "playerAttributes": {
                    "observation": 1,
                    "communication": 1,
                    "action": 1,
                    "empathy": 1
                },
                "availableActions": [
                    "enter_bookstore",
                    "look_around",
                    "examine_sign"
                ]
            }
        })";
#else
        nlohmann::json welcome = {
            {"type", "welcome"},
            {"message", "欢迎来到时光信物游戏世界！"},
            {"data", {
                {"currentLocation", "bookstore"},
                {"description", "你站在时光角落书店门前，温暖的灯光从窗户中透出..."},
                {"playerAttributes", {
                    {"observation", 1},
                    {"communication", 1}, 
                    {"action", 1},
                    {"empathy", 1}
                }},
                {"availableActions", nlohmann::json::array({
                    "enter_bookstore",
                    "look_around",
                    "examine_sign"
                })}
            }}
        };
#endif
        
        std::cout << "[WebSocket] [SIMULATION] Sending welcome message to client #" << connectionCount << std::endl;
        
        // 模拟收到一些消息
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        if (!isRunning) break;
        
        // 模拟处理客户端消息
        std::vector<std::string> simulatedMessages = {
            R"({"action": "move", "data": {"direction": "north"}})",
            R"({"action": "examine", "data": {"target": "bookshelf"}})",
            R"({"action": "talk", "data": {"target": "owner"}})"
        };
        
        for (const auto& msg : simulatedMessages) {
            if (!isRunning) break;
            
            std::cout << "[WebSocket] [SIMULATION] Received message: " << msg << std::endl;
            
            // 如果设置了消息处理器，调用它
            if (messageHandler) {
                messageHandler(msg);
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
    
    std::cout << "[WebSocket] Server simulation loop ended" << std::endl;
}