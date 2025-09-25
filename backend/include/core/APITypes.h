/*
 * APITypes.h
 * 前后端API接口规范
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

// 尝试包含nlohmann/json，如果失败则使用字符串处理
#ifdef __has_include
#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
#define HAS_NLOHMANN_JSON 1
#endif
#else
#define HAS_NLOHMANN_JSON 0
#endif

namespace API {

    /**
     * 消息类型枚举
     * 定义所有可能的消息类型
     */
    enum class MessageType {
        // 客户端发送给服务器的消息
        COMMAND,            // 游戏命令（移动、检查、对话等）
        DIALOGUE_CHOICE,    // 对话选择
        
        // 服务器发送给客户端的消息
        GAME_STATE,         // 游戏状态更新
        DIALOGUE,           // 对话内容
        SCENE_UPDATE,       // 场景更新
        EXAMINATION,        // 检查结果
        NOTIFICATION,       // 通知消息
        ERROR_MSG,          // 错误消息
        
        // 系统消息
        WELCOME,            // 欢迎消息
        HEARTBEAT          // 心跳消息
    };

    /**
     * 游戏动作类型
     */
    enum class ActionType {
        MOVE,              // 移动
        EXAMINE,           // 检查
        TALK,              // 对话
        USE_ITEM,          // 使用物品
        TAKE_ITEM,         // 拾取物品
        OPEN_JOURNAL,      // 打开日记
        SAVE_GAME,         // 保存游戏
        LOAD_GAME          // 加载游戏
    };

    /**
     * 玩家属性结构
     */
    struct PlayerAttributes {
        int observation = 1;    // 观察力
        int communication = 1;  // 沟通力
        int action = 1;         // 行动力
        int empathy = 1;        // 共情值

        // 转换为字符串（用于调试）
        std::string toString() const {
            return "Observation: " + std::to_string(observation) +
                   ", Communication: " + std::to_string(communication) +
                   ", Action: " + std::to_string(action) +
                   ", Empathy: " + std::to_string(empathy);
        }
    };

    /**
     * 对话选项结构
     */
    struct DialogueOption {
        std::string id;         // 选项ID
        std::string text;       // 选项文本
        std::string requirement; // 需求条件（可选）
        
        DialogueOption() = default;
        DialogueOption(const std::string& optionId, const std::string& optionText)
            : id(optionId), text(optionText) {}
        DialogueOption(const std::string& optionId, const std::string& optionText, const std::string& req)
            : id(optionId), text(optionText), requirement(req) {}
    };

    /**
     * 基础消息结构
     * 所有消息都包含这些基本信息
     */
    struct BaseMessage {
        MessageType type;
        std::string timestamp;
        
        BaseMessage(MessageType msgType) : type(msgType) {
            // 简单的时间戳（实际项目中可以用更精确的时间）
            timestamp = std::to_string(std::time(nullptr));
        }
    };

    // ====== 客户端发送给服务器的消息 ======

    /**
     * 游戏命令消息
     * 客户端发送游戏操作命令
     */
    struct CommandMessage : public BaseMessage {
        ActionType action;
        std::map<std::string, std::string> data;
        
        CommandMessage(ActionType act) : BaseMessage(MessageType::COMMAND), action(act) {}
        
        // 添加数据
        void addData(const std::string& key, const std::string& value) {
            data[key] = value;
        }
    };

    /**
     * 对话选择消息
     */
    struct DialogueChoiceMessage : public BaseMessage {
        std::string optionId;
        
        DialogueChoiceMessage(const std::string& id) 
            : BaseMessage(MessageType::DIALOGUE_CHOICE), optionId(id) {}
    };

    // ====== 服务器发送给客户端的消息 ======

    /**
     * 游戏状态消息
     * 包含当前游戏的完整状态
     */
    struct GameStateMessage : public BaseMessage {
        std::string currentLocation;
        PlayerAttributes playerAttributes;
        std::vector<std::string> inventory;
        std::vector<std::string> availableActions;
        
        GameStateMessage() : BaseMessage(MessageType::GAME_STATE) {}
    };

    /**
     * 对话消息
     */
    struct DialogueMessage : public BaseMessage {
        std::string speaker;
        std::string text;
        std::vector<DialogueOption> options;
        
        DialogueMessage() : BaseMessage(MessageType::DIALOGUE) {}
        DialogueMessage(const std::string& speakerName, const std::string& dialogueText)
            : BaseMessage(MessageType::DIALOGUE), speaker(speakerName), text(dialogueText) {}
    };

    /**
     * 场景更新消息
     */
    struct SceneUpdateMessage : public BaseMessage {
        std::string location;
        std::string description;
        std::vector<std::string> ambientEffects;
        std::string musicTrack;
        
        SceneUpdateMessage() : BaseMessage(MessageType::SCENE_UPDATE) {}
    };

    /**
     * 检查结果消息
     */
    struct ExaminationMessage : public BaseMessage {
        std::string target;
        std::string description;
        std::map<std::string, int> attributeGain;
        std::vector<std::string> newInsights;
        
        ExaminationMessage() : BaseMessage(MessageType::EXAMINATION) {}
    };

    /**
     * 通知消息
     */
    struct NotificationMessage : public BaseMessage {
        std::string title;
        std::string content;
        std::string level; // "info", "warning", "error", "success"
        
        NotificationMessage() : BaseMessage(MessageType::NOTIFICATION) {}
        NotificationMessage(const std::string& notifTitle, const std::string& notifContent, const std::string& notifLevel = "info")
            : BaseMessage(MessageType::NOTIFICATION), title(notifTitle), content(notifContent), level(notifLevel) {}
    };

    /**
     * 错误消息
     */
    struct ErrorMessage : public BaseMessage {
        std::string errorCode;
        std::string errorMessage;
        
        ErrorMessage() : BaseMessage(MessageType::ERROR_MSG) {}
        ErrorMessage(const std::string& code, const std::string& message)
            : BaseMessage(MessageType::ERROR_MSG), errorCode(code), errorMessage(message) {}
    };

    // ====== 工具函数 ======

    /**
     * 消息类型转换为字符串
     */
    inline std::string messageTypeToString(MessageType type) {
        switch (type) {
            case MessageType::COMMAND: return "command";
            case MessageType::DIALOGUE_CHOICE: return "dialogueChoice";
            case MessageType::GAME_STATE: return "gameState";
            case MessageType::DIALOGUE: return "dialogue";
            case MessageType::SCENE_UPDATE: return "sceneUpdate";
            case MessageType::EXAMINATION: return "examination";
            case MessageType::NOTIFICATION: return "notification";
            case MessageType::ERROR_MSG: return "error";
            case MessageType::WELCOME: return "welcome";
            case MessageType::HEARTBEAT: return "heartbeat";
            default: return "unknown";
        }
    }

    /**
     * 字符串转换为消息类型
     */
    inline MessageType stringToMessageType(const std::string& typeStr) {
        if (typeStr == "command") return MessageType::COMMAND;
        if (typeStr == "dialogueChoice") return MessageType::DIALOGUE_CHOICE;
        if (typeStr == "gameState") return MessageType::GAME_STATE;
        if (typeStr == "dialogue") return MessageType::DIALOGUE;
        if (typeStr == "sceneUpdate") return MessageType::SCENE_UPDATE;
        if (typeStr == "examination") return MessageType::EXAMINATION;
        if (typeStr == "notification") return MessageType::NOTIFICATION;
        if (typeStr == "error") return MessageType::ERROR_MSG;
        if (typeStr == "welcome") return MessageType::WELCOME;
        if (typeStr == "heartbeat") return MessageType::HEARTBEAT;
        return MessageType::COMMAND; // 默认值
    }

    /**
     * 动作类型转换为字符串
     */
    inline std::string actionTypeToString(ActionType action) {
        switch (action) {
            case ActionType::MOVE: return "move";
            case ActionType::EXAMINE: return "examine";
            case ActionType::TALK: return "talk";
            case ActionType::USE_ITEM: return "useItem";
            case ActionType::TAKE_ITEM: return "takeItem";
            case ActionType::OPEN_JOURNAL: return "openJournal";
            case ActionType::SAVE_GAME: return "saveGame";
            case ActionType::LOAD_GAME: return "loadGame";
            default: return "unknown";
        }
    }

    /**
     * 字符串转换为动作类型
     */
    inline ActionType stringToActionType(const std::string& actionStr) {
        if (actionStr == "move") return ActionType::MOVE;
        if (actionStr == "examine") return ActionType::EXAMINE;
        if (actionStr == "talk") return ActionType::TALK;
        if (actionStr == "useItem") return ActionType::USE_ITEM;
        if (actionStr == "takeItem") return ActionType::TAKE_ITEM;
        if (actionStr == "openJournal") return ActionType::OPEN_JOURNAL;
        if (actionStr == "saveGame") return ActionType::SAVE_GAME;
        if (actionStr == "loadGame") return ActionType::LOAD_GAME;
        return ActionType::EXAMINE; // 默认值
    }

