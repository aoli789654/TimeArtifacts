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

private:
    // 游戏状态
    std::string currentLocation;
    std::map<std::string, int> playerAttributes;
    std::vector<std::string> inventory;
    std::vector<std::string> availableActions;

    // 消息处理方法
    std::string handleMoveCommand(const std::string& message);
    std::string handleExamineCommand(const std::string& message);
    std::string handleTalkCommand(const std::string& message);
    std::string handleDialogueChoice(const std::string& message);

    // 响应生成方法
    std::string generateGameStateResponse();
    std::string generateDialogueResponse(const std::string& speaker, const std::string& text, const std::vector<std::pair<std::string, std::string>>& options);
    std::string generateSceneUpdateResponse(const std::string& location, const std::string& description);
    std::string generateErrorResponse(const std::string& errorMessage);

    // 工具方法
    std::string getCurrentTimestamp();
};