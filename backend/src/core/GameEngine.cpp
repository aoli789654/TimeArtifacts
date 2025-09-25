/**
 * GameEngine.cpp
 * 
 * 游戏引擎实现
 */

#include "core/GameEngine.h"
#include "StateManager.h"     // 状态管理器
#include "EventManager.h"     // 事件管理器
#include "Events.h"           // 事件类定义
#include "WebSocketServer.h"  // WebSocket服务器
#include <iostream>
#include <thread>
#include <chrono>

GameEngine::GameEngine() 
    : stateManager(nullptr)
    , eventManager(nullptr)
    , webSocketServer(nullptr)
    , initialized(false)
    , running(false)
    , targetFrameTime(1.0f / 60.0f) // 默认60FPS
    , frameCount(0) {
    
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
        std::cout << "[GameEngine] 警告: 游戏引擎已经初始化" << std::endl;
        return true;
    }
    
    std::cout << "[GameEngine] 正在启动游戏引擎初始化..." << std::endl;
    
    try {
        // 初始化各个子系统
        if (!initializeSubsystems()) {
            std::cerr << "[GameEngine] 子系统初始化失败" << std::endl;
            return false;
        }
        
        initialized.store(true);
        std::cout << "[GameEngine] 游戏引擎初始化完成" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[GameEngine] 初始化过程中发生异常: " << e.what() << std::endl;
        return false;
    }
}

void GameEngine::run() {
    if (!initialized.load()) {
        std::cerr << "[GameEngine] 错误: 游戏引擎未初始化" << std::endl;
        return;
    }
    
    running.store(true);
    std::cout << "[GameEngine] 正在启动主游戏循环" << std::endl;
    
    // 游戏主循环
    const auto frameInterval = std::chrono::milliseconds(16); // ~60 FPS
    
    while (running.load()) {
        auto frameStart = std::chrono::steady_clock::now();
        
        try {
            // 计算delta时间
            static auto lastFrameTime = frameStart;
            auto deltaTime = std::chrono::duration<float>(frameStart - lastFrameTime).count();
            lastFrameTime = frameStart;
            
            // 执行一次游戏更新
            update(deltaTime);
            
        } catch (const std::exception& e) {
            std::cerr << "[GameEngine] 游戏循环中发生异常: " << e.what() << std::endl;
            // 继续运行，不因单次异常而崩溃
        }
        
        // 控制帧率
        auto frameEnd = std::chrono::steady_clock::now();
        auto frameDuration = frameEnd - frameStart;
        
        if (frameDuration < frameInterval) {
            std::this_thread::sleep_for(frameInterval - frameDuration);
        }
    }
    
    std::cout << "[GameEngine] 主游戏循环已退出" << std::endl;
}

void GameEngine::requestShutdown() {
    std::cout << "[GameEngine] 已收到关闭请求" << std::endl;
    running.store(false);
}

void GameEngine::shutdown() {
    std::cout << "[GameEngine] 正在启动游戏引擎关闭..." << std::endl;
    
    // 停止主循环
    running.store(false);
    
    // 清理所有子系统
    cleanupSubsystems();
    
    initialized.store(false);
    std::cout << "[GameEngine] 游戏引擎关闭完成" << std::endl;
}

bool GameEngine::initializeSubsystems() {
    std::cout << "[GameEngine] 正在初始化子系统..." << std::endl;
    
    try {
        // 1. 创建事件管理器（首先创建，其他系统需要依赖它）
        std::cout << "[GameEngine] 正在创建事件管理器..." << std::endl;
        eventManager = std::make_unique<EventManager>();
        
        // 2. 创建状态管理器
        std::cout << "[GameEngine] 正在创建状态管理器..." << std::endl;
        stateManager = std::make_unique<StateManager>();
        
        // 3. 创建WebSocket服务器
        std::cout << "[GameEngine] 正在创建WebSocket服务器..." << std::endl;
        webSocketServer = std::make_unique<WebSocketServer>();
        
        // 4. 启动WebSocket服务器
        if (!webSocketServer->start(8080)) {
            std::cerr << "[GameEngine] WebSocket服务器启动失败" << std::endl;
            return false;
        }
        
        std::cout << "[GameEngine] WebSocket服务器启动成功，正在监听端口 8080" << std::endl;
        
        // 5. 设置系统间的连接
        setupEventListeners();
        
        // TODO: 后端架构师继续添加其他子系统
        // 3. 创建事件管理器
        // 4. 创建状态管理器  
        // 5. 创建数据加载器
        
        std::cout << "[GameEngine] 所有子系统初始化成功" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[GameEngine] 子系统初始化过程中发生异常: " << e.what() << std::endl;
        return false;
    }
}