#ifdef HAS_NLOHMANN_JSON
    // ====== JSON序列化函数（如果有nlohmann/json库）======

    /**
     * 将玩家属性转换为JSON
     */
    inline nlohmann::json playerAttributesToJson(const PlayerAttributes& attrs) {
        return nlohmann::json{
            {"observation", attrs.observation},
            {"communication", attrs.communication},
            {"action", attrs.action},
            {"empathy", attrs.empathy}
        };
    }

    /**
     * 将对话选项转换为JSON
     */
    inline nlohmann::json dialogueOptionToJson(const DialogueOption& option) {
        nlohmann::json j = {
            {"id", option.id},
            {"text", option.text}
        };
        if (!option.requirement.empty()) {
            j["requirement"] = option.requirement;
        }
        return j;
    }

    /**
     * 将游戏状态消息转换为JSON
     */
    inline nlohmann::json gameStateToJson(const GameStateMessage& msg) {
        return nlohmann::json{
            {"type", messageTypeToString(msg.type)},
            {"timestamp", msg.timestamp},
            {"data", {
                {"currentLocation", msg.currentLocation},
                {"playerAttributes", playerAttributesToJson(msg.playerAttributes)},
                {"inventory", msg.inventory},
                {"availableActions", msg.availableActions}
            }}
        };
    }

    /**
     * 将对话消息转换为JSON
     */
    inline nlohmann::json dialogueToJson(const DialogueMessage& msg) {
        nlohmann::json options = nlohmann::json::array();
        for (const auto& option : msg.options) {
            options.push_back(dialogueOptionToJson(option));
        }

        return nlohmann::json{
            {"type", messageTypeToString(msg.type)},
            {"timestamp", msg.timestamp},
            {"data", {
                {"speaker", msg.speaker},
                {"text", msg.text},
                {"options", options}
            }}
        };
    }

#endif // HAS_NLOHMANN_JSON

} // namespace API
