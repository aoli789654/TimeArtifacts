/**
 * StateManager.h
 * 
 * 状态管理器 - 状态机系统的控制中心
 * 
 * 【文件作用】：
 * 1. 管理当前活动状态和状态切换
 * 2. 协调状态的生命周期（enter → update → exit）
 * 3. 确保状态切换的安全性和一致性
 * 4. 提供状态历史记录和回退功能
 * 
 * 【设计模式】：状态模式 + 管理器模式
 * 【核心职责】：作为GameEngine和具体GameState之间的桥梁
 */

#pragma once

#include "core/GameState.h"
#include <memory>
#include <stack>
#include <string>

/**
 * 状态管理器类
 * 
 * 【核心概念】：
 * - 维护当前状态和待切换状态
 * - 处理状态间的安全切换
 * - 支持状态栈（用于暂停菜单等覆盖状态）
 * - 提供统一的状态生命周期管理
 */
class StateManager {
private:
    // 当前活动状态
    std::unique_ptr<GameState> currentState;
    
    // 下一个要切换到的状态（延迟切换机制）
    std::unique_ptr<GameState> nextState;
    
    // 状态栈（用于支持状态覆盖，如暂停菜单）
    std::stack<std::unique_ptr<GameState>> stateStack;
    
    // 状态切换标志
    bool shouldChangeState;
    bool shouldPushState;
    bool shouldPopState;
    
    // 调试信息
    std::string lastStateName;

public:
    /**
     * 构造函数
     * 【作用】：初始化状态管理器
     */
    StateManager();
    
    /**
     * 析构函数
     * 【作用】：确保所有状态正确清理
     */
    ~StateManager();
    
    /**
     * 设置初始状态
     * 【作用】：游戏启动时设置第一个状态
     * 【参数】：initialState - 初始游戏状态（通常是主菜单）
     * 【注意】：只能在没有当前状态时调用
     */
    void setInitialState(std::unique_ptr<GameState> initialState);
    
    /**
     * 切换到新状态
     * 【作用】：请求切换到新的游戏状态
     * 【参数】：newState - 目标状态
     * 【机制】：使用延迟切换，确保在安全的时机进行切换
     * 
     * 【使用示例】：
     * - 从探索状态切换到对话状态
     * - 从对话状态返回探索状态
     */
    void changeState(std::unique_ptr<GameState> newState);
    
    /**
     * 压入新状态（覆盖当前状态）
     * 【作用】：在当前状态之上添加新状态（如暂停菜单）
     * 【参数】：overlayState - 覆盖状态
     * 【特点】：不会销毁当前状态，只是暂时覆盖
     */
    void pushState(std::unique_ptr<GameState> overlayState);
    
    /**
     * 弹出当前状态（返回到之前的状态）
     * 【作用】：移除当前覆盖状态，返回到下层状态
     * 【使用场景】：关闭暂停菜单、关闭设置界面等
     */
    void popState();
    
    /**
     * 更新状态管理器
     * 【作用】：
     * 1. 处理延迟的状态切换
     * 2. 更新当前活动状态
     * 3. 检查状态的自动切换需求
     * 
     * 【参数】：deltaTime - 距离上次更新的时间间隔
     * 【调用频率】：每帧调用一次
     */
    void update(float deltaTime);
    
    /**
     * 渲染当前状态
     * 【作用】：调用当前状态的渲染方法
     * 【注意】：如果有状态栈，会按顺序渲染所有状态
     */
    void render();
    
    /**
     * 处理用户输入
     * 【作用】：将用户输入传递给当前活动状态
     * 【参数】：input - 用户输入内容
     * 【路由】：输入总是发送给栈顶状态（最上层的活动状态）
     */
    void handleInput(const std::string& input);
    
    /**
     * 获取当前状态名称
     * 【作用】：用于调试和日志记录
     * 【返回】：当前状态的名称，如果没有状态则返回"None"
     */
    std::string getCurrentStateName() const;
    
    /**
     * 检查是否有活动状态
     * 【作用】：确认状态管理器是否正常工作
     * 【返回】：true如果有当前状态，false否则
     */
    bool hasCurrentState() const;
    
    /**
     * 检查状态栈深度
     * 【作用】：用于调试状态覆盖情况
     * 【返回】：当前状态栈的深度
     */
    size_t getStateStackDepth() const;

private:
    /**
     * 执行实际的状态切换
     * 【作用】：安全地执行状态切换逻辑
     * 【内部方法】：处理状态的exit()和enter()调用
     */
    void performStateChange();
    
    /**
     * 执行状态压入操作
     * 【作用】：安全地将状态压入栈中
     */
    void performStatePush();
    
    /**
     * 执行状态弹出操作
     * 【作用】：安全地从栈中弹出状态
     */
    void performStatePop();
    
    /**
     * 清理当前状态
     * 【作用】：安全地清理状态资源
     */
    void cleanupCurrentState();
};
