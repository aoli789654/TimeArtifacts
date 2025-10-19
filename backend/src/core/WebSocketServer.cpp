/**
 * WebSocketServer.cpp
 * 
 * WebSocket服务器实现
 */

#include "WebSocketServer.h"
<<<<<<< Updated upstream
=======
#include "SimpleWebSocketServer.h"
#include "APIHandler.h"
#include "../../include/core/GameEngine.h"
#include "../../include/core/GameState.h"
>>>>>>> Stashed changes
#include <iostream>

<<<<<<< Updated upstream
// 如果没有nlohmann/json，使用简单的字符串拼接
#ifndef NLOHMANN_JSON_VERSION_MAJOR
#define USE_SIMPLE_JSON 1
#endif

// 构造函数 - 创建WebSocket服务器对象时调用
WebSocketServer::WebSocketServer() : isRunning(false), port(8080) {
    std::cout << "[WebSocket] Creating WebSocket server (simplified version)" << std::endl;
=======
WebSocketServer::WebSocketServer() 
    : gameEngine(nullptr), isRunning(false) {
    std::cout << "[WebSocket] 正在创建WebSocket服务器" << std::endl;
    
    server = std::make_unique<SimpleWebSocketServer>();
    apiHandler = std::make_unique<APIHandler>();
    
    std::cout << "[WebSocket] WebSocket服务器已创建" << std::endl;
>>>>>>> Stashed changes
}

WebSocketServer::~WebSocketServer() {
    if (isRunning) {
        stop();
    }
    std::cout << "[WebSocket] WebSocket server destroyed" << std::endl;
}

bool WebSocketServer::start(uint16_t port) {
    if (isRunning) {
        std::cout << "[WebSocket] WARNING: Server already running" << std::endl;
        return true;
    }
    
    // 设置消息处理器
    server->setMessageHandler([this](const std::string& message, int clientId) {
        this->handleMessage(message, clientId);
    });
    
<<<<<<< Updated upstream
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
=======
    if (server->start(port)) {
        isRunning = true;
        std::cout << "[WebSocket] WebSocket服务器启动成功，端口: " << port << std::endl;
        return true;
    } else {
        std::cerr << "[WebSocket] WebSocket服务器启动失败" << std::endl;
>>>>>>> Stashed changes
        return false;
    }
}

void WebSocketServer::stop() {
    if (!isRunning) {
        return;
    }
    
    std::cout << "[WebSocket] Stopping WebSocket server..." << std::endl;
    
    server->stop();
    isRunning = false;
    
<<<<<<< Updated upstream
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
=======
    std::cout << "[WebSocket] WebSocket服务器已停止" << std::endl;
}

void WebSocketServer::setGameEngine(GameEngine* engine) {
    gameEngine = engine;
    std::cout << "[WebSocket] 游戏引擎引用已设置" << std::endl;
>>>>>>> Stashed changes
}

void WebSocketServer::sendToAll(const std::string& message) {
    if (!isRunning) {
        std::cout << "[WebSocket] WARNING: Server not running, cannot send message" << std::endl;
        return;
    }
    
<<<<<<< Updated upstream
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
=======
    server->broadcastMessage(message);
}

void WebSocketServer::sendToClient(int clientId, const std::string& message) {
    if (!isRunning) {
        std::cout << "[WebSocket] 警告: 服务器未运行，无法发送消息" << std::endl;
        return;
    }
    
    server->sendMessage(clientId, message);
}

void WebSocketServer::handleMessage(const std::string& message, int clientId) {
    std::cout << "[WebSocket] 收到客户端 " << clientId << " 消息: " << message << std::endl;
    
    try {
        // 使用API处理器处理消息
        if (apiHandler) {
            std::string response = apiHandler->handleMessage(message);
            sendToClient(clientId, response);
        }
        
        // 如果有游戏引擎，也可以直接处理游戏相关消息
        if (gameEngine) {
            GameState* currentState = gameEngine->getCurrentGameState();
            if (currentState) {
                // 可以在这里添加更复杂的消息路由逻辑
                // 比如根据消息类型决定是否传递给当前游戏状态
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[WebSocket] 处理消息时发生错误: " << e.what() << std::endl;
        
        // 发送错误响应
        std::string errorResponse = R"({"type":"error","message":"服务器处理消息时发生错误"})";
        sendToClient(clientId, errorResponse);
    }
>>>>>>> Stashed changes
}