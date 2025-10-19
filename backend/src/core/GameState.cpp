/**
 * GameState.cpp
 * 
 * 游戏状态系统实现
 */

#include "../../include/core/GameState.h"
#include "../../include/core/GameEngine.h"
#include "../../include/core/DataLoader.h"
#include "WebSocketServer.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// === GameChoice实现 ===

bool GameChoice::isAvailable(const PlayerAttributes& attrs, const std::vector<std::string>& inventory) const {
    // 检查属性需求
    if (!requirements.attribute.empty()) {
        int playerValue = 0;
        if (requirements.attribute == "observation") {
            playerValue = attrs.observation;
        } else if (requirements.attribute == "communication") {
            playerValue = attrs.communication;
        } else if (requirements.attribute == "action") {
            playerValue = attrs.action;
        } else if (requirements.attribute == "empathy") {
            playerValue = attrs.empathy;
        }
        
        if (playerValue < requirements.threshold) {
            return false;
        }
    }
    
    // 检查物品需求
    for (const std::string& requiredItem : requirements.items) {
        if (std::find(inventory.begin(), inventory.end(), requiredItem) == inventory.end()) {
            return false;
        }
    }
    
    return true;
}

// === GameState基类实现 ===

GameState::GameState(GameStateType type, GameEngine* engine) 
    : type_(type), engine_(engine), initialized_(false) {
    std::cout << "[GameState] 游戏状态已创建，类型: " << static_cast<int>(type) << std::endl;
}

DataLoader* GameState::getDataLoader() const {
    if (engine_) {
        return engine_->getDataLoader();
    }
    return nullptr;
}

void GameState::sendMessage(const std::string& message) const {
    if (engine_ && engine_->getWebSocketServer()) {
        // 这里应该通过WebSocket发送消息给前端
        // 目前先输出到控制台
        std::cout << "[GameState] 发送消息: " << message << std::endl;
    }
}

std::string GameState::createMessage(const std::string& type, const std::string& data) const {
    // 简化版JSON生成
    std::ostringstream json;
    json << "{\"type\":\"" << type << "\",\"data\":" << data << "}";
    return json.str();
}

// === ExplorationState实现 ===

ExplorationState::ExplorationState(GameEngine* engine) 
    : GameState(GameStateType::EXPLORATION, engine)
    , currentLocation_("time_corner_bookstore") {
    std::cout << "[ExplorationState] 探索状态已创建" << std::endl;
}

void ExplorationState::enter() {
    std::cout << "[ExplorationState] 进入探索状态" << std::endl;
    initialized_ = true;
    
    // 初始化玩家属性
    playerAttributes_ = PlayerAttributes();
    
    // 设置起始地点
    setCurrentLocation("time_corner_bookstore");
    
    // 更新可用选择
    updateAvailableChoices();
    
    // 发送初始状态给前端
    std::string locationDesc = generateLocationDescription();
    std::string actions = generateAvailableActions();
    
    std::ostringstream gameState;
    gameState << "{";
    gameState << "\"location\":\"" << currentLocation_ << "\",";
    gameState << "\"description\":\"" << locationDesc << "\",";
    gameState << "\"actions\":" << actions << ",";
    gameState << "\"attributes\":{";
    gameState << "\"observation\":" << playerAttributes_.observation << ",";
    gameState << "\"communication\":" << playerAttributes_.communication << ",";
    gameState << "\"action\":" << playerAttributes_.action << ",";
    gameState << "\"empathy\":" << playerAttributes_.empathy;
    gameState << "},";
    gameState << "\"inventory\":[";
    for (size_t i = 0; i < inventory_.size(); ++i) {
        gameState << "\"" << inventory_[i] << "\"";
        if (i < inventory_.size() - 1) gameState << ",";
    }
    gameState << "]";
    gameState << "}";
    
    sendMessage(createMessage("gameState", gameState.str()));
}

