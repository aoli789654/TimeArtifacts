/**
 * DataLoader.cpp
 * 
 * 游戏数据加载器实现
 */

#include "../../include/core/DataLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// 构造函数 - 初始化所有成员变量
DataLoader::DataLoader() 
    : dataPath_("shared/data/")
    , dataLoaded_(false)
    , loadedLocations_(0)
    , loadedItems_(0)
    , loadedDialogues_(0) {
    
    std::cout << "[DataLoader] 数据加载器已创建" << std::endl;
}

// 析构函数 - 清理资源
DataLoader::~DataLoader() {
    clearAllData();
    std::cout << "[DataLoader] 数据加载器已销毁" << std::endl;
}

// 初始化数据加载器
bool DataLoader::initialize(const std::string& dataPath) {
    std::cout << "[DataLoader] 正在初始化数据加载器..." << std::endl;
    std::cout << "[DataLoader] 数据路径: " << dataPath << std::endl;
    
    dataPath_ = dataPath;
    
    // 确保路径以斜杠结尾
    if (!dataPath_.empty() && dataPath_.back() != '/' && dataPath_.back() != '\\') {
        dataPath_ += "/";
    }
    
    std::cout << "[DataLoader] 数据加载器初始化完成" << std::endl;
    return true;
}

// 加载所有游戏数据
bool DataLoader::loadAllData() {
    std::cout << "[DataLoader] 开始加载所有游戏数据..." << std::endl;
    
    // 清理之前的数据
    clearAllData();
    
    bool success = true;
    
    // 1. 加载游戏配置
    std::cout << "[DataLoader] 正在加载游戏配置..." << std::endl;
    if (!loadGameConfig()) {
        std::cerr << "[DataLoader] 游戏配置加载失败" << std::endl;
        success = false;
    }
    
    // 2. 加载地点数据
    std::cout << "[DataLoader] 正在加载地点数据..." << std::endl;
    if (!loadLocations()) {
        std::cerr << "[DataLoader] 地点数据加载失败" << std::endl;
        success = false;
    }
    
    // 3. 加载物品数据
    std::cout << "[DataLoader] 正在加载物品数据..." << std::endl;
    if (!loadItems()) {
        std::cerr << "[DataLoader] 物品数据加载失败" << std::endl;
        success = false;
    }
    
    // 4. 加载对话数据（暂时跳过）
    // loadDialogues();
    
    if (success) {
        dataLoaded_ = true;
        std::cout << "[DataLoader] 所有数据加载完成！" << std::endl;
        std::cout << getLoadingStats() << std::endl;
    } else {
        std::cerr << "[DataLoader] 数据加载过程中出现错误" << std::endl;
    }
    
    return success;
}

// 重新加载所有数据
bool DataLoader::reloadAllData() {
    std::cout << "[DataLoader] 正在重新加载所有数据..." << std::endl;
    return loadAllData();
}

// 获取游戏配置
const GameConfig& DataLoader::getGameConfig() const {
    return gameConfig_;
}

// 根据ID获取地点数据
const LocationData* DataLoader::getLocation(const std::string& locationId) const {
    auto it = locations_.find(locationId);
    if (it != locations_.end()) {
        return &it->second;
    }
    return nullptr;
}

// 获取所有地点数据
const std::map<std::string, LocationData>& DataLoader::getAllLocations() const {
    return locations_;
}

// 根据ID获取物品数据
const ItemData* DataLoader::getItem(const std::string& itemId) const {
    auto it = items_.find(itemId);
    if (it != items_.end()) {
        return &it->second;
    }
    return nullptr;
}

// 获取所有物品数据
const std::map<std::string, ItemData>& DataLoader::getAllItems() const {
    return items_;
}

// 检查数据是否已加载
bool DataLoader::isDataLoaded() const {
    return dataLoaded_;
}

