# 游戏系统开发者入门指南

欢迎加入时光信物项目！作为游戏系统开发者，您将负责实现游戏的核心逻辑，包括玩家系统、物品管理、属性机制等。

## 🎯 您的职责

### 第一阶段任务（第1-3周）
1. **实现Player类** - 玩家属性、物品栏、洞察系统
2. **创建物品系统** - 物品类层次结构和使用机制
3. **设计洞察系统** - 基于选择的深度互动机制
4. **建立游戏规则引擎** - 处理玩家行为和游戏逻辑
5. **存档系统接口** - 游戏状态的保存和加载

## 🚀 环境搭建

### 1. 确认C++环境
```bash
# 确认编译器版本
g++ --version  # 需要支持C++17
clang++ --version

# 确认CMake版本  
cmake --version  # 需要3.16以上
```

### 2. 了解项目结构
```
backend/
├── src/
│   ├── gameplay/          # ← 您的主要工作目录
│   │   ├── Player.cpp
│   │   ├── Item.cpp
│   │   ├── Inventory.cpp
│   │   └── ...
│   ├── core/             # 核心引擎（架构师负责）
│   └── main.cpp
├── include/
│   ├── gameplay/         # ← 您的头文件目录
│   └── core/
└── shared/data/          # ← 游戏数据文件
```

### 3. 编译和测试
```bash
cd backend/build
make
./TimeArtifacts

# 应该能看到程序正常启动
```

## 📚 核心设计概念

### 1. 玩家系统架构
```cpp
class Player {
private:
    std::string name;
    std::string currentLocation;
    
    // 四大属性系统
    std::map<AttributeType, int> attributes;
    
    // 洞察系统 - 深度互动的核心
    std::set<std::string> insights;
    
    // 物品栏
    std::unique_ptr<Inventory> inventory;
    
    // 游戏进度标记
    std::set<std::string> storyFlags;
    std::set<std::string> completedQuests;

public:
    // 属性操作
    int getAttribute(AttributeType type) const;
    void addAttribute(AttributeType type, int delta);
    bool hasAttributeThreshold(AttributeType type, int threshold) const;
    
    // 洞察系统
    void addInsight(const std::string& insightId);
    bool hasInsight(const std::string& insightId) const;
    
    // 物品管理
    bool addItem(std::unique_ptr<Item> item);
    Item* getItem(const std::string& itemId) const;
    
    // 游戏进度
    void setStoryFlag(const std::string& flag);
    bool hasStoryFlag(const std::string& flag) const;
};
```

### 2. 物品系统设计
```cpp
// 基础物品类
class Item {
protected:
    std::string id;
    std::string name;
    std::string description;
    ItemType type;
    bool canUse;

public:
    virtual ~Item() = default;
    virtual bool use(Player& player) = 0;
    virtual std::string getDisplayName() const { return name; }
    virtual ItemType getType() const { return type; }
};

// 时光信物 - 游戏的核心物品
class MementoItem : public Item {
private:
    int mementoValue;  // 信物的记忆价值
    std::vector<std::string> relatedInsights;

public:
    bool use(Player& player) override {
        // 使用时光信物可以获得深度洞察
        for (const auto& insight : relatedInsights) {
            player.addInsight(insight);
        }
        return true;
    }
};

// 线索物品 - 帮助解谜
class ClueItem : public Item {
private:
    std::vector<std::string> clueInsights;
    std::map<AttributeType, int> attributeBonus;

public:
    bool use(Player& player) override {
        // 使用线索物品提升相关属性
        for (const auto& [attr, bonus] : attributeBonus) {
            player.addAttribute(attr, bonus);
        }
        return true;
    }
};
```

### 3. 洞察系统核心机制
```cpp
class InsightSystem {
private:
    std::map<std::string, InsightData> insights;
    
public:
    struct InsightData {
        std::string id;
        std::string name;
        std::string description;
        std::vector<std::string> prerequisites;  // 前置洞察
        std::map<std::string, std::string> unlockedDialogues;  // 解锁的对话选项
    };
    
    bool canUnlockInsight(const Player& player, const std::string& insightId) const {
        const auto& insight = insights.at(insightId);
        for (const auto& prereq : insight.prerequisites) {
            if (!player.hasInsight(prereq)) {
                return false;
            }
        }
        return true;
    }
};
```