void GameEngine::update(float deltaTime) {
    frameCount++;
    
    try {
        // 1. 处理事件队列
        if (eventManager) {
            eventManager->processEvents(50); // 每帧最多处理50个事件
        }
        
        // 2. 更新状态管理器
        if (stateManager) {
            stateManager->update(deltaTime);
        }
        
        // 3. 处理系统级事件
        handleSystemEvents();
        
        // 4. 渲染当前状态
        if (stateManager) {
            stateManager->render();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[GameEngine] 更新过程中发生异常: " << e.what() << std::endl;
        
        // 发布错误事件
        if (eventManager) {
            auto errorEvent = std::make_unique<ErrorEvent>("UPDATE_ERROR", e.what(), "GameEngine");
            eventManager->publishImmediate(std::move(errorEvent));
        }
    }
}

void GameEngine::cleanupSubsystems() {
    std::cout << "[GameEngine] 正在清理子系统..." << std::endl;
    
    // 按照初始化的逆序进行清理
    
    // 1. 停止WebSocket服务器
    if (webSocketServer) {
        std::cout << "[GameEngine] 正在停止WebSocket服务器..." << std::endl;
        webSocketServer->stop();
        webSocketServer.reset();  // 释放内存
        std::cout << "[GameEngine] WebSocket服务器已停止" << std::endl;
    }
    
    // 2. 清理状态管理器
    if (stateManager) {
        std::cout << "[GameEngine] 正在清理状态管理器..." << std::endl;
        stateManager.reset();
        std::cout << "[GameEngine] 状态管理器已清理" << std::endl;
    }
    
    // 3. 清理事件管理器（最后清理，因为其他系统可能还需要发布事件）
    if (eventManager) {
        std::cout << "[GameEngine] 正在清理事件管理器..." << std::endl;
        eventManager.reset();
        std::cout << "[GameEngine] 事件管理器已清理" << std::endl;
    }
    
    // TODO: 后续添加其他子系统的清理
    // 4. 清理数据加载器
    // 5. 清理音频系统
    
    std::cout << "[GameEngine] 子系统清理完成" << std::endl;
}

// =================================================================
// 访问器方法
// =================================================================

StateManager* GameEngine::getStateManager() const {
    return stateManager.get();
}

EventManager* GameEngine::getEventManager() const {
    return eventManager.get();
}

WebSocketServer* GameEngine::getWebSocketServer() const {
    return webSocketServer.get();
}

void GameEngine::setTargetFPS(int fps) {
    if (fps > 0) {
        targetFrameTime = 1.0f / fps;
        std::cout << "[GameEngine] 设置目标帧率: " << fps << " FPS" << std::endl;
    }
}

float GameEngine::getCurrentFPS() const {
    if (targetFrameTime > 0) {
        return 1.0f / targetFrameTime;
    }
    return 0.0f;
}

// =================================================================
// 事件系统方法
// =================================================================

void GameEngine::setupEventListeners() {
    if (!eventManager) {
        return;
    }
    
    std::cout << "[GameEngine] 设置事件监听器..." << std::endl;
    
    // 监听游戏状态切换事件
    eventManager->subscribe("GameStateChanged", 
        [this](const Event& e) {
            // 处理状态切换事件的逻辑
            std::cout << "[GameEngine] 收到状态切换事件" << std::endl;
        }, "GameEngine", 1);
    
    // 监听错误事件
    eventManager->subscribe("Error", 
        [this](const Event& e) {
            // 处理错误事件
            std::cout << "[GameEngine] 收到错误事件，考虑关闭游戏" << std::endl;
        }, "GameEngine", 0);
    
    std::cout << "[GameEngine] 事件监听器设置完成" << std::endl;
}

void GameEngine::handleSystemEvents() {
    // 处理系统级事件
    // 这个方法在主循环中每帧调用
    
    // 目前暂时空实现，后续可以添加：
    // - 检查系统资源状态
    // - 处理操作系统消息
    // - 监控性能指标
    // - 处理紧急关闭请求等
}