bool ExplorationState::handleInput(const std::string& input) {
    if (!initialized_) {
        std::cerr << "[ExplorationState] 状态未初始化，无法处理输入" << std::endl;
        return false;
    }
    
    std::cout << "[ExplorationState] 处理输入: " << input << std::endl;
    
    // 解析输入命令
    std::istringstream iss(input);
    std::string command;
    iss >> command;
    
    // 转换为小写
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    if (command == "move" || command == "go") {
        std::string direction;
        iss >> direction;
        return handleMove(direction);
    } else if (command == "examine" || command == "look") {
        std::string target;
        iss >> target;
        return handleExamine(target);
    } else if (command == "interact" || command == "use") {
        std::string interactionId;
        iss >> interactionId;
        return handleInteraction(interactionId);
    } else if (command == "talk") {
        std::string characterId;
        iss >> characterId;
        return handleTalk(characterId);
    } else if (command == "help") {
        sendMessage(createMessage("help", "\"可用命令: move <方向>, examine <目标>, interact <交互ID>, talk <角色ID>\""));
        return true;
    } else {
        sendMessage(createMessage("error", "\"未知命令: " + command + "\""));
        return false;
    }
}

void ExplorationState::update(float deltaTime) {
    // 探索状态的更新逻辑
    static float updateTimer = 0.0f;
    updateTimer += deltaTime;
    
    // 每5秒检查一次任务状态
    if (updateTimer >= 5.0f) {
        updateTimer = 0.0f;
        
        if (checkQuestCompletion()) {
            std::cout << "[ExplorationState] 主要任务已完成！" << std::endl;
            sendMessage(createMessage("questComplete", "\"恭喜！你已经收集到了所有的时光信物。\""));
        }
    }
}

std::string ExplorationState::render() const {
    std::ostringstream output;
    
    output << "=== " << getCurrentLocation() << " ===" << std::endl;
    output << generateLocationDescription() << std::endl;
    output << std::endl;
    output << "可用动作:" << std::endl;
    output << generateAvailableActions() << std::endl;
    output << std::endl;
    output << "玩家属性: 观察力=" << playerAttributes_.observation;
    output << " 沟通力=" << playerAttributes_.communication;
    output << " 行动力=" << playerAttributes_.action;
    output << " 共情值=" << playerAttributes_.empathy << std::endl;
    
    if (!inventory_.empty()) {
        output << "背包物品: ";
        for (size_t i = 0; i < inventory_.size(); ++i) {
            output << inventory_[i];
            if (i < inventory_.size() - 1) output << ", ";
        }
        output << std::endl;
    }
    
    return output.str();
}

void ExplorationState::exit() {
    std::cout << "[ExplorationState] 退出探索状态" << std::endl;
    initialized_ = false;
}

void ExplorationState::setCurrentLocation(const std::string& locationId) {
    currentLocation_ = locationId;
    std::cout << "[ExplorationState] 当前地点设置为: " << locationId << std::endl;
    
    // 更新可用选择
    updateAvailableChoices();
}

void ExplorationState::addItem(const std::string& itemId) {
    if (std::find(inventory_.begin(), inventory_.end(), itemId) == inventory_.end()) {
        inventory_.push_back(itemId);
        std::cout << "[ExplorationState] 获得物品: " << itemId << std::endl;
        
        // 发送物品获得消息
        sendMessage(createMessage("itemGained", "\"" + itemId + "\""));
    }
}

bool ExplorationState::removeItem(const std::string& itemId) {
    auto it = std::find(inventory_.begin(), inventory_.end(), itemId);
    if (it != inventory_.end()) {
        inventory_.erase(it);
        std::cout << "[ExplorationState] 失去物品: " << itemId << std::endl;
        
        // 发送物品失去消息
        sendMessage(createMessage("itemLost", "\"" + itemId + "\""));
        return true;
    }
    return false;
}

bool ExplorationState::hasItem(const std::string& itemId) const {
    return std::find(inventory_.begin(), inventory_.end(), itemId) != inventory_.end();
}

// === 私有方法实现 ===