// 获取数据加载统计信息
std::string DataLoader::getLoadingStats() const {
    std::ostringstream stats;
    stats << "[DataLoader] 加载统计:\n";
    stats << "  - 地点数量: " << loadedLocations_ << "\n";
    stats << "  - 物品数量: " << loadedItems_ << "\n";
    stats << "  - 对话数量: " << loadedDialogues_ << "\n";
    stats << "  - 总计: " << (loadedLocations_ + loadedItems_ + loadedDialogues_) << " 项数据";
    return stats.str();
}

// === 私有方法实现 ===

// 加载游戏配置文件
bool DataLoader::loadGameConfig() {
    std::string configFile = dataPath_ + "config.json";
    std::string jsonContent = readJsonFile(configFile);
    
    if (jsonContent.empty()) {
        std::cerr << "[DataLoader] 无法读取配置文件: " << configFile << std::endl;
        return false;
    }
    
    try {
        // 解析游戏基本信息
        gameConfig_.title = parseJsonString(jsonContent, "title");
        gameConfig_.version = parseJsonString(jsonContent, "version");
        gameConfig_.defaultLanguage = parseJsonString(jsonContent, "defaultLanguage");
        
        // 解析设置信息
        gameConfig_.targetFPS = parseJsonInt(jsonContent, "targetFPS");
        gameConfig_.autoSave = parseJsonBool(jsonContent, "autoSave");
        gameConfig_.debugMode = parseJsonBool(jsonContent, "debugMode");
        
        // 解析路径信息
        gameConfig_.savesPath = parseJsonString(jsonContent, "saves");
        gameConfig_.assetsPath = parseJsonString(jsonContent, "assets");
        gameConfig_.logsPath = parseJsonString(jsonContent, "logs");
        
        std::cout << "[DataLoader] 游戏配置加载成功:" << std::endl;
        std::cout << "  - 游戏标题: " << gameConfig_.title << std::endl;
        std::cout << "  - 版本: " << gameConfig_.version << std::endl;
        std::cout << "  - 目标FPS: " << gameConfig_.targetFPS << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[DataLoader] 解析配置文件时发生错误: " << e.what() << std::endl;
        return false;
    }
}

// 加载地点数据文件
bool DataLoader::loadLocations() {
    std::string locationsFile = dataPath_ + "locations.json";
    std::string jsonContent = readJsonFile(locationsFile);
    
    if (jsonContent.empty()) {
        std::cerr << "[DataLoader] 无法读取地点文件: " << locationsFile << std::endl;
        return false;
    }
    
    try {
        // 简化版JSON解析 - 查找locations对象
        size_t locationsStart = jsonContent.find("\"locations\"");
        if (locationsStart == std::string::npos) {
            std::cerr << "[DataLoader] 地点文件格式错误：找不到locations节点" << std::endl;
            return false;
        }
        
        // 解析每个地点（简化实现，实际项目中建议使用专业的JSON库）
        // 这里我们先加载几个关键地点作为示例
        
        // 加载时光角落书店
        LocationData bookstore;
        bookstore.id = "time_corner_bookstore";
        bookstore.name = "时光角落书店";
        bookstore.descriptions["default"] = "这是一家温馨的旧书店，书架上摆满了各个年代的书籍。";
        bookstore.exits["north"] = "old_street";
        bookstore.exits["east"] = "small_courtyard";
        bookstore.items.push_back("old_diary");
        bookstore.items.push_back("dusty_photo");
        bookstore.characters.push_back("bookstore_owner");
        
        // 添加交互选项
        LocationData::Interaction examineBookshelf;
        examineBookshelf.id = "examine_bookshelf";
        examineBookshelf.name = "检查书架";
        examineBookshelf.description = "仔细查看书架上的书籍";
        examineBookshelf.requirements.attribute = "observation";
        examineBookshelf.requirements.threshold = 2;
        examineBookshelf.results.text = "在书架的角落，你发现了一本看起来很特别的日记...";
        examineBookshelf.results.items.push_back("hidden_diary");
        examineBookshelf.results.insights.push_back("bookstore_secret");
        bookstore.interactions.push_back(examineBookshelf);
        
        locations_[bookstore.id] = bookstore;
        
        // 加载老街
        LocationData oldStreet;
        oldStreet.id = "old_street";
        oldStreet.name = "老街";
        oldStreet.descriptions["default"] = "这是一条古老的石板路，两旁是历史悠久的建筑。";
        oldStreet.exits["south"] = "time_corner_bookstore";
        oldStreet.exits["west"] = "harbor";
        oldStreet.exits["north"] = "town_square";
        oldStreet.items.push_back("street_lamp_clue");
        
        // 添加交互选项
        LocationData::Interaction examineStreetLamp;
        examineStreetLamp.id = "examine_street_lamp";
        examineStreetLamp.name = "检查路灯";
        examineStreetLamp.description = "观察古老的街灯";
        examineStreetLamp.requirements.attribute = "observation";
        examineStreetLamp.requirements.threshold = 1;
        examineStreetLamp.results.text = "路灯底座上刻着一些模糊的文字...";
        examineStreetLamp.results.insights.push_back("street_history");
        oldStreet.interactions.push_back(examineStreetLamp);
        
        locations_[oldStreet.id] = oldStreet;
        
        loadedLocations_ = static_cast<int>(locations_.size());
        std::cout << "[DataLoader] 地点数据加载成功，共 " << loadedLocations_ << " 个地点" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[DataLoader] 解析地点文件时发生错误: " << e.what() << std::endl;
        return false;
    }
}

