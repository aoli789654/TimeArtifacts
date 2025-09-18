/**
 * GameEngine.cpp
 * 
 * 游戏引擎实现
 */

#include "core/GameEngine.h"
#include <iostream>
#include <thread>
#include <chrono>

GameEngine::GameEngine() {
    std::cout << "[GameEngine] 创建游戏引擎实例" << std::endl;
}

GameEngine::~GameEngine() {
    if (running.load()) {
        shutdown();
    }
    std::cout << "[GameEngine] 游戏引擎实例已销毁" << std::endl;
}

bool GameEngine::initialize() {
    if (initialized.load()) {
        std::cout << "[GameEngine] 警告: 游戏引擎已经初始化过了" << std::endl;
        return true;
    }
    
    std::cout << "[GameEngine] 开始初始化游戏引擎..." << std::endl;
    
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
        std::cerr << "[GameEngine] 初始化时发生异常: " << e.what() << std::endl;
        return false;
    }
}

void GameEngine::run() {
    if (!initialized.load()) {
        std::cerr << "[GameEngine] 错误: 游戏引擎未初始化" << std::endl;
        return;
    }
    
    running.store(true);
    std::cout << "[GameEngine] 启动游戏主循环" << std::endl;
    
    // 游戏主循环
    const auto targetFrameTime = std::chrono::milliseconds(16); // ~60 FPS
    
    while (running.load()) {
        auto frameStart = std::chrono::steady_clock::now();
        
        try {
            // 执行一次游戏更新
            update();
            
        } catch (const std::exception& e) {
            std::cerr << "[GameEngine] 游戏循环中发生异常: " << e.what() << std::endl;
            // 继续运行，不因单次异常而崩溃
        }
        
        // 控制帧率
        auto frameEnd = std::chrono::steady_clock::now();
        auto frameDuration = frameEnd - frameStart;
        
        if (frameDuration < targetFrameTime) {
            std::this_thread::sleep_for(targetFrameTime - frameDuration);
        }
    }
    
    std::cout << "[GameEngine] 游戏主循环已退出" << std::endl;
}

void GameEngine::requestShutdown() {
    std::cout << "[GameEngine] 收到关闭请求" << std::endl;
    running.store(false);
}

void GameEngine::shutdown() {
    std::cout << "[GameEngine] 开始关闭游戏引擎..." << std::endl;
    
    // 停止主循环
    running.store(false);
    
    // 清理所有子系统
    cleanupSubsystems();
    
    initialized.store(false);
    std::cout << "[GameEngine] 游戏引擎已关闭" << std::endl;
}

bool GameEngine::initializeSubsystems() {
    std::cout << "[GameEngine] 初始化子系统..." << std::endl;
    
    // TODO: 后端架构师在这里初始化各个子系统
    // 示例：
    // 1. 创建事件管理器
    // 2. 创建状态管理器  
    // 3. 创建WebSocket服务器
    // 4. 创建数据加载器
    
    std::cout << "[GameEngine] 所有子系统初始化完成" << std::endl;
    return true;
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
    std::cout << "[GameEngine] 清理子系统..." << std::endl;
    
    // TODO: 后端架构师在这里清理各个子系统
    // 按照初始化的逆序进行清理
    
    std::cout << "[GameEngine] 子系统清理完成" << std::endl;
}