bool ExplorationState::handleMove(const std::string& direction) {
    DataLoader* dataLoader = getDataLoader();
    if (!dataLoader) {
        sendMessage(createMessage("error", "\"数据加载器不可用\""));
        return false;
    }
    
    const LocationData* currentLoc = dataLoader->getLocation(currentLocation_);
    if (!currentLoc) {
        sendMessage(createMessage("error", "\"当前地点数据不存在\""));
        return false;
    }
    
    // 查找出口
    auto exitIt = currentLoc->exits.find(direction);
    if (exitIt == currentLoc->exits.end()) {
        sendMessage(createMessage("error", "\"这个方向没有出口\""));
        return false;
    }
    
    std::string newLocation = exitIt->second;
    const LocationData* newLoc = dataLoader->getLocation(newLocation);
    if (!newLoc) {
        sendMessage(createMessage("error", "\"目标地点数据不存在\""));
        return false;
    }
    
    // 移动到新地点
    setCurrentLocation(newLocation);
    
    // 发送移动成功消息
    std::string locationDesc = generateLocationDescription();
    std::ostringstream moveResult;
    moveResult << "{\"newLocation\":\"" << newLocation << "\",\"description\":\"" << locationDesc << "\"}";
    sendMessage(createMessage("moved", moveResult.str()));
    
    return true;
}

bool ExplorationState::handleExamine(const std::string& target) {
    DataLoader* dataLoader = getDataLoader();
    if (!dataLoader) {
        return false;
    }
    
    // 检查是否是背包中的物品
    if (hasItem(target)) {
        const ItemData* item = dataLoader->getItem(target);
        if (item && item->examinable) {
            std::ostringstream examineResult;
            examineResult << "{\"item\":\"" << target << "\",\"text\":\"" << item->examineResults.text << "\"}";
            sendMessage(createMessage("examined", examineResult.str()));
            
            // 应用检查结果
            for (const std::string& insight : item->examineResults.insights) {
                insights_.push_back(insight);
            }
            
            // 应用属性变化
            for (const auto& attrChange : item->examineResults.attributes) {
                if (attrChange.first == "observation") {
                    playerAttributes_.observation += attrChange.second;
                } else if (attrChange.first == "communication") {
                    playerAttributes_.communication += attrChange.second;
                } else if (attrChange.first == "action") {
                    playerAttributes_.action += attrChange.second;
                } else if (attrChange.first == "empathy") {
                    playerAttributes_.empathy += attrChange.second;
                }
            }
            
            return true;
        }
    }
    
    sendMessage(createMessage("error", "\"无法检查该物品\""));
    return false;
}

bool ExplorationState::handleInteraction(const std::string& interactionId) {
    DataLoader* dataLoader = getDataLoader();
    if (!dataLoader) {
        return false;
    }
    
    const LocationData* currentLoc = dataLoader->getLocation(currentLocation_);
    if (!currentLoc) {
        return false;
    }
    
    // 查找交互选项
    for (const auto& interaction : currentLoc->interactions) {
        if (interaction.id == interactionId) {
            // 检查需求条件
            bool canInteract = true;
            if (!interaction.requirements.attribute.empty()) {
                int playerValue = 0;
                if (interaction.requirements.attribute == "observation") {
                    playerValue = playerAttributes_.observation;
                } else if (interaction.requirements.attribute == "communication") {
                    playerValue = playerAttributes_.communication;
                } else if (interaction.requirements.attribute == "action") {
                    playerValue = playerAttributes_.action;
                } else if (interaction.requirements.attribute == "empathy") {
                    playerValue = playerAttributes_.empathy;
                }
                
                if (playerValue < interaction.requirements.threshold) {
                    canInteract = false;
                }
            }
            
            if (!canInteract) {
                sendMessage(createMessage("error", "\"你的能力还不足以进行这个交互\""));
                return false;
            }
            
            // 执行交互
            std::ostringstream interactionResult;
            interactionResult << "{\"interaction\":\"" << interactionId << "\",\"text\":\"" << interaction.results.text << "\"}";
            sendMessage(createMessage("interacted", interactionResult.str()));
            
            // 应用交互结果
            for (const std::string& item : interaction.results.items) {
                addItem(item);
            }
            
            for (const std::string& insight : interaction.results.insights) {
                insights_.push_back(insight);
            }
            
            // 更新可用选择
            updateAvailableChoices();
            
            return true;
        }
    }
    
    sendMessage(createMessage("error", "\"未找到该交互选项\""));
    return false;
}