// 加载物品数据文件
bool DataLoader::loadItems() {
    std::string itemsFile = dataPath_ + "items.json";
    std::string jsonContent = readJsonFile(itemsFile);
    
    if (jsonContent.empty()) {
        std::cerr << "[DataLoader] 无法读取物品文件: " << itemsFile << std::endl;
        return false;
    }
    
    try {
        // 简化版物品数据加载
        
        // 加载泛黄的日记
        ItemData oldDiary;
        oldDiary.id = "old_diary";
        oldDiary.name = "泛黄的日记";
        oldDiary.type = "clue";
        oldDiary.description = "一本泛黄的日记，封面已经有些破损，但里面的文字依然清晰可见。";
        oldDiary.examinable = true;
        oldDiary.examineResults.text = "日记记录了某位旅人在这座城市的见闻，提到了一些神秘的'时光信物'。";
        oldDiary.examineResults.insights.push_back("diary_content");
        oldDiary.examineResults.attributes["observation"] = 1;
        items_[oldDiary.id] = oldDiary;
        
        // 加载神秘钥匙
        ItemData mysteriousKey;
        mysteriousKey.id = "mysterious_key";
        mysteriousKey.name = "神秘钥匙";
        mysteriousKey.type = "memento";
        mysteriousKey.description = "一把造型独特的钥匙，材质看起来很古老，上面刻着精美的花纹。";
        mysteriousKey.examinable = true;
        mysteriousKey.examineResults.text = "钥匙上的花纹似乎诉说着什么故事，你感觉它一定能打开某个重要的地方。";
        mysteriousKey.examineResults.insights.push_back("mysterious_key_purpose");
        items_[mysteriousKey.id] = mysteriousKey;
        
        // 加载老照片
        ItemData dustyPhoto;
        dustyPhoto.id = "dusty_photo";
        dustyPhoto.name = "老照片";
        dustyPhoto.type = "story";
        dustyPhoto.description = "一张蒙了灰尘的老照片，显示着这座城市过去的模样。";
        dustyPhoto.examinable = true;
        dustyPhoto.examineResults.text = "照片中的城市看起来比现在更加繁华，街道上人来人往，充满生机。";
        dustyPhoto.examineResults.insights.push_back("city_golden_age");
        dustyPhoto.examineResults.attributes["empathy"] = 1;
        items_[dustyPhoto.id] = dustyPhoto;
        
        // 加载隐藏的日记
        ItemData hiddenDiary;
        hiddenDiary.id = "hidden_diary";
        hiddenDiary.name = "隐藏的日记";
        hiddenDiary.type = "memento";
        hiddenDiary.description = "藏在书架后面的日记，看起来记录了一些重要的秘密。";
        hiddenDiary.examinable = true;
        hiddenDiary.examineResults.text = "这本日记详细记录了时光信物的线索，以及如何寻找它们的方法。";
        hiddenDiary.examineResults.insights.push_back("time_artifacts_location");
        hiddenDiary.examineResults.insights.push_back("search_method");
        hiddenDiary.examineResults.attributes["observation"] = 2;
        items_[hiddenDiary.id] = hiddenDiary;
        
        loadedItems_ = static_cast<int>(items_.size());
        std::cout << "[DataLoader] 物品数据加载成功，共 " << loadedItems_ << " 个物品" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[DataLoader] 解析物品文件时发生错误: " << e.what() << std::endl;
        return false;
    }
}

