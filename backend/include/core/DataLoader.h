/**
 * DataLoader.h
 * 
 * 游戏数据加载器 - 负责加载和管理所有游戏配置数据
 * 
 * 功能说明：
 * 1. 统一加载JSON配置文件（地点、物品、对话等）
 * 2. 提供数据缓存机制，避免重复读取文件
 * 3. 支持热重载，方便开发时调试
 * 4. 优雅的错误处理和日志输出
 */

#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <functional>

// 前向声明
struct GameConfig;
struct LocationData;
struct ItemData;
struct DialogueData;

/**
 * 游戏配置结构体
 * 存储从config.json加载的基本游戏设置
 */
struct GameConfig {
    std::string title;           // 游戏标题
    std::string version;         // 版本号
    std::string defaultLanguage; // 默认语言
    int targetFPS;              // 目标帧率
    bool autoSave;              // 是否自动保存
    bool debugMode;             // 调试模式
    
    // 路径配置
    std::string savesPath;      // 存档路径
    std::string assetsPath;     // 资源路径
    std::string logsPath;       // 日志路径
};

/**
 * 地点数据结构体
 * 存储每个游戏地点的详细信息
 */
struct LocationData {
    std::string id;                                    // 地点ID
    std::string name;                                  // 地点名称
    std::map<std::string, std::string> descriptions;   // 不同情况下的描述
    std::map<std::string, std::string> exits;          // 出口连接
    std::vector<std::string> items;                    // 地点中的物品
    std::vector<std::string> characters;               // 地点中的角色
    
    // 交互选项（如检查书架、观察路灯等）
    struct Interaction {
        std::string id;
        std::string name;
        std::string description;
        
        // 需求条件
        struct Requirements {
            std::string attribute;  // 需要的属性
            int threshold;          // 阈值
        } requirements;
        
        // 交互结果
        struct Results {
            std::string text;                    // 结果文本
            std::vector<std::string> items;      // 获得的物品
            std::vector<std::string> insights;   // 获得的洞察
        } results;
    };
    
    std::vector<Interaction> interactions;
};

/**
 * 物品数据结构体
 * 存储物品的属性和交互信息
 */
struct ItemData {
    std::string id;          // 物品ID
    std::string name;        // 物品名称
    std::string type;        // 物品类型（clue/memento/story等）
    std::string description; // 物品描述
    bool examinable;         // 是否可检查
    
    // 检查结果
    struct ExamineResults {
        std::string text;                           // 检查文本
        std::vector<std::string> insights;          // 获得的洞察
        std::map<std::string, int> attributes;      // 属性加成
    } examineResults;
};

/**
 * 对话数据结构体（预留，将来实现）
 */
struct DialogueData {
    std::string id;
    std::string speaker;
    std::string text;
    // ... 更多对话相关数据
};

/**
 * DataLoader类 - 游戏数据加载器
 * 
 * 这个类就像游戏的"图书管理员"，负责：
 * - 从文件中读取所有游戏数据
 * - 将JSON数据转换为C++结构体
 * - 缓存数据以提高性能
 * - 提供便捷的数据查询接口
 */
class DataLoader {
public:
    // 构造函数和析构函数
    DataLoader();
    ~DataLoader();

    /**
     * 初始化数据加载器
     * @param dataPath 数据文件根目录路径（默认为"../shared/data/"）
     * @return 成功返回true，失败返回false
     */
    bool initialize(const std::string& dataPath = "../shared/data/");

    /**
     * 加载所有游戏数据
     * 这个方法会依次加载配置、地点、物品、对话等所有数据
     * @return 成功返回true，失败返回false
     */
    bool loadAllData();

    /**
     * 重新加载所有数据（热重载功能）
     * 用于开发时修改数据文件后立即生效
     * @return 成功返回true，失败返回false
     */
    bool reloadAllData();

    // === 数据查询接口 ===

    /**
     * 获取游戏配置
     * @return 游戏配置结构体的引用
     */
    const GameConfig& getGameConfig() const;

    /**
     * 根据ID获取地点数据
     * @param locationId 地点ID
     * @return 地点数据指针，如果不存在返回nullptr
     */
    const LocationData* getLocation(const std::string& locationId) const;

    /**
     * 获取所有地点数据
     * @return 地点数据映射表的引用
     */
    const std::map<std::string, LocationData>& getAllLocations() const;

    /**
     * 根据ID获取物品数据
     * @param itemId 物品ID
     * @return 物品数据指针，如果不存在返回nullptr
     */
    const ItemData* getItem(const std::string& itemId) const;

    /**
     * 获取所有物品数据
     * @return 物品数据映射表的引用
     */
    const std::map<std::string, ItemData>& getAllItems() const;

    // === 实用工具方法 ===

    /**
     * 检查数据是否已加载
     * @return 已加载返回true，否则返回false
     */
    bool isDataLoaded() const;

    /**
     * 获取数据加载统计信息
     * @return 包含加载统计的字符串
     */
    std::string getLoadingStats() const;

private:
    // === 私有成员变量 ===
    std::string dataPath_;                              // 数据文件路径
    bool dataLoaded_;                                   // 数据是否已加载
    
    // 数据存储
    GameConfig gameConfig_;                             // 游戏配置
    std::map<std::string, LocationData> locations_;     // 地点数据
    std::map<std::string, ItemData> items_;             // 物品数据
    std::map<std::string, DialogueData> dialogues_;     // 对话数据（预留）
    
    // 统计信息
    int loadedLocations_;                               // 已加载地点数量
    int loadedItems_;                                   // 已加载物品数量
    int loadedDialogues_;                               // 已加载对话数量

    // === 私有方法 ===

    /**
     * 加载游戏配置文件
     * @return 成功返回true，失败返回false
     */
    bool loadGameConfig();

    /**
     * 加载地点数据文件
     * @return 成功返回true，失败返回false
     */
    bool loadLocations();

    /**
     * 加载物品数据文件
     * @return 成功返回true，失败返回false
     */
    bool loadItems();

    /**
     * 加载对话数据文件（预留）
     * @return 成功返回true，失败返回false
     */
    bool loadDialogues();

    /**
     * 读取JSON文件内容
     * @param filePath 文件路径
     * @return 文件内容字符串，失败返回空字符串
     */
    std::string readJsonFile(const std::string& filePath) const;

    /**
     * 解析JSON字符串中的字符串值
     * 简化版JSON解析，用于提取基本的字符串和数值
     * @param json JSON字符串
     * @param key 要查找的键
     * @return 对应的值，如果不存在返回空字符串
     */
    std::string parseJsonString(const std::string& json, const std::string& key) const;

    /**
     * 解析JSON字符串中的整数值
     * @param json JSON字符串
     * @param key 要查找的键
     * @return 对应的整数值，如果不存在返回0
     */
    int parseJsonInt(const std::string& json, const std::string& key) const;

    /**
     * 解析JSON字符串中的布尔值
     * @param json JSON字符串
     * @param key 要查找的键
     * @return 对应的布尔值，如果不存在返回false
     */
    bool parseJsonBool(const std::string& json, const std::string& key) const;

    /**
     * 清理所有已加载的数据
     */
    void clearAllData();
};