bool ExplorationState::handleTalk(const std::string& characterId) {
    DataLoader* dataLoader = getDataLoader();
    if (!dataLoader) {
        return false;
    }
    
    const LocationData* currentLoc = dataLoader->getLocation(currentLocation_);
    if (!currentLoc) {
        return false;
    }
    
    // 检查角色是否在当前地点
    bool characterFound = false;
    for (const std::string& character : currentLoc->characters) {
        if (character == characterId) {
            characterFound = true;
            break;
        }
    }
    
    if (!characterFound) {
        sendMessage(createMessage("error", "\"这里没有这个角色\""));
        return false;
    }
    
    // 发送切换到对话状态的消息
    std::ostringstream talkData;
    talkData << "{\"character\":\"" << characterId << "\",\"location\":\"" << currentLocation_ << "\"}";
    sendMessage(createMessage("startDialogue", talkData.str()));
    
    std::cout << "[ExplorationState] 开始与 " << characterId << " 对话" << std::endl;
    return true;
}

void ExplorationState::updateAvailableChoices() {
    availableChoices_.clear();
    
    DataLoader* dataLoader = getDataLoader();
    if (!dataLoader) {
        return;
    }
    
    const LocationData* currentLoc = dataLoader->getLocation(currentLocation_);
    if (!currentLoc) {
        return;
    }
    
    // 添加地点交互选择
    for (const auto& interaction : currentLoc->interactions) {
        GameChoice choice;
        choice.id = interaction.id;
        choice.text = interaction.name;
        choice.description = interaction.description;
        
        // 设置需求条件
        choice.requirements.attribute = interaction.requirements.attribute;
        choice.requirements.threshold = interaction.requirements.threshold;
        
        // 设置结果
        choice.results.text = interaction.results.text;
        choice.results.items_gained = interaction.results.items;
        choice.results.insights = interaction.results.insights;
        
        availableChoices_.push_back(choice);
    }
    
    std::cout << "[ExplorationState] 更新可用选择，共 " << availableChoices_.size() << " 个选择" << std::endl;
}

std::string ExplorationState::generateLocationDescription() const {
    DataLoader* dataLoader = getDataLoader();
    if (!dataLoader) {
        return "数据加载器不可用";
    }
    
    const LocationData* location = dataLoader->getLocation(currentLocation_);
    if (!location) {
        return "地点数据不存在";
    }
    
    // 返回默认描述
    auto descIt = location->descriptions.find("default");
    if (descIt != location->descriptions.end()) {
        return descIt->second;
    }
    
    return "这里没有描述";
}

std::string ExplorationState::generateAvailableActions() const {
    std::ostringstream actions;
    actions << "[";
    
    bool first = true;
    for (const auto& choice : availableChoices_) {
        if (choice.isAvailable(playerAttributes_, inventory_)) {
            if (!first) actions << ",";
            actions << "{\"id\":\"" << choice.id << "\",\"text\":\"" << choice.text << "\",\"description\":\"" << choice.description << "\"}";
            first = false;
        }
    }
    
    actions << "]";
    return actions.str();
}

void ExplorationState::applyChoiceResults(const GameChoice::Results& results) {
    // 应用物品变化
    for (const std::string& item : results.items_gained) {
        addItem(item);
    }
    
    for (const std::string& item : results.items_lost) {
        removeItem(item);
    }
    
    // 应用属性变化
    for (const auto& attrChange : results.attribute_changes) {
        if (attrChange.first == "observation") {
            playerAttributes_.observation += attrChange.second;
        } else if (attrChange.first == "communication") {
            playerAttributes_.communication += attrChange.second;
        } else if (attrChange.first == "action") {
            playerAttributes_.action += attrChange.second;
        } else if (attrChange.first == "empathy") {
            playerAttributes_.empathy += attrChange.second;
        }
    }
    
    // 应用洞察
    for (const std::string& insight : results.insights) {
        insights_.push_back(insight);
    }
}

bool ExplorationState::checkQuestCompletion() const {
    // 检查是否收集到了三件时光信物
    return hasItem("mei_watch") && hasItem("wutongyu_sheet") && hasItem("class_photo");
}