## 📋 第一周任务清单

### 任务1：实现Player类基础框架
- [ ] 创建 `src/gameplay/Player.h` 和 `Player.cpp`
- [ ] 实现四种属性的管理（观察力、沟通力、行动力、共情值）
- [ ] 添加基础的getter/setter方法
- [ ] 实现位置管理系统

**参考数据**：查看 `shared/data/config.json` 中的默认属性值

### 任务2：创建Inventory物品栏系统
- [ ] 创建 `src/gameplay/Inventory.h` 和 `Inventory.cpp`
- [ ] 实现物品的添加、移除、查找功能
- [ ] 支持不同类型物品的分类管理
- [ ] 添加物品栏容量限制

**核心功能**：
```cpp
class Inventory {
private:
    std::vector<std::unique_ptr<Item>> items;
    size_t maxSize;

public:
    bool addItem(std::unique_ptr<Item> item);
    bool removeItem(const std::string& itemId);
    Item* findItem(const std::string& itemId) const;
    std::vector<Item*> getItemsByType(ItemType type) const;
    size_t getItemCount() const { return items.size(); }
    bool isFull() const { return items.size() >= maxSize; }
};
```

### 任务3：实现洞察系统
- [ ] 创建 `src/gameplay/Insight.h` 和 `Insight.cpp`
- [ ] 设计洞察数据结构
- [ ] 实现洞察的获得和检查机制
- [ ] 与对话系统的集成接口

**洞察示例**：
```cpp
// 玩家通过观察获得洞察
player.addInsight("bookstore_secret");

// 在对话中检查洞察
if (player.hasInsight("bookstore_secret")) {
    // 解锁特殊对话选项
    addDialogueOption("[洞察] 您是不是在这里藏了什么重要的东西？");
}
```

## 🎮 游戏机制设计

### 1. 属性系统详解

#### 观察力 (Observation)
- **作用**：发现隐藏物品、解锁特殊交互
- **提升方式**：仔细观察环境、检查细节
- **应用场景**：发现书架后的隐藏日记、注意到NPC的微表情

#### 沟通力 (Communication)  
- **作用**：解锁更多对话选项、获得NPC信任
- **提升方式**：与NPC对话、选择合适的回应
- **应用场景**：说服书店老板分享秘密、与害羞的角色建立联系

#### 行动力 (Action)
- **作用**：执行需要勇气的行为、开启封锁区域
- **提升方式**：做出果断的选择、帮助他人
- **应用场景**：爬上危险的阁楼、主动介入冲突

#### 共情值 (Empathy)
- **作用**：理解他人情感、获得深层故事线索
- **提升方式**：关怀他人、做出富有同情心的选择
- **应用场景**：安慰伤心的NPC、理解角色的内心痛苦

### 2. 游戏进程设计
```cpp
class GameProgress {
private:
    int mementosFound = 0;      // 已找到的时光信物数量
    int totalMementos = 3;      // 游戏总共需要找到的信物数量
    
public:
    bool isGameComplete() const {
        return mementosFound >= totalMementos;
    }
    
    void onMementoFound(const std::string& mementoId) {
        mementosFound++;
        // 触发相应的游戏事件
        EventManager::getInstance().publish(
            std::make_unique<MementoFoundEvent>(mementoId)
        );
    }
};
```

## 🔧 开发建议和最佳实践

### 1. 智能指针使用
```cpp
// 使用unique_ptr管理物品内存
std::unique_ptr<Item> createItem(const std::string& itemId) {
    if (itemId == "old_diary") {
        return std::make_unique<ClueItem>("old_diary", "泛黄的日记");
    }
    // ...
}

// 在Player类中使用
bool Player::addItem(std::unique_ptr<Item> item) {
    return inventory->addItem(std::move(item));
}
```