// 加载对话数据文件（预留）
bool DataLoader::loadDialogues() {
    // 暂时返回true，将来实现
    loadedDialogues_ = 0;
    return true;
}

// 读取JSON文件内容
std::string DataLoader::readJsonFile(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[DataLoader] 无法打开文件: " << filePath << std::endl;
        return "";
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    std::string content = buffer.str();
    std::cout << "[DataLoader] 成功读取文件: " << filePath << " (大小: " << content.size() << " 字节)" << std::endl;
    
    return content;
}

// 简化版JSON解析 - 提取字符串值
std::string DataLoader::parseJsonString(const std::string& json, const std::string& key) const {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    
    if (keyPos == std::string::npos) {
        return "";
    }
    
    // 查找冒号
    size_t colonPos = json.find(":", keyPos);
    if (colonPos == std::string::npos) {
        return "";
    }
    
    // 查找值的开始引号
    size_t valueStart = json.find("\"", colonPos);
    if (valueStart == std::string::npos) {
        return "";
    }
    valueStart++; // 跳过引号
    
    // 查找值的结束引号
    size_t valueEnd = json.find("\"", valueStart);
    if (valueEnd == std::string::npos) {
        return "";
    }
    
    return json.substr(valueStart, valueEnd - valueStart);
}

// 简化版JSON解析 - 提取整数值
int DataLoader::parseJsonInt(const std::string& json, const std::string& key) const {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    
    if (keyPos == std::string::npos) {
        return 0;
    }
    
    // 查找冒号
    size_t colonPos = json.find(":", keyPos);
    if (colonPos == std::string::npos) {
        return 0;
    }
    
    // 跳过空格，查找数字
    size_t valueStart = colonPos + 1;
    while (valueStart < json.length() && (json[valueStart] == ' ' || json[valueStart] == '\t')) {
        valueStart++;
    }
    
    // 提取数字字符串
    size_t valueEnd = valueStart;
    while (valueEnd < json.length() && (std::isdigit(json[valueEnd]) || json[valueEnd] == '-')) {
        valueEnd++;
    }
    
    if (valueEnd > valueStart) {
        std::string numberStr = json.substr(valueStart, valueEnd - valueStart);
        try {
            return std::stoi(numberStr);
        } catch (...) {
            return 0;
        }
    }
    
    return 0;
}

// 简化版JSON解析 - 提取布尔值
bool DataLoader::parseJsonBool(const std::string& json, const std::string& key) const {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    
    if (keyPos == std::string::npos) {
        return false;
    }
    
    // 查找冒号
    size_t colonPos = json.find(":", keyPos);
    if (colonPos == std::string::npos) {
        return false;
    }
    
    // 查找true或false
    size_t truePos = json.find("true", colonPos);
    size_t falsePos = json.find("false", colonPos);
    
    if (truePos != std::string::npos && (falsePos == std::string::npos || truePos < falsePos)) {
        return true;
    }
    
    return false;
}

// 清理所有已加载的数据
void DataLoader::clearAllData() {
    locations_.clear();
    items_.clear();
    dialogues_.clear();
    
    loadedLocations_ = 0;
    loadedItems_ = 0;
    loadedDialogues_ = 0;
    dataLoaded_ = false;
    
    // 重置游戏配置为默认值
    gameConfig_ = GameConfig{};
}

