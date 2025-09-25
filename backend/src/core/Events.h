/**
 * Events.h
 * 
 * 游戏事件系统 - 事件类定义
 * 
 * 【文件作用】：
 * 1. 定义事件基类和所有具体事件类型
 * 2. 为观察者模式提供数据结构基础
 * 3. 实现游戏各系统间的松耦合通信
 * 4. 支持事件的序列化和日志记录
 * 
 * 【设计模式】：观察者模式 + 多态事件系统
 * 【核心优势】：系统间解耦、易于扩展、统一的事件处理
 * 
 * 【使用场景示例】：
 * - 玩家获得物品 → UI更新物品栏 + 音效播放 + 成就检查
 * - 属性提升 → UI动画 + 音效 + 存档更新 + 解锁新选项
 * - 场景切换 → 背景音乐变化 + UI更新 + 状态切换
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>

/**
 * 事件基类
 * 
 * 【设计原理】：
 * - 所有事件都继承这个基类，实现多态
 * - 包含时间戳，便于事件排序和调试
 * - 虚析构函数确保派生类正确清理
 */
class Event {
public:
    virtual ~Event() = default;
    
    /**
     * 获取事件类型
     * 【作用】：用于事件分发和过滤
     * 【返回】：事件的唯一类型标识符
     */
    virtual std::string getType() const = 0;
    
    /**
     * 获取事件时间戳
     * 【作用】：用于事件排序和性能分析
     */
    std::chrono::steady_clock::time_point getTimestamp() const {
        return timestamp;
    }
    
    /**
     * 获取事件优先级
     * 【作用】：高优先级事件优先处理
     * 【默认】：普通优先级(5)，范围0-10，数字越小优先级越高
     */
    virtual int getPriority() const {
        return 5;
    }
    
    /**
     * 检查事件是否可以被取消
     * 【作用】：某些关键事件不应被取消
     */
    virtual bool isCancellable() const {
        return true;
    }

protected:
    Event() : timestamp(std::chrono::steady_clock::now()) {}

private:
    std::chrono::steady_clock::time_point timestamp;
};

// =============================================================================
// 玩家相关事件
// =============================================================================

/**
 * 属性变化事件
 * 【触发时机】：玩家的观察力、沟通力、行动力、共情值发生变化
 * 【响应者】：UI系统（更新显示）、音效系统（播放提升音效）、成就系统
 */
class AttributeChangedEvent : public Event {
public:
    std::string attributeName;  // 属性名称："observation", "communication", "action", "empathy"
    int oldValue;              // 变化前的值
    int newValue;              // 变化后的值
    std::string reason;        // 变化原因："dialogue_choice", "item_examination", "story_progress"
    
    AttributeChangedEvent(const std::string& name, int old_val, int new_val, const std::string& change_reason = "")
        : attributeName(name), oldValue(old_val), newValue(new_val), reason(change_reason) {}
    
    std::string getType() const override { return "AttributeChanged"; }
    
    /**
     * 获取属性变化量
     */
    int getDelta() const { return newValue - oldValue; }
    
    /**
     * 检查是否为属性提升
     */
    bool isImprovement() const { return newValue > oldValue; }
};

/**
 * 物品获得事件
 * 【触发时机】：玩家获得新物品（时光信物、线索、故事物品）
 * 【响应者】：物品栏UI、音效系统、成就系统、剧情系统
 */
class ItemAcquiredEvent : public Event {
public:
    std::string itemId;        // 物品唯一ID
    std::string itemName;      // 物品显示名称
    std::string itemType;      // 物品类型："memento", "clue", "story"
    std::string source;        // 获得来源："examination", "dialogue", "discovery"
    
    ItemAcquiredEvent(const std::string& id, const std::string& name, 
                     const std::string& type = "story", const std::string& item_source = "")
        : itemId(id), itemName(name), itemType(type), source(item_source) {}
    
    std::string getType() const override { return "ItemAcquired"; }
    
    // 物品获得通常比较重要，优先级较高
    int getPriority() const override { return 3; }
};

/**
 * 物品失去事件
 * 【触发时机】：玩家失去物品（使用消耗品、交易等）
 */
class ItemLostEvent : public Event {
public:
    std::string itemId;
    std::string itemName;
    std::string reason;        // 失去原因："used", "traded", "story_requirement"
    
    ItemLostEvent(const std::string& id, const std::string& name, const std::string& lose_reason = "")
        : itemId(id), itemName(name), reason(lose_reason) {}
    
    std::string getType() const override { return "ItemLost"; }
};

// =============================================================================
// 场景和探索相关事件
// =============================================================================

/**
 * 场景切换事件
 * 【触发时机】：玩家从一个场景移动到另一个场景
 * 【响应者】：场景管理器、背景音乐系统、UI系统、保存系统
 */
class LocationChangedEvent : public Event {
public:
    std::string fromLocation;  // 离开的场景ID
    std::string toLocation;    // 进入的场景ID
    std::string transitionType; // 切换类型："walk", "door", "teleport"
    
    LocationChangedEvent(const std::string& from, const std::string& to, 
                        const std::string& transition = "walk")
        : fromLocation(from), toLocation(to), transitionType(transition) {}
    
    std::string getType() const override { return "LocationChanged"; }
    
    // 场景切换需要及时响应
    int getPriority() const override { return 2; }
};

/**
 * 物体检查事件
 * 【触发时机】：玩家检查场景中的物体或环境
 * 【响应者】：属性系统（可能提升观察力）、剧情系统、音效系统
 */
class ObjectExaminedEvent : public Event {
public:
    std::string objectId;      // 被检查物体的ID
    std::string objectName;    // 物体名称
    std::string locationId;    // 所在场景
    bool firstTimeExamined;    // 是否首次检查
    