### 2. 枚举类型的使用
```cpp
enum class AttributeType {
    OBSERVATION,
    COMMUNICATION,
    ACTION,
    EMPATHY
};

enum class ItemType {
    MEMENTO,    // 时光信物
    CLUE,       // 线索物品  
    STORY       // 故事物品
};
```

### 3. 事件驱动设计
```cpp
// 当属性变化时发布事件
void Player::addAttribute(AttributeType type, int delta) {
    int oldValue = attributes[type];
    attributes[type] += delta;
    
    // 发布属性变化事件
    EventManager::getInstance().publish(
        std::make_unique<AttributeChangedEvent>(type, oldValue, attributes[type])
    );
}
```

### 4. JSON数据集成
```cpp
// 从JSON文件加载物品数据
void ItemFactory::loadFromJson(const std::string& filename) {
    std::ifstream file(filename);
    nlohmann::json itemsJson;
    file >> itemsJson;
    
    for (const auto& [itemId, itemData] : itemsJson["items"].items()) {
        // 解析物品数据并创建物品对象
        auto item = createItemFromJson(itemId, itemData);
        itemTemplates[itemId] = std::move(item);
    }
}
```

## 🤝 与其他成员协作

### 与后端架构师协调
- **API接口**：定义玩家状态的网络传输格式
- **事件系统**：使用架构师提供的事件管理器
- **数据持久化**：配合实现存档功能

### 与前端工程师协调
- **数据格式**：确保游戏状态能正确显示在界面上
- **实时更新**：支持属性、物品等状态的实时同步

### 与音频工程师协调
- **事件触发**：为物品使用、属性提升等事件提供音频触发点
- **状态联动**：某些音效可能依赖于玩家的状态或位置

## 📖 学习资源

### C++进阶
- [现代C++教程](https://changkun.de/modern-cpp/)
- [C++核心指南](https://isocpp.github.io/CppCoreGuidelines/)
- [智能指针详解](https://zh.cppreference.com/w/cpp/memory)

### 游戏设计
- [游戏机制设计艺术](https://book.douban.com/subject/26791007/)
- [游戏设计的艺术](https://book.douban.com/subject/26264554/)

### 数据结构与算法
- [STL容器使用指南](https://zh.cppreference.com/w/cpp/container)

## 🆘 常见问题

### Q: 如何处理复杂的游戏状态？
A: 使用状态模式和数据驱动设计。将复杂逻辑分解为小的、可测试的函数。

### Q: 内存管理有什么注意事项？
A: 优先使用智能指针，避免手动new/delete。遵循RAII原则。

### Q: 如何设计可扩展的物品系统？
A: 使用多态和工厂模式。每种物品类型继承基类Item，通过JSON配置创建物品。

### Q: 怎样平衡游戏难度？
A: 通过数据驱动的配置文件调整属性要求、物品效果等参数。添加难度测试函数。

## 🎁 实用代码模板

### 属性检查模板
```cpp
template<typename T>
bool checkRequirement(const Player& player, const std::string& reqType, const T& threshold) {
    if (reqType == "observation") {
        return player.getAttribute(AttributeType::OBSERVATION) >= threshold;
    }
    // 添加其他检查类型...
    return false;
}
```

### 游戏事件模板
```cpp
class GameEvent {
public:
    virtual ~GameEvent() = default;
    virtual std::string getType() const = 0;
    virtual nlohmann::json toJson() const = 0;
};

class ItemAcquiredEvent : public GameEvent {
private:
    std::string itemId;
    std::string playerName;

public:
    ItemAcquiredEvent(const std::string& id, const std::string& name)
        : itemId(id), playerName(name) {}
    
    std::string getType() const override { return "ItemAcquired"; }
    
    nlohmann::json toJson() const override {
        return {
            {"type", "ItemAcquired"},
            {"itemId", itemId},
            {"player", playerName}
        };
    }
};
```

---

**记住**：您实现的游戏逻辑是整个项目的核心！专注于创建有趣、平衡、有深度的游戏机制。

每一行代码都可能影响玩家的游戏体验，保持对细节的关注！🎮
