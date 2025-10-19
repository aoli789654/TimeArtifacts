/**
 * DialogueState.h
 * 
 * 对话状态系统 - 处理与NPC的对话交互
 * 
 * 功能说明：
 * 1. 管理与NPC的对话流程
 * 2. 处理对话选择和分支
 * 3. 应用对话结果（物品、属性变化等）
 * 4. 支持条件性对话选项
 */

#pragma once

#include "GameState.h"
#include <string>
#include <vector>
#include <map>

// 前向声明
struct CharacterData;

/**
 * 对话选项结构体
 * 表示玩家在对话中可以选择的回应
 */
struct DialogueOption {
    std::string id;             // 选项ID
    std::string text;           // 选项文本
    std::string description;    // 选项描述
    
    // 需求条件
    struct Requirements {
        std::string attribute;              // 需要的属性
        int threshold;                      // 阈值
        std::vector<std::string> items;     // 需要的物品
        std::vector<std::string> insights;  // 需要的洞察
        std::string questState;             // 需要的任务状态
    } requirements;
    
    // 选择结果
    struct Results {
        std::string responseText;                   // NPC回应文本
        std::vector<std::string> items_gained;      // 获得的物品
        std::vector<std::string> items_lost;        // 失去的物品
        std::map<std::string, int> attribute_changes; // 属性变化
        std::vector<std::string> insights;          // 获得的洞察
        std::string nextDialogue;                   // 下一个对话ID
        bool endDialogue;                           // 是否结束对话
        std::string questUpdate;                    // 任务更新
    } results;
    
    bool isAvailable(const PlayerAttributes& attrs, 
                    const std::vector<std::string>& inventory,
                    const std::vector<std::string>& insights) const;
};

/**
 * 对话节点结构体
 * 表示对话中的一个节点（NPC的一句话和可能的回应选项）
 */
struct DialogueNode {
    std::string id;                         // 节点ID
    std::string speakerName;                // 说话者名称
    std::string text;                       // 对话文本
    std::vector<DialogueOption> options;    // 可用的回应选项
    bool isEnd;                            // 是否是结束节点
    
    DialogueNode() : isEnd(false) {}
};

/**
 * 角色数据结构体
 * 存储NPC的基本信息和对话数据
 */
struct CharacterData {
    std::string id;                         // 角色ID
    std::string name;                       // 角色名称
    std::string title;                      // 角色称谓
    std::string description;                // 角色描述
    std::string location;                   // 所在地点
    
    std::map<std::string, std::string> dialogues;      // 基础对话文本
    std::map<std::string, DialogueNode> dialogueTree;  // 对话树
    std::string defaultDialogue;                        // 默认对话ID
    
    // 角色交互
    struct Interaction {
        std::string name;
        std::string description;
        struct Requirements {
            std::vector<std::string> items;
        } requirements;
        struct Results {
            std::string text;
            std::vector<std::string> items_give;
            std::map<std::string, int> attributes;
        } results;
    };
    
    std::map<std::string, Interaction> interactions;
};

/**
 * DialogueState类
 * 对话状态 - 处理与NPC的对话交互
 */
class DialogueState : public GameState {
public:
    DialogueState(GameEngine* engine);
    virtual ~DialogueState() = default;
    
    // 实现基类接口
    void enter() override;
    bool handleInput(const std::string& input) override;
    void update(float deltaTime) override;
    std::string render() const override;
    void exit() override;
    std::string getName() const override { return "Dialogue"; }
    
    /**
     * 开始与指定角色的对话
     * @param characterId 角色ID
     * @param dialogueId 对话ID（可选，默认使用角色的默认对话）
     */
    void startDialogue(const std::string& characterId, const std::string& dialogueId = "");
    
    /**
     * 设置玩家数据（从ExplorationState传入）
     * @param attributes 玩家属性
     * @param inventory 背包物品
     * @param insights 获得的洞察
     */
    void setPlayerData(const PlayerAttributes& attributes, 
                      const std::vector<std::string>& inventory,
                      const std::vector<std::string>& insights);
    
    /**
     * 获取更新后的玩家数据
     */
    const PlayerAttributes& getPlayerAttributes() const { return playerAttributes_; }
    const std::vector<std::string>& getInventory() const { return inventory_; }
    const std::vector<std::string>& getInsights() const { return insights_; }

private:
    std::string currentCharacterId_;        // 当前对话的角色ID
    std::string currentDialogueId_;         // 当前对话节点ID
    DialogueNode currentNode_;              // 当前对话节点
    
    // 玩家数据
    PlayerAttributes playerAttributes_;     // 玩家属性
    std::vector<std::string> inventory_;    // 背包物品
    std::vector<std::string> insights_;     // 获得的洞察
    
    bool dialogueActive_;                   // 对话是否激活
    
    /**
     * 加载角色数据
     * @param characterId 角色ID
     * @return 角色数据指针，失败返回nullptr
     */
    const CharacterData* loadCharacterData(const std::string& characterId);
    
    /**
     * 处理对话选择
     * @param optionIndex 选项索引
     * @return 是否成功处理
     */
    bool handleDialogueChoice(int optionIndex);
    
    /**
     * 应用对话选择结果
     * @param results 选择结果
     */
    void applyDialogueResults(const DialogueOption::Results& results);
    
    /**
     * 生成当前对话显示内容
     * @return 对话内容文本
     */
    std::string generateDialogueDisplay() const;
    
    /**
     * 生成可用选项列表
     * @return 选项列表文本
     */
    std::string generateOptionsDisplay() const;
    
    /**
     * 创建基于剧情的对话树
     * @param characterId 角色ID
     * @return 对话树映射
     */
    std::map<std::string, DialogueNode> createDialogueTree(const std::string& characterId);
    
    /**
     * 创建陈老板的对话树
     */
    std::map<std::string, DialogueNode> createChenBossDialogue();
    
    /**
     * 创建李老师的对话树
     */
    std::map<std::string, DialogueNode> createLiTeacherDialogue();
    
    /**
     * 创建张老师的对话树
     */
    std::map<std::string, DialogueNode> createZhangTeacherDialogue();
};
