/**
 * GameEngine.h
 * 
 * 游戏引擎核心类 - 整个游戏的控制中心
 * 
 * 【文件作用】：
 * 1. 管理游戏的整体生命周期（初始化→运行→关闭）
 * 2. 协调各个子系统（状态管理、事件系统、网络通信等）
 * 3. 提供统一的游戏控制接口
 * 4. 处理系统级的异常和错误恢复
 * 
 * 【设计模式】：外观模式 + 单例模式的思想
 * 【核心职责】：作为各个子系统的协调者和管理者
 */

#pragma once

#include <memory>
#include <atomic>
#include <string>

// 前向声明 - 避免循环依赖
class StateManager;
class EventManager;
class WebSocketServer;

/**
 * 游戏引擎主类
 * 
 * 【核心概念】：
 * - 作为整个游戏的"大脑"，协调所有子系统的工作
 * - 管理游戏的主循环和生命周期
 * - 提供子系统间的通信桥梁
 * - 确保游戏的稳定运行和优雅关闭
 */
class GameEngine {
private:
    // 核心子系统
    std::unique_ptr<StateManager> stateManager;     // 状态管理器
    std::unique_ptr<EventManager> eventManager;     // 事件管理器
    std::unique_ptr<WebSocketServer> webSocketServer; // 网络通信服务器
    
    // 引擎状态控制
    std::atomic<bool> initialized;  // 是否已初始化
    std::atomic<bool> running;      // 是否正在运行
    
    // 性能监控
    float targetFrameTime;          // 目标帧时间（秒）
    int frameCount;                 // 帧计数器
    
public:
    /**
     * 构造函数
     * 【作用】：创建游戏引擎实例，但不进行初始化
     */
    GameEngine();
    
    /**
     * 析构函数
     * 【作用】：确保引擎正确关闭和资源清理
     */
    ~GameEngine();
    
    // 禁用拷贝构造和赋值（游戏引擎应该是唯一的）
    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;
    
    /**
     * 初始化游戏引擎
     * 【作用】：
     * 1. 初始化所有子系统
     * 2. 建立子系统间的连接
     * 3. 加载必要的配置和资源
     * 4. 准备游戏的初始状态
     * 
     * 【返回】：true表示初始化成功，false表示失败
     * 【注意】：只能调用一次，重复调用会返回false
     */
    bool initialize();
    
    /**
     * 运行游戏主循环
     * 【作用】：
     * 1. 执行游戏的主循环
     * 2. 协调各子系统的更新
     * 3. 控制游戏的帧率
     * 4. 处理系统级事件
     * 
     * 【阻塞】：此方法会阻塞直到游戏结束
     * 【前置条件】：必须先调用initialize()
     */
    void run();
    
    /**
     * 请求关闭游戏
     * 【作用】：优雅地请求游戏结束
     * 【特点】：异步调用，不会立即停止游戏
     * 【使用场景】：响应用户关闭请求、严重错误等
     */
    void requestShutdown();
    
    /**
     * 关闭游戏引擎
     * 【作用】：
     * 1. 停止主循环
     * 2. 清理所有子系统
     * 3. 保存必要的数据
     * 4. 释放资源
     * 
     * 【注意】：可以多次调用，但只有第一次调用有效
     */
    void shutdown();
    
    /**
     * 检查引擎是否已初始化
     */
    bool isInitialized() const { return initialized.load(); }
    
    /**
     * 检查引擎是否正在运行
     */
    bool isRunning() const { return running.load(); }
    
    /**
     * 获取状态管理器
     * 【作用】：允许外部系统访问状态管理器
     * 【返回】：状态管理器指针，如果未初始化则返回nullptr
     */
    StateManager* getStateManager() const;
    
    /**
     * 获取事件管理器
     * 【作用】：允许外部系统订阅和发布事件
     * 【返回】：事件管理器指针，如果未初始化则返回nullptr
     */
    EventManager* getEventManager() const;
    
    /**
     * 获取WebSocket服务器
     * 【作用】：允许访问网络通信接口
     * 【返回】：WebSocket服务器指针，如果未初始化则返回nullptr
     */
    WebSocketServer* getWebSocketServer() const;
    
    /**
     * 设置目标帧率
     * 【参数】：fps - 目标每秒帧数
     * 【作用】：控制游戏循环的执行频率
     */
    void setTargetFPS(int fps);
    
    /**
     * 获取当前帧率
     * 【返回】：当前的实际帧率
     */
    float getCurrentFPS() const;

private:
    /**
     * 初始化子系统
     * 【作用】：创建和配置所有子系统
     * 【返回】：是否成功初始化所有子系统
     */
    bool initializeSubsystems();
    
    /**
     * 主循环的一次更新
     * 【作用】：执行一帧的游戏逻辑更新
     * 【参数】：deltaTime - 距离上次更新的时间间隔
     */
    void update(float deltaTime);
    
    /**
     * 清理子系统
     * 【作用】：按正确顺序清理所有子系统
     */
    void cleanupSubsystems();
    
    /**
     * 设置事件监听器
     * 【作用】：让引擎监听重要的系统事件
     */
    void setupEventListeners();
    
    /**
     * 处理系统级事件
     * 【作用】：响应系统关闭、错误等重要事件
     */
    void handleSystemEvents();
};
