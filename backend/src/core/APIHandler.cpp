/**
 * APIHandler.cpp
 * 
 * API消息处理器实现（简化版本）
 */

#include "APIHandler.h"
#include <iostream>
#include <sstream>
#include <chrono>

APIHandler::APIHandler() {
    std::cout << "[APIHandler] 正在创建API消息处理器" << std::endl;
    
    // 初始化默认游戏状态
    currentLocation = "bookstore";
    playerAttributes["observation"] = 1;
    playerAttributes["communication"] = 1;
    playerAttributes["action"] = 1;
    playerAttributes["empathy"] = 1;
    inventory.push_back("old_diary");
    inventory.push_back("mysterious_key");
    availableActions.push_back("examine_bookshelf");
    availableActions.push_back("talk_to_owner");
    availableActions.push_back("look_around");
    
    std::cout << "[APIHandler] 默认游戏状态已初始化" << std::endl;
}

std::string APIHandler::handleMessage(const std::string& rawMessage) {
    std::cout << "[APIHandler] 正在处理消息: " << rawMessage << std::endl;

    try {
        // 简化版本：解析基本的JSON消息
        if (rawMessage.find("\"action\"") != std::string::npos) {
            if (rawMessage.find("move") != std::string::npos) {
                return handleMoveCommand(rawMessage);
            } else if (rawMessage.find("examine") != std::string::npos) {
                return handleExamineCommand(rawMessage);
            } else if (rawMessage.find("talk") != std::string::npos) {
                return handleTalkCommand(rawMessage);
            }
        } else if (rawMessage.find("\"optionId\"") != std::string::npos) {
            return handleDialogueChoice(rawMessage);
        }
        
        // 未知消息类型，返回默认游戏状态
        return generateGameStateResponse();
        
    } catch (const std::exception& e) {
        std::cerr << "[APIHandler] 处理消息时发生错误: " << e.what() << std::endl;
        return generateErrorResponse("Failed to process message: " + std::string(e.what()));
    }
}

std::string APIHandler::handleMoveCommand(const std::string& message) {
    std::cout << "[APIHandler] 正在处理移动命令" << std::endl;
    
    // 模拟移动到新位置
    currentLocation = "old_street";
    availableActions.clear();
    availableActions.push_back("examine_street_lamp");
    availableActions.push_back("enter_bookstore");
    availableActions.push_back("walk_to_harbor");
    
    return generateSceneUpdateResponse("old_street", "You step onto the old cobblestone street. The air is cooler here, carrying the faint scent of the sea.");
}

std::string APIHandler::handleExamineCommand(const std::string& message) {
    std::cout << "[APIHandler] 正在处理检查命令" << std::endl;
    
    // 模拟属性提升
    playerAttributes["observation"]++;
    
    return generateGameStateResponse();
}

std::string APIHandler::handleTalkCommand(const std::string& message) {
    std::cout << "[APIHandler] 正在处理对话命令" << std::endl;
    
    return generateDialogueResponse(
        "Bookstore Owner",
        "Welcome to Time Corner, young one. You seem to be searching for something special.",
        {
            {"opt1", "Tell me about this city's past."},
            {"opt2", "[Observe] Notice the sadness in his eyes."}
        }
    );
}

std::string APIHandler::handleDialogueChoice(const std::string& message) {
    std::cout << "[APIHandler] 正在处理对话选择" << std::endl;
    
    if (message.find("opt1") != std::string::npos) {
        playerAttributes["communication"]++;
        return generateSceneUpdateResponse(currentLocation, "The owner nods slowly, a distant look in his eyes. 'This city holds many forgotten stories...'");
    } else if (message.find("opt2") != std::string::npos) {
        playerAttributes["empathy"]++;
        return generateDialogueResponse(
            "Bookstore Owner",
            "Ah, you have a keen eye. Indeed, some memories are best left undisturbed...",
            {}
        );
    }
    
    return generateErrorResponse("Invalid dialogue option");
}

std::string APIHandler::generateGameStateResponse() {
    std::ostringstream json;
    json << "{\n";
    json << "  \"type\": \"gameState\",\n";
    json << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    json << "  \"data\": {\n";
    json << "    \"currentLocation\": \"" << currentLocation << "\",\n";
    json << "    \"playerAttributes\": {\n";
    json << "      \"observation\": " << playerAttributes["observation"] << ",\n";
    json << "      \"communication\": " << playerAttributes["communication"] << ",\n";
    json << "      \"action\": " << playerAttributes["action"] << ",\n";
    json << "      \"empathy\": " << playerAttributes["empathy"] << "\n";
    json << "    },\n";
    json << "    \"inventory\": [";
    for (size_t i = 0; i < inventory.size(); ++i) {
        json << "\"" << inventory[i] << "\"";
        if (i < inventory.size() - 1) json << ",";
    }
    json << "],\n";
    json << "    \"availableActions\": [";
    for (size_t i = 0; i < availableActions.size(); ++i) {
        json << "\"" << availableActions[i] << "\"";
        if (i < availableActions.size() - 1) json << ",";
    }
    json << "]\n";
    json << "  }\n";
    json << "}";
    
    return json.str();
}

std::string APIHandler::generateDialogueResponse(const std::string& speaker, const std::string& text, const std::vector<std::pair<std::string, std::string>>& options) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"type\": \"dialogue\",\n";
    json << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    json << "  \"data\": {\n";
    json << "    \"speaker\": \"" << speaker << "\",\n";
    json << "    \"text\": \"" << text << "\",\n";
    json << "    \"options\": [";
    for (size_t i = 0; i < options.size(); ++i) {
        json << "{\n";
        json << "      \"id\": \"" << options[i].first << "\",\n";
        json << "      \"text\": \"" << options[i].second << "\"\n";
        json << "    }";
        if (i < options.size() - 1) json << ",";
    }
    json << "]\n";
    json << "  }\n";
    json << "}";
    
    return json.str();
}

std::string APIHandler::generateSceneUpdateResponse(const std::string& location, const std::string& description) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"type\": \"sceneUpdate\",\n";
    json << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    json << "  \"data\": {\n";
    json << "    \"location\": \"" << location << "\",\n";
    json << "    \"description\": \"" << description << "\",\n";
    json << "    \"ambientEffects\": [\"gentle_breeze\", \"distant_gulls\"],\n";
    json << "    \"musicTrack\": \"old_street_theme\"\n";
    json << "  }\n";
    json << "}";
    
    return json.str();
}

std::string APIHandler::generateErrorResponse(const std::string& errorMessage) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"type\": \"error\",\n";
    json << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    json << "  \"data\": {\n";
    json << "    \"message\": \"" << errorMessage << "\",\n";
    json << "    \"code\": 0\n";
    json << "  }\n";
    json << "}";
    
    return json.str();
}

std::string APIHandler::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    return std::to_string(time_t);
}