    ObjectExaminedEvent(const std::string& obj_id, const std::string& obj_name, 
                       const std::string& location, bool first_time = false)
        : objectId(obj_id), objectName(obj_name), locationId(location), firstTimeExamined(first_time) {}
    
    std::string getType() const override { return "ObjectExamined"; }
};

// =============================================================================
// 对话和剧情相关事件
// =============================================================================

/**
 * 对话开始事件
 * 【触发时机】：开始与NPC对话
 * 【响应者】：状态管理器（切换到对话状态）、UI系统、音效系统
 */
class DialogueStartedEvent : public Event {
public:
    std::string characterId;   // NPC角色ID
    std::string characterName; // NPC名称
    std::string dialogueId;    // 对话ID
    
    DialogueStartedEvent(const std::string& char_id, const std::string& char_name, 
                        const std::string& dialogue_id)
        : characterId(char_id), characterName(char_name), dialogueId(dialogue_id) {}
    
    std::string getType() const override { return "DialogueStarted"; }
    int getPriority() const override { return 1; } // 对话切换优先级最高
};

/**
 * 对话选择事件
 * 【触发时机】：玩家在对话中做出选择
 * 【响应者】：剧情系统、属性系统、音效系统
 */
class DialogueChoiceEvent : public Event {
public:
    std::string dialogueId;
    std::string choiceId;      // 选择的选项ID
    std::string choiceText;    // 选择的文本内容
    std::vector<std::string> requirements; // 选择的前置条件
    
    DialogueChoiceEvent(const std::string& dialogue_id, const std::string& choice_id, 
                       const std::string& choice_text)
        : dialogueId(dialogue_id), choiceId(choice_id), choiceText(choice_text) {}
    
    std::string getType() const override { return "DialogueChoice"; }
};

/**
 * 对话结束事件
 * 【触发时机】：对话结束，返回探索模式
 */
class DialogueEndedEvent : public Event {
public:
    std::string characterId;
    std::string dialogueId;
    std::string endReason;     // 结束原因："completed", "interrupted", "choice_exit"
    
    DialogueEndedEvent(const std::string& char_id, const std::string& dialogue_id, 
                      const std::string& reason = "completed")
        : characterId(char_id), dialogueId(dialogue_id), endReason(reason) {}
    
    std::string getType() const override { return "DialogueEnded"; }
};

// =============================================================================
// 洞察和发现相关事件
// =============================================================================

/**
 * 洞察获得事件
 * 【触发时机】：玩家获得新的洞察或线索
 * 【响应者】：日记系统、UI提示系统、音效系统、剧情解锁系统
 */
class InsightGainedEvent : public Event {
public:
    std::string insightId;     // 洞察ID
    std::string description;   // 洞察描述
    std::string category;      // 洞察类别："character", "location", "story", "mystery"
    std::string trigger;       // 触发方式："dialogue", "examination", "item_use"
    
    InsightGainedEvent(const std::string& id, const std::string& desc, 
                      const std::string& cat = "story", const std::string& trig = "")
        : insightId(id), description(desc), category(cat), trigger(trig) {}
    
    std::string getType() const override { return "InsightGained"; }
    int getPriority() const override { return 3; } // 洞察比较重要
};

/**
 * 谜题解决事件
 * 【触发时机】：玩家成功解决游戏中的谜题或难题
 */
class PuzzleSolvedEvent : public Event {
public:
    std::string puzzleId;
    std::string puzzleName;
    std::string solution;      // 解决方案
    int attempts;              // 尝试次数
    
    PuzzleSolvedEvent(const std::string& id, const std::string& name, 
                     const std::string& sol = "", int att = 1)
        : puzzleId(id), puzzleName(name), solution(sol), attempts(att) {}
    
    std::string getType() const override { return "PuzzleSolved"; }
    int getPriority() const override { return 4; }
};

// =============================================================================
// 系统相关事件
// =============================================================================

/**
 * 游戏状态切换事件
 * 【触发时机】：游戏状态发生切换（探索→对话→日记等）
 * 【响应者】：UI系统、音效系统、输入处理系统
 */
class GameStateChangedEvent : public Event {
public:
    std::string fromState;     // 原状态
    std::string toState;       // 目标状态
    std::string trigger;       // 切换触发原因
    
    GameStateChangedEvent(const std::string& from, const std::string& to, 
                         const std::string& trig = "")
        : fromState(from), toState(to), trigger(trig) {}
    
    std::string getType() const override { return "GameStateChanged"; }
    int getPriority() const override { return 1; } // 状态切换优先级最高
    bool isCancellable() const override { return false; } // 状态切换不可取消
};

/**
 * 游戏保存事件
 * 【触发时机】：游戏数据需要保存
 */
class GameSavedEvent : public Event {
public:
    std::string saveSlot;      // 存档槽位
    std::string saveTime;      // 保存时间
    bool autoSave;             // 是否为自动保存
    
    GameSavedEvent(const std::string& slot, const std::string& time, bool auto_save = false)
        : saveSlot(slot), saveTime(time), autoSave(auto_save) {}
    
    std::string getType() const override { return "GameSaved"; }
};

/**
 * 错误事件
 * 【触发时机】：系统发生错误
 * 【响应者】：日志系统、错误处理系统、UI提示系统
 */
class ErrorEvent : public Event {
public:
    std::string errorCode;     // 错误代码
    std::string errorMessage;  // 错误信息
    std::string source;        // 错误来源
    
    ErrorEvent(const std::string& code, const std::string& message, 
              const std::string& src = "")
        : errorCode(code), errorMessage(message), source(src) {}
    
    std::string getType() const override { return "Error"; }
    int getPriority() const override { return 0; } // 错误事件最高优先级
    bool isCancellable() const override { return false; } // 错误事件不可取消
};
