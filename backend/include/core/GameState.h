/**
 * GameState.h
 * 
 * 游戏状态基类接口 - 状态机系统的核心
 * 
 * 【文件作用】：
 * 1. 定义所有游戏状态的标准接口
 * 2. 确保状态切换的一致性
 * 3. 支持状态的生命周期管理（进入→更新→退出）
 * 4. 为状态机提供多态基础
 * 
 * 【设计模式】：抽象基类 + 模板方法模式
 * 【使用场景】：ExploringState、DialogueState、JournalState等都继承此类
 */

#pragma once

#include <string>
#include <memory>

/**
 * 游戏状态基类
 * 
 * 【核心概念】：
 * - 每个状态代表游戏的一个阶段（如探索模式、对话模式）
 * - 状态有明确的生命周期：enter → handleInput/update → exit
 * - 状态间可以切换，由StateManager统一管理
 */
class GameState {
public:
    virtual ~GameState() = default;
    
    /**
     * 进入状态时调用
     * 【作用】：初始化状态相关的资源和界面
     * 【示例】：进入对话状态时，加载对话内容并显示对话框
     */
    virtual void enter() = 0;
    
    /**
     * 处理用户输入
     * 【作用】：响应玩家的操作（移动、选择、输入文字等）
     * 【参数】：input - 用户输入的内容或命令
     * 【示例】：在探索状态下处理"look around"命令
     */
    virtual void handleInput(const std::string& input) = 0;
    
    /**
     * 更新游戏逻辑
     * 【作用】：处理状态内的逻辑更新（如动画、计时器等）
     * 【参数】：deltaTime - 距离上次更新的时间间隔（秒）
     * 【示例】：更新打字机效果的文字显示进度
     */
    virtual void update(float deltaTime) = 0;
    
    /**
     * 渲染界面
     * 【作用】：将当前状态的内容输出到前端
     * 【示例】：渲染当前场景描述、玩家属性、可用操作等
     */
    virtual void render() = 0;
    
    /**
     * 退出状态时调用
     * 【作用】：清理状态相关的资源，保存必要的数据
     * 【示例】：退出对话状态时，保存对话结果到玩家数据
     */
    virtual void exit() = 0;
    
    /**
     * 获取状态名称
     * 【作用】：用于调试和日志记录
     * 【返回】：状态的唯一标识名称
     */
    virtual std::string getName() const = 0;
    
    /**
     * 检查是否可以切换到其他状态
     * 【作用】：某些状态可能需要完成特定条件才能切换
     * 【默认实现】：总是允许切换
     */
    virtual bool canTransition() const {
        return true;
    }
    
    /**
     * 获取下一个状态（如果有自动切换需求）
     * 【作用】：支持状态的自动切换逻辑
     * 【默认实现】：无自动切换
     */
    virtual std::unique_ptr<GameState> getNextState() {
        return nullptr;
    }
};

/**
 * 状态类型枚举
 * 【作用】：便于识别和管理不同类型的状态
 */
enum class GameStateType {
    MAIN_MENU,      // 主菜单
    EXPLORING,      // 探索模式 - 玩家在场景中移动和交互
    DIALOGUE,       // 对话模式 - 与NPC对话或做选择
    JOURNAL,        // 日记模式 - 查看收集的信息和线索
    INVENTORY,      // 物品栏 - 管理持有的物品
    PAUSE_MENU,     // 暂停菜单
    SETTINGS        // 设置界面
};

/**
 * 状态工厂类前向声明
 * 【作用】：负责创建具体的状态实例
 * 【后续实现】：在StateManager中使用
 */
class StateFactory;
