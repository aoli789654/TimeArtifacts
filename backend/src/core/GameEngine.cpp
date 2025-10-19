/**
 * GameEngine.cpp
 * 
 * 游戏引擎实现
 */

#include "../../include/core/GameEngine.h"
#include "../../include/core/DataLoader.h"
#include "../../include/core/GameState.h"
#include "WebSocketServer.h"
#include "HttpServer.h"
#include <iostream>
#include <thread>
#include <chrono>

GameEngine::GameEngine() 
    : initialized(false), running(false), targetFrameTime(1.0f / 60.0f) {
    std::cout << "[GameEngine] 正在创建游戏引擎实例" << std::endl;
}

GameEngine::~GameEngine() {
    if (running.load()) {
        shutdown();
    }
    std::cout << "[GameEngine] 游戏引擎实例已销毁" << std::endl;
}

bool GameEngine::initialize() {
    if (initialized.load()) {
        std::cout << "[GameEngine] 游戏引擎已经初始化" << std::endl;
        return true;
    }
    
    std::cout << "[GameEngine] 正在初始化游戏引擎..." << std::endl;
    
    try {
        if (!initializeSubsystems()) {
            std::cerr << "[GameEngine] 子系统初始化失败" << std::endl;
            return false;
        }
        
        initialized.store(true);
        std::cout << "[GameEngine] 游戏引擎初始化成功" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[GameEngine] 初始化过程中发生异常: " << e.what() << std::endl;
        return false;
    }
}

void GameEngine::run() {
    if (!initialized.load()) {
        std::cerr << "[GameEngine] 游戏引擎未初始化，无法运行" << std::endl;
        return;
    }
    
    if (running.load()) {
        std::cout << "[GameEngine] 游戏引擎已经在运行" << std::endl;
        return;
    }
    
    std::cout << "[GameEngine] 正在启动游戏主循环..." << std::endl;
    running.store(true);
    
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (running.load()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        try {
            update(deltaTime);
        } catch (const std::exception& e) {
            std::cerr << "[GameEngine] 更新过程中发生异常: " << e.what() << std::endl;
        }
        
        // 控制帧率
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    std::cout << "[GameEngine] 游戏主循环已退出" << std::endl;
}

void GameEngine::requestShutdown() {
    std::cout << "[GameEngine] 收到关闭请求" << std::endl;
    running.store(false);
}

void GameEngine::shutdown() {
    std::cout << "[GameEngine] 正在关闭游戏引擎..." << std::endl;
    
    running.store(false);
    
    try {
        cleanupSubsystems();
        initialized.store(false);
        std::cout << "[GameEngine] 游戏引擎已安全关闭" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[GameEngine] 关闭过程中发生异常: " << e.what() << std::endl;
    }
}

WebSocketServer* GameEngine::getWebSocketServer() const {
    return webSocketServer.get();
}

HttpServer* GameEngine::getHttpServer() const {
    return httpServer.get();
}

DataLoader* GameEngine::getDataLoader() const {
    return dataLoader.get();
}

GameState* GameEngine::getCurrentGameState() const {
    return currentGameState.get();
}

void GameEngine::setTargetFPS(int fps) {
    if (fps > 0) {
        targetFrameTime = 1.0f / fps;
        std::cout << "[GameEngine] 目标FPS设置为: " << fps << std::endl;
    }
}

float GameEngine::getCurrentFPS() const {
    return 1.0f / targetFrameTime;
}

bool GameEngine::initializeSubsystems() {
    std::cout << "[GameEngine] 正在初始化子系统..." << std::endl;
    
    try {
        // 初始化数据加载器
        dataLoader = std::make_unique<DataLoader>();
        if (!dataLoader) {
            std::cerr << "[GameEngine] 数据加载器创建失败" << std::endl;
            return false;
        }
        std::cout << "[GameEngine] 数据加载器初始化成功" << std::endl;
        
        // 初始化游戏状态 - 暂时设为nullptr，由具体的游戏逻辑管理
        currentGameState = nullptr;
        std::cout << "[GameEngine] 游戏状态初始化成功" << std::endl;
        
        // 初始化HTTP服务器
        httpServer = std::make_unique<HttpServer>(this);
        if (!httpServer || !httpServer->start(8081)) {
            std::cerr << "[GameEngine] HTTP服务器启动失败" << std::endl;
            return false;
        }
        std::cout << "[GameEngine] HTTP服务器初始化成功" << std::endl;
        
        // 初始化WebSocket服务器
        webSocketServer = std::make_unique<WebSocketServer>(this, 8080);
        if (!webSocketServer || !webSocketServer->start()) {
            std::cerr << "[GameEngine] WebSocket服务器启动失败" << std::endl;
            return false;
        }
        std::cout << "[GameEngine] WebSocket服务器初始化成功" << std::endl;
        
        std::cout << "[GameEngine] 所有子系统初始化完成" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[GameEngine] 子系统初始化异常: " << e.what() << std::endl;
        return false;
    }
}

void GameEngine::update(float deltaTime) {
    // 这里可以添加游戏逻辑更新
    // 目前主要由HTTP和WebSocket服务器处理请求
}

void GameEngine::cleanupSubsystems() {
    std::cout << "[GameEngine] 正在清理子系统..." << std::endl;
    
    try {
        if (webSocketServer) {
            webSocketServer->stop();
            webSocketServer.reset();
            std::cout << "[GameEngine] WebSocket服务器已清理" << std::endl;
        }
        
        if (httpServer) {
            httpServer->stop();
            httpServer.reset();
            std::cout << "[GameEngine] HTTP服务器已清理" << std::endl;
        }
        
        if (currentGameState) {
            currentGameState.reset();
            std::cout << "[GameEngine] 游戏状态已清理" << std::endl;
        }
        
        if (dataLoader) {
            dataLoader.reset();
            std::cout << "[GameEngine] 数据加载器已清理" << std::endl;
        }
        
        std::cout << "[GameEngine] 所有子系统清理完成" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[GameEngine] 清理子系统时发生异常: " << e.what() << std::endl;
    }
}