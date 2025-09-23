/**
 * GameEngine.cpp
 * 
 * 游戏引擎实现
 */

#include "core/GameEngine.h"
#include "WebSocketServer.h"  // 引入我们新创建的WebSocket服务器
#include <iostream>
#include <thread>
#include <chrono>

GameEngine::GameEngine() {
    std::cout << "[GameEngine] Creating game engine instance" << std::endl;
}

GameEngine::~GameEngine() {
    if (running.load()) {
        shutdown();
    }
    std::cout << "[GameEngine] Game engine instance destroyed" << std::endl;
}

bool GameEngine::initialize() {
    if (initialized.load()) {
        std::cout << "[GameEngine] WARNING: Game engine already initialized" << std::endl;
        return true;
    }
    
    std::cout << "[GameEngine] Starting game engine initialization..." << std::endl;
    
    try {
        // 初始化各个子系统
        if (!initializeSubsystems()) {
            std::cerr << "[GameEngine] Subsystem initialization failed" << std::endl;
            return false;
        }
        
        initialized.store(true);
        std::cout << "[GameEngine] Game engine initialization completed" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[GameEngine] Exception during initialization: " << e.what() << std::endl;
        return false;
    }
}

void GameEngine::run() {
    if (!initialized.load()) {
        std::cerr << "[GameEngine] ERROR: Game engine not initialized" << std::endl;
        return;
    }
    
    running.store(true);
    std::cout << "[GameEngine] Starting main game loop" << std::endl;
    
    // 游戏主循环
    const auto targetFrameTime = std::chrono::milliseconds(16); // ~60 FPS
    
    while (running.load()) {
        auto frameStart = std::chrono::steady_clock::now();
        
        try {
            // 执行一次游戏更新
            update();
            
        } catch (const std::exception& e) {
            std::cerr << "[GameEngine] Exception in game loop: " << e.what() << std::endl;
            // 继续运行，不因单次异常而崩溃
        }
        
        // 控制帧率
        auto frameEnd = std::chrono::steady_clock::now();
        auto frameDuration = frameEnd - frameStart;
        
        if (frameDuration < targetFrameTime) {
            std::this_thread::sleep_for(targetFrameTime - frameDuration);
        }
    }
    
    std::cout << "[GameEngine] Main game loop exited" << std::endl;
}

void GameEngine::requestShutdown() {
    std::cout << "[GameEngine] Shutdown request received" << std::endl;
    running.store(false);
}

void GameEngine::shutdown() {
    std::cout << "[GameEngine] Starting game engine shutdown..." << std::endl;
    
    // 停止主循环
    running.store(false);
    
    // 清理所有子系统
    cleanupSubsystems();
    
    initialized.store(false);
    std::cout << "[GameEngine] Game engine shutdown complete" << std::endl;
}

bool GameEngine::initializeSubsystems() {
    std::cout << "[GameEngine] Initializing subsystems..." << std::endl;
    
    try {
        // 1. 创建WebSocket服务器
        std::cout << "[GameEngine] Creating WebSocket server..." << std::endl;
        webSocketServer = std::make_unique<WebSocketServer>();
        
        // 2. 启动WebSocket服务器
        if (!webSocketServer->start(8080)) {
            std::cerr << "[GameEngine] WebSocket server startup failed" << std::endl;
            return false;
        }
        
        std::cout << "[GameEngine] WebSocket server started successfully, listening on port 8080" << std::endl;
        
        // TODO: 后端架构师继续添加其他子系统
        // 3. 创建事件管理器
        // 4. 创建状态管理器  
        // 5. 创建数据加载器
        
        std::cout << "[GameEngine] All subsystems initialized successfully" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[GameEngine] Exception during subsystem initialization: " << e.what() << std::endl;
        return false;
    }
}

void GameEngine::update() {
    // TODO: 后端架构师在这里实现游戏逻辑更新
    // 示例：
    // 1. 更新状态管理器
    // 2. 处理网络消息
    // 3. 更新游戏逻辑
    // 4. 发送状态给客户端
    
    // 临时：避免CPU占用过高
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void GameEngine::cleanupSubsystems() {
    std::cout << "[GameEngine] Cleaning up subsystems..." << std::endl;
    
    // 按照初始化的逆序进行清理
    
    // 1. 停止WebSocket服务器
    if (webSocketServer) {
        std::cout << "[GameEngine] Stopping WebSocket server..." << std::endl;
        webSocketServer->stop();
        webSocketServer.reset();  // 释放内存
        std::cout << "[GameEngine] WebSocket server stopped" << std::endl;
    }
    
    // TODO: 后端架构师继续清理其他子系统
    // 2. 清理数据加载器
    // 3. 清理状态管理器
    // 4. 清理事件管理器
    
    std::cout << "[GameEngine] Subsystem cleanup complete" << std::endl;
}
