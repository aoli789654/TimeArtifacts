/**
 * APIHandler.h
 * 
 * API消息处理器（简化版本）
 */

#pragma once

#include <string>
#include <vector>
#include <map>

/**
 * API处理器类
 * 负责处理前端发来的消息，并生成相应的响应
 */
class APIHandler {
public:
    APIHandler();
    ~APIHandler() = default;

    /**
     * 处理收到的消息
     * @param rawMessage 原始JSON字符串消息
     * @return 响应消息的JSON字符串
     */
    std::string handleMessage(const std::string& rawMessage);
    
    /**
     * 生成游戏状态响应
     * @return 游戏状态的JSON字符串
     */
    std::string generateGameStateResponse();

private:
    // 物品分级系统
    enum class ItemGrade {
        BASIC,      // 基础版本
        PERFECT,    // 完美版本
        STORY       // 故事物品
    };
    
    struct GameItem {
        std::string itemId;
        std::string name;
        std::string description;
        ItemGrade grade;
        
        GameItem(const std::string& id, const std::string& n, const std::string& desc, ItemGrade g)
            : itemId(id), name(n), description(desc), grade(g) {}
    };

    // 游戏状态
    std::string currentLocation;
    std::map<std::string, int> playerAttributes;
    std::vector<GameItem> inventory;
    std::vector<std::string> storyItems;  // 故事物品单独追踪
    std::vector<std::string> availableActions;
    std::vector<std::string> completedDialogues;  // 已完成的对话选择
    
    // 场景状态布尔值
    std::map<std::string, bool> sceneStates;

    // 消息处理方法
    std::string handleMoveCommand(const std::string& message);
    std::string handleExamineCommand(const std::string& message);
    std::string handleTalkCommand(const std::string& message);
    std::string handleDialogueChoice(const std::string& message);
    std::string handleSaveGame(const std::string& message);
    std::string handleLoadGame(const std::string& message);
    std::string handleShowAttributesCommand();
    std::string handleShowInventoryCommand();

    // 响应生成方法
    std::string generateDialogueResponse(const std::string& speaker, const std::string& text, const std::vector<std::pair<std::string, std::string>>& options);
    std::string generateSceneUpdateResponse(const std::string& location, const std::string& description);
    std::string generateErrorResponse(const std::string& errorMessage);

    // 工具方法
    std::string getCurrentTimestamp();
    
    // 属性检查系统
    bool checkAttribute(const std::string& attribute, int requiredLevel);
    void increaseAttribute(const std::string& attribute, int amount = 1);
    
    // 物品管理系统
    void addItem(const std::string& itemId, const std::string& name, const std::string& description, ItemGrade grade);
    bool hasItem(const std::string& itemId);
    bool hasStoryItem(const std::string& itemId);
    int countMementos();  // 计算信物数量
    int countStoryItems(); // 计算故事物品数量
    
    // 对话选项生成（支持属性检查）
    struct DialogueOption {
        std::string text;
        std::string optionId;
        std::string requiredAttribute;  // 空字符串表示无要求
        int requiredLevel;              // 0表示无要求
        
        DialogueOption(const std::string& t, const std::string& id, 
                      const std::string& attr = "", int level = 0)
            : text(t), optionId(id), requiredAttribute(attr), requiredLevel(level) {}
    };
    
    std::vector<std::pair<std::string, std::string>> filterDialogueOptions(
        const std::vector<DialogueOption>& options);
    
    // 对话状态管理
    void markDialogueCompleted(const std::string& dialogueId);
    bool isDialogueCompleted(const std::string& dialogueId);
    
    // 场景状态管理
    void setSceneState(const std::string& stateKey, bool value);
    bool getSceneState(const std::string& stateKey);
    
    // 动态生成可用操作（基于对话状态）
    void updateAvailableActions();
    
    // JSON字符串转义
    std::string escapeJsonString(const std::string& input);
    
    // 获取位置描述
    std::string getLocationDescription(const std::string& location);
};