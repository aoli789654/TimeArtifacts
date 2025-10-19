/**
 * GameState.h
 * 
 * 游戏状态系统 - 管理不同的游戏场景和交互
 * 
 * 功能说明：
 * 1. 定义游戏状态的基类接口
 * 2. 管理状态转换和生命周期
 * 3. 处理用户输入和游戏逻辑
 * 4. 支持不同类型的游戏场景（探索、对话、选择等）
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

// 前向声明
class GameEngine;
class DataLoader;
class DialogueState;
struct LocationData;
struct ItemData;

/**
 * 游戏状态类型枚举
 * 定义游戏中可能的状态类型
 */
enum class GameStateType {
    EXPLORATION,    // 探索状态 - 在地点间移动和交互
    DIALOGUE,       // 对话状态 - 与NPC对话
    CHOICE,         // 选择状态 - 做出重要决定
    INVENTORY,      // 背包状态 - 查看和使用物品
    ENDING          // 结局状态 - 游戏结束
};

/**
 * 玩家属性结构体
 * 存储玩家的四个核心属性
 */
struct PlayerAttributes {
    int observation;    // 观察力 - 发现线索的能力
    int communication;  // 沟通力 - 与NPC交流的效果
    int action;         // 行动力 - 主动帮助他人的意愿
    int empathy;        // 共情值 - 理解他人情感的能力（决定结局）
    
    PlayerAttributes() : observation(1), communication(1), action(1), empathy(1) {}
};

/**
 * 游戏选择结构体
 * 表示玩家可以做出的选择
 */
struct GameChoice {
    std::string id;             // 选择ID
    std::string text;           // 选择文本
    std::string description;    // 选择描述
    
    // 需求条件
    struct Requirements {
        std::string attribute;  // 需要的属性
        int threshold;          // 阈值
        std::vector<std::string> items; // 需要的物品
    } requirements;
    
    // 选择结果
    struct Results {
        std::string text;                           // 结果文本
        std::vector<std::string> items_gained;      // 获得的物品
        std::vector<std::string> items_lost;        // 失去的物品
        std::map<std::string, int> attribute_changes; // 属性变化
        std::vector<std::string> insights;          // 获得的洞察
        std::string next_state;                     // 下一个状态
    } results;
    
    bool isAvailable(const PlayerAttributes& attrs, const std::vector<std::string>& inventory) const;
};

/**
 * GameState抽象基类
 * 所有游戏状态的基类，定义了状态的通用接口
 */
class GameState {
public:
    GameState(GameStateType type, GameEngine* engine);
    virtual ~GameState() = default;
    
    // 禁用拷贝
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;
    
    /**
     * 进入状态时调用
     * 用于初始化状态相关的数据和UI
     */
    virtual void enter() = 0;
    
    /**
     * 处理用户输入
     * @param input 用户输入的命令或选择
     * @return 是否成功处理输入
     */
    virtual bool handleInput(const std::string& input) = 0;
    
    /**
     * 更新状态逻辑
     * @param deltaTime 距离上次更新的时间（秒）
     */
    virtual void update(float deltaTime) = 0;
    
    /**
     * 渲染状态内容
     * 生成当前状态的文本描述
     * @return 状态描述文本
     */
    virtual std::string render() const = 0;
    
    /**
     * 退出状态时调用
     * 用于清理状态相关的资源
     */
    virtual void exit() = 0;
    
    /**
     * 获取状态名称
     * @return 状态名称字符串
     */
    virtual std::string getName() const = 0;
    
    /**
     * 获取状态类型
     * @return 状态类型枚举
     */
    GameStateType getType() const { return type_; }
    
    /**
     * 检查状态是否完成
     * @return 状态是否已完成
     */
    virtual bool isComplete() const { return false; }

protected:
    GameStateType type_;        // 状态类型
    GameEngine* engine_;        // 游戏引擎引用
    bool initialized_;          // 是否已初始化
    
    /**
     * 获取数据加载器
     * @return DataLoader指针
     */
    DataLoader* getDataLoader() const;
    
    /**
     * 发送消息给前端
     * @param message JSON格式的消息
     */
    void sendMessage(const std::string& message) const;
    
    /**
     * 生成JSON格式的游戏状态消息
     * @param type 消息类型
     * @param data 消息数据
     * @return JSON字符串
     */
    std::string createMessage(const std::string& type, const std::string& data) const;
};

/**
 * ExplorationState类
 * 探索状态 - 处理地点移动、物品交互、NPC对话等
 */
class ExplorationState : public GameState {
public:
    ExplorationState(GameEngine* engine);
    virtual ~ExplorationState() = default;
    
    // 实现基类接口
    void enter() override;
    bool handleInput(const std::string& input) override;
    void update(float deltaTime) override;
    std::string render() const override;
    void exit() override;
    std::string getName() const override { return "Exploration"; }
    
    /**
     * 设置当前地点
     * @param locationId 地点ID
     */
    void setCurrentLocation(const std::string& locationId);
    
    /**
     * 获取当前地点
     * @return 当前地点ID
     */
    std::string getCurrentLocation() const { return currentLocation_; }
    
    /**
     * 添加物品到背包
     * @param itemId 物品ID
     */
    void addItem(const std::string& itemId);
    
    /**
     * 从背包移除物品
     * @param itemId 物品ID
     * @return 是否成功移除
     */
    bool removeItem(const std::string& itemId);
    
    /**
     * 检查背包中是否有指定物品
     * @param itemId 物品ID
     * @return 是否拥有该物品
     */
    bool hasItem(const std::string& itemId) const;
    
    /**
     * 获取背包物品列表
     * @return 物品ID列表
     */
    std::vector<std::string> getInventory() const { return inventory_; }
    
    /**
     * 获取玩家属性
     * @return 玩家属性引用
     */
    PlayerAttributes& getPlayerAttributes() { return playerAttributes_; }
    const PlayerAttributes& getPlayerAttributes() const { return playerAttributes_; }

private:
    std::string currentLocation_;           // 当前地点ID
    std::vector<std::string> inventory_;    // 背包物品列表
    PlayerAttributes playerAttributes_;     // 玩家属性
    std::vector<std::string> insights_;     // 获得的洞察
    std::vector<GameChoice> availableChoices_; // 当前可用的选择
    
    /**
     * 处理移动命令
     * @param direction 移动方向
     * @return 是否成功移动
     */
    bool handleMove(const std::string& direction);
    
    /**
     * 处理检查命令
     * @param target 检查目标
     * @return 是否成功检查
     */
    bool handleExamine(const std::string& target);
    
    /**
     * 处理交互命令
     * @param interactionId 交互ID
     * @return 是否成功交互
     */
    bool handleInteraction(const std::string& interactionId);
    
    /**
     * 处理与NPC对话
     * @param characterId 角色ID
     * @return 是否成功对话
     */
    bool handleTalk(const std::string& characterId);
    
    /**
     * 更新可用选择列表
     */
    void updateAvailableChoices();
    
    /**
     * 生成地点描述
     * @return 地点描述文本
     */
    std::string generateLocationDescription() const;
    
    /**
     * 生成可用动作列表
     * @return 动作列表文本
     */
    std::string generateAvailableActions() const;
    
    /**
     * 应用选择结果
     * @param choice 选择结果
     */
    void applyChoiceResults(const GameChoice::Results& results);
    
    /**
     * 检查任务完成条件
     * @return 是否完成主要任务
     */
    bool checkQuestCompletion() const;
};
