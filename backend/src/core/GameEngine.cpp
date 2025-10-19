<<<<<<< Updated upstream
/**
 * GameEngine.cpp
 * 
 * 游戏引擎实现
 */

#include "core/GameEngine.h"
#include "WebSocketServer.h"  // 引入我们新创建的WebSocket服务器
=======
#include "../../include/core/GameEngine.h"
#include "../../include/core/DataLoader.h"
#include "../../include/core/GameState.h"
#include "WebSocketServer.h"
#include "HttpServer.h"
>>>>>>> Stashed changes
#include <iostream>
#include <thread>
#include <chrono>

<<<<<<< Updated upstream
GameEngine::GameEngine() {
    std::cout << "[GameEngine] Creating game engine instance" << std::endl;
}

GameEngine::~GameEngine() {
    if (running.load()) {
        shutdown();
    }
    std::cout << "[GameEngine] Game engine instance destroyed" << std::endl;
=======
GameEngine::GameEngine() 
    : initialized(false), running(false), targetFrameTime(1.0f/60.0f) {
    std::cout << "[GameEngine] 游戏引擎已创建" << std::endl;
}

GameEngine::~GameEngine() {
    shutdown();
    std::cout << "[GameEngine] 游戏引擎已销毁" << std::endl;
>>>>>>> Stashed changes
}

bool GameEngine::initialize() {
    if (initialized.load()) {
<<<<<<< Updated upstream
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
=======
        std::cout << "[GameEngine] 引擎已经初始化过了" << std::endl;
        return true;
    }
    
    std::cout << "[GameEngine] 正在初始化游戏引擎..." << std::endl;
    
    if (!initializeSubsystems()) {
        std::cout << "[GameEngine] 子系统初始化失败" << std::endl;
>>>>>>> Stashed changes
        return false;
    }
    
    initialized.store(true);
    std::cout << "[GameEngine] 游戏引擎初始化完成" << std::endl;
    return true;
}

void GameEngine::run() {
    if (!initialized.load()) {
<<<<<<< Updated upstream
        std::cerr << "[GameEngine] ERROR: Game engine not initialized" << std::endl;
=======
        std::cout << "[GameEngine] 引擎未初始化，无法运行" << std::endl;
>>>>>>> Stashed changes
        return;
    }
    
    running.store(true);
<<<<<<< Updated upstream
    std::cout << "[GameEngine] Starting main game loop" << std::endl;
    
    // 游戏主循环
    const auto targetFrameTime = std::chrono::milliseconds(16); // ~60 FPS
=======
    std::cout << "[GameEngine] 游戏主循环开始" << std::endl;
    
    auto lastTime = std::chrono::steady_clock::now();
>>>>>>> Stashed changes
    
    while (running.load()) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
<<<<<<< Updated upstream
        try {
            // 执行一次游戏更新
            update();
            
        } catch (const std::exception& e) {
            std::cerr << "[GameEngine] Exception in game loop: " << e.what() << std::endl;
            // 继续运行，不因单次异常而崩溃
        }
=======
        update(deltaTime);
>>>>>>> Stashed changes
        
        // 帧率控制
        auto frameEnd = std::chrono::steady_clock::now();
<<<<<<< Updated upstream
        auto frameDuration = frameEnd - frameStart;
        
        if (frameDuration < targetFrameTime) {
            std::this_thread::sleep_for(targetFrameTime - frameDuration);
        }
    }
    
    std::cout << "[GameEngine] Main game loop exited" << std::endl;
}

void GameEngine::requestShutdown() {
    std::cout << "[GameEngine] Shutdown request received" << std::endl;
=======
        auto frameDuration = std::chrono::duration<float>(frameEnd - currentTime).count();
        if (frameDuration < targetFrameTime) {
            std::this_thread::sleep_for(
                std::chrono::duration<float>(targetFrameTime - frameDuration)
            );
        }
    }
    
    std::cout << "[GameEngine] 游戏主循环结束" << std::endl;
}

void GameEngine::requestShutdown() {
    std::cout << "[GameEngine] 收到关闭请求" << std::endl;
>>>>>>> Stashed changes
    running.store(false);
}

void GameEngine::shutdown() {
<<<<<<< Updated upstream
    std::cout << "[GameEngine] Starting game engine shutdown..." << std::endl;
=======
    if (!initialized.load()) {
        return;
    }
>>>>>>> Stashed changes
    
    std::cout << "[GameEngine] 正在关闭游戏引擎..." << std::endl;
    running.store(false);
    cleanupSubsystems();
    initialized.store(false);
<<<<<<< Updated upstream
    std::cout << "[GameEngine] Game engine shutdown complete" << std::endl;
=======
    std::cout << "[GameEngine] 游戏引擎已关闭" << std::endl;
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
        std::cout << "[GameEngine] 目标帧率设置为: " << fps << " FPS" << std::endl;
    }
}

