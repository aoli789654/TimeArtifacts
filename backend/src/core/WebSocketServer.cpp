/**
 * WebSocketServer.cpp
 * 
 * WebSocket服务器实现（简化版本）
 */

#include "WebSocketServer.h"
#include "APIHandler.h"
#include <iostream>
#include <chrono>

// 如果没有nlohmann/json，使用简单的字符串拼接
#ifndef NLOHMANN_JSON_VERSION_MAJOR
#define USE_SIMPLE_JSON 1
#endif

// 构造函数 - 创建WebSocket服务器对象时调用
WebSocketServer::WebSocketServer() : isRunning(false), port(8080) {
    std::cout << "[WebSocket] 正在创建WebSocket服务器（简化版本）" << std::endl;
    
    // 创建API处理器
    apiHandler = std::make_unique<APIHandler>();
    std::cout << "[WebSocket] API处理器已创建" << std::endl;
}

// 析构函数 - 销毁对象时调用
WebSocketServer::~WebSocketServer() {
    if (isRunning) {
        stop();
    }
    std::cout << "[WebSocket] WebSocket服务器已销毁" << std::endl;
}

// 启动服务器
bool WebSocketServer::start(uint16_t serverPort) {
    if (isRunning) {
        std::cout << "[WebSocket] 警告: 服务器已经在运行" << std::endl;
        return true;
    }
    
    this->port = serverPort;
    
    try {
        std::cout << "[WebSocket] 正在启动WebSocket服务器（模拟模式），端口: " << port << std::endl;
        
        // 在单独的线程中运行服务器模拟（不阻塞主程序）
        serverThread = std::thread([this]() {
            this->simulateServerLoop();
        });
        
        isRunning = true;
        std::cout << "[WebSocket] WebSocket服务器启动成功（模拟模式）！" << std::endl;
        std::cout << "[WebSocket] 注意: 这是用于学习和测试的简化版本" << std::endl;
        std::cout << "[WebSocket] 真正的WebSocket功能将在后续实现" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[WebSocket] 服务器启动失败: " << e.what() << std::endl;
        return false;
    }
}

// 停止服务器
void WebSocketServer::stop() {
    if (!isRunning) {
        return;
    }
    
    std::cout << "[WebSocket] 正在停止WebSocket服务器..." << std::endl;
    
    // 停止服务器
    isRunning = false;
    
    // 等待服务器线程结束
    if (serverThread.joinable()) {
        serverThread.join();
    }
    
    std::cout << "[WebSocket] WebSocket服务器已停止" << std::endl;
}

// 设置消息处理函数
void WebSocketServer::setMessageHandler(std::function<void(const std::string&)> handler) {
    messageHandler = handler;
    std::cout << "[WebSocket] 消息处理器已设置" << std::endl;
}

// 向所有客户端发送消息（模拟版本）
void WebSocketServer::sendToAll(const std::string& message) {
    if (!isRunning) {
        std::cout << "[WebSocket] 警告: 服务器未运行，无法发送消息" << std::endl;
        return;
    }
    
    std::cout << "[WebSocket] [模拟] 向客户端发送消息: " << message.substr(0, 100);
    if (message.length() > 100) {
        std::cout << "...";
    }
    std::cout << std::endl;
}

// 模拟服务器运行循环
void WebSocketServer::simulateServerLoop() {
    std::cout << "[WebSocket] 服务器模拟循环已启动，端口: " << port << std::endl;
    
    int connectionCount = 0;
    
    while (isRunning) {
        // 模拟每10秒有一个新的"连接"
        std::this_thread::sleep_for(std::chrono::seconds(10));
        
        if (!isRunning) break;
        
        connectionCount++;
        std::cout << "[WebSocket] [模拟] 新客户端连接 #" << connectionCount << std::endl;
        
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
        
        std::cout << "[WebSocket] [模拟] 向客户端 #" << connectionCount << " 发送欢迎消息" << std::endl;
        
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
            
            std::cout << "[WebSocket] [模拟] 收到消息: " << msg << std::endl;
            
            // 使用API处理器处理消息
            if (apiHandler) {
                std::string response = apiHandler->handleMessage(msg);
                std::cout << "[WebSocket] [模拟] 生成的响应: " << response.substr(0, 150);
                if (response.length() > 150) {
                    std::cout << "...";
                }
                std::cout << std::endl;
            }
            
            // 如果设置了消息处理器，也调用它
            if (messageHandler) {
                messageHandler(msg);
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
    
    std::cout << "[WebSocket] 服务器模拟循环已结束" << std::endl;
}