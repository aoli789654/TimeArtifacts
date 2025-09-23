/**
 * GameEngine.h
 * 
 * 游戏引擎主类 - 协调所有子系统的工作
 * 
 * 职责：
 * 1. 管理游戏的生命周期
 * 2. 协调各个子系统
 * 3. 处理主游戏循环
 * 
 * 作者：后端架构师
 */

#pragma once

#include <memory>
#include <atomic>

// 前向声明
class WebSocketServer;
// 以下类将在后续阶段实现
// class StateManager;
// class EventManager;
// class DataLoader;

/**
 * 游戏引擎主类
 * 
 * 这是整个游戏的核心控制器，负责：
 * - 初始化所有子系统
 * - 管理游戏主循环
 * - 协调各模块间的交互
 */
class GameEngine {
public:
    GameEngine();
    ~GameEngine();
    
    // 禁用拷贝构造和赋值
    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;
    
    /**
     * 初始化游戏引擎
     * @return 成功返回true，失败返回false
     */
    bool initialize();
    
    /**
     * 运行游戏主循环
     * 这个函数会阻塞，直到游戏退出
     */
    void run();
    
    /**
     * 请求游戏退出
     * 这个函数是线程安全的
     */
    void requestShutdown();
    
    /**
     * 关闭游戏引擎，清理所有资源
     */
    void shutdown();
    
    /**
     * 检查游戏是否正在运行
     */
    bool isRunning() const { return running.load(); }

private:
    // 子系统管理器
    std::unique_ptr<WebSocketServer> webSocketServer;
    
    // 以下子系统将在后续阶段实现
    // std::unique_ptr<StateManager> stateManager;
    // std::unique_ptr<EventManager> eventManager;
    // std::unique_ptr<DataLoader> dataLoader;
    
    // 游戏状态
    std::atomic<bool> running{false};
    std::atomic<bool> initialized{false};
    
    /**
     * 初始化各个子系统
     */
    bool initializeSubsystems();
    
    /**
     * 游戏主循环的一次迭代
     */
    void update();
    
    /**
     * 清理所有子系统
     */
    void cleanupSubsystems();
};