float GameEngine::getCurrentFPS() const {
    return 1.0f / targetFrameTime;
>>>>>>> Stashed changes
}

bool GameEngine::initializeSubsystems() {
    std::cout << "[GameEngine] Initializing subsystems..." << std::endl;
    
    try {
<<<<<<< Updated upstream
        // 1. 创建WebSocket服务器
        std::cout << "[GameEngine] Creating WebSocket server..." << std::endl;
        webSocketServer = std::make_unique<WebSocketServer>();
        
        // 2. 启动WebSocket服务器
        if (!webSocketServer->start(8080)) {
            std::cerr << "[GameEngine] WebSocket server startup failed" << std::endl;
            return false;
        }
        
        std::cout << "[GameEngine] WebSocket server started successfully, listening on port 8080" << std::endl;
=======
        // 1. 初始化数据加载器
        std::cout << "[GameEngine] 正在创建数据加载器..." << std::endl;
        dataLoader = std::make_unique<DataLoader>();
        
        if (!dataLoader->initialize()) {
            std::cerr << "[GameEngine] 数据加载器初始化失败" << std::endl;
            return false;
        }
        
        if (!dataLoader->loadAllData()) {
            std::cerr << "[GameEngine] 游戏数据加载失败" << std::endl;
            return false;
        }
        std::cout << "[GameEngine] 数据加载器初始化成功" << std::endl;
        
        // 2. 初始化HTTP服务器
        std::cout << "[GameEngine] 正在创建HTTP服务器..." << std::endl;
        httpServer = std::make_unique<HttpServer>();
        httpServer->setGameEngine(this);
>>>>>>> Stashed changes
        
        if (!httpServer->start(8081)) {
            std::cerr << "[GameEngine] HTTP服务器启动失败" << std::endl;
            return false;
        }
        std::cout << "[GameEngine] HTTP服务器启动成功，正在监听端口 8081" << std::endl;
        
        // 3. 初始化游戏状态
        std::cout << "[GameEngine] 正在创建游戏状态..." << std::endl;
        currentGameState = std::make_unique<ExplorationState>(this);
        
        if (currentGameState) {
            currentGameState->enter();
            std::cout << "[GameEngine] 游戏状态初始化成功" << std::endl;
        } else {
            std::cerr << "[GameEngine] 游戏状态创建失败" << std::endl;
            return false;
        }
        
        std::cout << "[GameEngine] All subsystems initialized successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[GameEngine] Exception during subsystem initialization: " << e.what() << std::endl;
        return false;
    }
}

<<<<<<< Updated upstream
void GameEngine::update() {
    // TODO: 后端架构师在这里实现游戏逻辑更新
    // 示例：
    // 1. 更新状态管理器
    // 2. 处理网络消息
    // 3. 更新游戏逻辑
    // 4. 发送状态给客户端
    
    // 临时：避免CPU占用过高
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
=======
void GameEngine::update(float deltaTime) {
    // 更新当前游戏状态
    if (currentGameState) {
        currentGameState->update(deltaTime);
    }
    
    // 简单的更新逻辑
    static int updateCount = 0;
    updateCount++;
    
    if (updateCount % 300 == 0) { // 每5秒输出一次（假设60FPS）
        std::cout << "[GameEngine] 游戏运行中... (更新次数: " << updateCount << ")" << std::endl;
        
        // 输出当前游戏状态
        if (currentGameState) {
            std::cout << "[GameEngine] 当前游戏状态: " << currentGameState->getName() << std::endl;
        }
    }
>>>>>>> Stashed changes
}

void GameEngine::cleanupSubsystems() {
    std::cout << "[GameEngine] Cleaning up subsystems..." << std::endl;
    
    if (currentGameState) {
        currentGameState->exit();
        currentGameState.reset();
        std::cout << "[GameEngine] 游戏状态已清理" << std::endl;
    }
    
    if (webSocketServer) {
<<<<<<< Updated upstream
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
=======
        webSocketServer->stop();
        webSocketServer.reset();
        std::cout << "[GameEngine] WebSocket服务器已关闭" << std::endl;
    }
    
    if (dataLoader) {
        dataLoader.reset();
        std::cout << "[GameEngine] 数据加载器已清理" << std::endl;
    }
    
    std::cout << "[GameEngine] 所有子系统已清理" << std::endl;
}
>>>>>>> Stashed changes
