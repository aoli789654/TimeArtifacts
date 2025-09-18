# 音频与特色功能工程师入门指南

欢迎加入时光信物项目！作为音频工程师，您将负责为游戏营造沉浸式的听觉体验，同时实现一些独特的游戏功能。

## 🎯 您的职责

### 第一阶段任务（第1-3周）
1. **集成SFML音频系统** - 建立音频播放基础架构
2. **设计音频管理器** - 管理背景音乐和音效播放
3. **实现日记系统** - 玩家的个人记录和成长轨迹
4. **创建谜题系统框架** - 为小游戏和解谜提供基础
5. **音频资源管理** - 组织和优化音频文件

## 🚀 环境搭建

### 1. 安装SFML Audio
```bash
# 通过vcpkg安装（推荐）
cd vcpkg
./vcpkg install sfml[audio]

# 或者手动下载SFML
# https://www.sfml-dev.org/download.php
```

### 2. 验证SFML安装
```cpp
// 创建测试文件 test_audio.cpp
#include <SFML/Audio.hpp>
#include <iostream>

int main() {
    // 测试音频播放
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile("test.wav")) {
        std::cout << "SFML Audio 工作正常！" << std::endl;
        
        sf::Sound sound;
        sound.setBuffer(buffer);
        sound.play();
        
        // 等待播放完成
        while (sound.getStatus() == sf::Sound::Playing) {
            sf::sleep(sf::milliseconds(100));
        }
    } else {
        std::cout << "无法加载音频文件，但SFML已正确安装" << std::endl;
    }
    
    return 0;
}
```

### 3. 集成到主项目
确认 `backend/CMakeLists.txt` 中包含：
```cmake
find_package(sfml COMPONENTS audio CONFIG REQUIRED)
target_link_libraries(TimeArtifacts PRIVATE sfml-audio)
```

## 📚 核心技术栈

### 1. SFML Audio系统架构
```cpp
class AudioManager {
private:
    // 背景音乐（流式播放）
    sf::Music currentMusic;
    std::map<std::string, std::string> musicPaths;
    
    // 音效（预加载）
    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::vector<sf::Sound> activeSounds;
    
    // 音量控制
    float masterVolume = 1.0f;
    float musicVolume = 0.7f;
    float sfxVolume = 0.8f;
    
public:
    // 背景音乐控制
    void playMusic(const std::string& musicId);
    void stopMusic();
    void setMusicVolume(float volume);
    
    // 音效播放
    void playSound(const std::string& soundId);
    void preloadSound(const std::string& soundId, const std::string& filename);
    
    // 全局音量
    void setMasterVolume(float volume);
    
    // 资源管理
    void loadAudioResources(const std::string& configFile);
    void cleanup();
};
```

### 2. 音频事件响应系统
```cpp
class AudioEventHandler {
private:
    AudioManager* audioManager;
    
public:
    // 监听游戏事件并播放相应音效
    void onLocationChanged(const LocationChangedEvent& event) {
        std::string ambientMusic = getLocationMusic(event.newLocation);
        audioManager->playMusic(ambientMusic);
    }
    
    void onItemAcquired(const ItemAcquiredEvent& event) {
        std::string soundEffect = getItemSound(event.itemType);
        audioManager->playSound(soundEffect);
    }
    
    void onAttributeChanged(const AttributeChangedEvent& event) {
        if (event.newValue > event.oldValue) {
            audioManager->playSound("attribute_gain");
        }
    }
    
    void onDialogueStarted(const DialogueEvent& event) {
        audioManager->playSound("dialogue_beep");
    }
};
```

### 3. 日记系统设计
```cpp
class Journal {
public:
    struct JournalEntry {
        std::string id;
        std::string title;
        std::string content;
        std::string timestamp;
        JournalEntryType type;
        std::vector<std::string> relatedItems;
        std::vector<std::string> relatedInsights;
    };
    
    enum class JournalEntryType {
        STORY_PROGRESS,     // 剧情进展
        CHARACTER_MEETING,  // 角色初次见面
        ITEM_DISCOVERY,     // 物品发现
        INSIGHT_GAINED,     // 洞察获得
        PERSONAL_REFLECTION // 个人感悟
    };

private:
    std::vector<JournalEntry> entries;
    Player* player;

public:
    void addEntry(const std::string& title, const std::string& content, JournalEntryType type);
    void addAutoEntry(const std::string& eventId, const nlohmann::json& eventData);
    std::vector<JournalEntry> getEntriesByType(JournalEntryType type) const;
    std::vector<JournalEntry> getRecentEntries(int count = 5) const;
    
    // 与前端的接口
    nlohmann::json exportToJson() const;
    void loadFromJson(const nlohmann::json& data);
};
```

## 📋 第一周任务清单

### 任务1：实现AudioManager基础框架
- [ ] 创建 `src/audio/AudioManager.h` 和 `AudioManager.cpp`
- [ ] 实现基础的音乐播放功能
- [ ] 添加音效预加载和播放系统
- [ ] 实现音量控制功能

**测试用音频**：可以使用免费的测试音频文件
- [Freesound.org](https://freesound.org/) - 免费音效
- [Pixabay Music](https://pixabay.com/music/) - 免费背景音乐

### 任务2：创建Journal日记系统
- [ ] 创建 `src/features/Journal.h` 和 `Journal.cpp`
- [ ] 实现日记条目的添加和管理
- [ ] 设计自动日记生成机制
- [ ] 添加日记的JSON序列化功能

**日记内容示例**：
```cpp
// 自动生成的日记条目
void Journal::onLocationFirstVisit(const std::string& locationId) {
    std::string title = "初次造访：" + getLocationDisplayName(locationId);
    std::string content = generateLocationReflection(locationId);
    addEntry(title, content, JournalEntryType::STORY_PROGRESS);
}

void Journal::onCharacterFirstMeeting(const std::string& characterId) {
    std::string title = "初次见面：" + getCharacterDisplayName(characterId);
    std::string content = generateCharacterImpression(characterId);
    addEntry(title, content, JournalEntryType::CHARACTER_MEETING);
}
```

### 任务3：集成音频事件系统
- [ ] 创建 `src/audio/AudioEventHandler.h` 和 `AudioEventHandler.cpp`
- [ ] 监听游戏事件并触发相应音效
- [ ] 实现场景音乐的自动切换
- [ ] 添加音频配置文件加载功能

## 🎵 音频设计指南

### 1. 音乐分类和用途

#### 环境音乐 (Ambient Music)
- **书店**：温暖、安静的Lo-fi音乐，带有纸张翻动的细微声响
- **古街**：怀旧的轻音乐，偶尔有远处的钟声或脚步声
- **港口**：带有海风和海鸥声的深沉音乐

#### 情感音乐 (Emotional Music)
- **回忆主题**：柔和的钢琴独奏，带有一丝忧伤
- **发现主题**：充满希望的弦乐，表达新发现的喜悦
- **离别主题**：深情的小提琴，表达不舍和眷恋

#### 互动音效 (Interactive SFX)
- **界面音效**：按钮点击、菜单切换、通知提示
- **游戏音效**：物品获得、属性提升、洞察获得
- **环境音效**：脚步声、开门声、翻页声

### 2. 音频文件规范
```
assets/audio/
├── music/
│   ├── ambient/
│   │   ├── bookstore_day.ogg      (书店白天)
│   │   ├── bookstore_evening.ogg  (书店黄昏)
│   │   ├── old_street_day.ogg     (古街白天)
│   │   └── harbor_night.ogg       (港口夜晚)
│   ├── emotional/
│   │   ├── memory_theme.ogg       (回忆主题)
│   │   ├── discovery_theme.ogg    (发现主题)
│   │   └── farewell_theme.ogg     (离别主题)
│   └── special/
│       ├── main_theme.ogg         (主题曲)
│       └── ending_theme.ogg       (结局曲)
├── sfx/
│   ├── ui/
│   │   ├── button_click.wav       (按钮点击)
│   │   ├── menu_open.wav          (菜单打开)
│   │   ├── notification.wav       (通知声)
│   │   └── dialogue_beep.wav      (对话提示音)
│   ├── gameplay/
│   │   ├── item_pickup.wav        (物品获得)
│   │   ├── attribute_gain.wav     (属性提升)
│   │   ├── insight_gained.wav     (洞察获得)
│   │   └── page_turn.wav          (翻页声)
│   └── ambient/
│       ├── footsteps.wav          (脚步声)
│       ├── door_open.wav          (开门声)
│       ├── bell_ring.wav          (铃声)
│       └── rain_light.wav         (小雨声)
```

### 3. 动态音频系统
```cpp
class DynamicAudioSystem {
private:
    AudioManager* audioManager;
    std::string currentAmbientMusic;
    std::vector<std::string> ambientSounds;
    
public:
    void updateAudioForLocation(const std::string& locationId, const std::string& timeOfDay, const std::string& weather) {
        // 根据位置、时间、天气选择合适的音乐
        std::string musicKey = locationId + "_" + timeOfDay;
        if (weather == "rainy") {
            musicKey += "_rain";
        }
        
        audioManager->playMusic(musicKey);
        
        // 添加环境音效
        if (weather == "rainy") {
            audioManager->playSound("rain_light");
        }
    }
    
    void playEmotionalCue(const std::string& emotion, float intensity) {
        std::string soundId = emotion + "_" + std::to_string(int(intensity * 10));
        audioManager->playSound(soundId);
    }
};
```

## 🧩 谜题系统设计

### 1. 谜题基础框架
```cpp
class Puzzle {
protected:
    std::string id;
    std::string name;
    std::string description;
    PuzzleDifficulty difficulty;
    bool isCompleted;
    
public:
    virtual ~Puzzle() = default;
    virtual bool attemptSolve(const std::string& answer) = 0;
    virtual std::string getHint(int hintLevel) = 0;
    virtual nlohmann::json getStateJson() const = 0;
    virtual void loadFromJson(const nlohmann::json& data) = 0;
};

// 记忆重组谜题示例
class MemoryReconstructionPuzzle : public Puzzle {
private:
    std::vector<std::string> memoryFragments;
    std::vector<int> correctOrder;
    std::vector<int> currentOrder;
    
public:
    bool attemptSolve(const std::string& answer) override {
        // 解析玩家的排列顺序
        auto playerOrder = parseOrder(answer);
        
        if (playerOrder == correctOrder) {
            isCompleted = true;
            onPuzzleCompleted();
            return true;
        }
        
        return false;
    }
    
    std::string getHint(int hintLevel) override {
        switch (hintLevel) {
            case 1: return "注意时间顺序的线索...";
            case 2: return "第一个片段提到了春天...";
            case 3: return "最后一个片段是关于告别的...";
            default: return "仔细观察每个片段中的时间暗示。";
        }
    }
};
```

### 2. 小游戏系统
```cpp
class MiniGame {
protected:
    std::string gameId;
    GameState currentState;
    int score;
    int maxScore;
    
public:
    enum class GameState {
        NOT_STARTED,
        IN_PROGRESS,
        COMPLETED,
        FAILED
    };
    
    virtual void startGame() = 0;
    virtual void processInput(const std::string& input) = 0;
    virtual bool isGameOver() const = 0;
    virtual GameResult getResult() const = 0;
};

// 对话质询小游戏
class DialogueChallenge : public MiniGame {
private:
    std::vector<std::string> questions;
    std::vector<std::vector<std::string>> answers;
    std::vector<bool> correctAnswers;
    int currentQuestion;
    int remainingAttempts;
    
public:
    void startGame() override {
        currentState = GameState::IN_PROGRESS;
        currentQuestion = 0;
        remainingAttempts = 3;
        score = 0;
    }
    
    void processInput(const std::string& input) override {
        bool isCorrect = checkAnswer(currentQuestion, input);
        
        if (isCorrect) {
            score++;
            correctAnswers[currentQuestion] = true;
            onCorrectAnswer();
        } else {
            remainingAttempts--;
            onWrongAnswer();
        }
        
        if (remainingAttempts <= 0) {
            currentState = GameState::FAILED;
        } else if (currentQuestion >= questions.size() - 1) {
            currentState = GameState::COMPLETED;
        } else {
            currentQuestion++;
        }
    }
};
```

## 🔧 开发建议和最佳实践

### 1. 音频性能优化
```cpp
class AudioCache {
private:
    std::map<std::string, sf::SoundBuffer> cachedBuffers;
    size_t maxCacheSize = 50;  // 最大缓存数量
    
public:
    sf::SoundBuffer* getBuffer(const std::string& filename) {
        auto it = cachedBuffers.find(filename);
        if (it != cachedBuffers.end()) {
            return &it->second;
        }
        
        if (cachedBuffers.size() >= maxCacheSize) {
            // 清理最少使用的缓存
            clearOldestCache();
        }
        
        sf::SoundBuffer buffer;
        if (buffer.loadFromFile(filename)) {
            cachedBuffers[filename] = std::move(buffer);
            return &cachedBuffers[filename];
        }
        
        return nullptr;
    }
};
```

### 2. 线程安全的音频播放
```cpp
class ThreadSafeAudioManager : public AudioManager {
private:
    mutable std::mutex audioMutex;
    
public:
    void playSound(const std::string& soundId) override {
        std::lock_guard<std::mutex> lock(audioMutex);
        AudioManager::playSound(soundId);
    }
    
    void playMusic(const std::string& musicId) override {
        std::lock_guard<std::mutex> lock(audioMutex);
        AudioManager::playMusic(musicId);
    }
};
```

### 3. 配置文件驱动的音频系统
```json
// audio_config.json
{
    "music": {
        "bookstore_day": {
            "file": "music/ambient/bookstore_day.ogg",
            "loop": true,
            "volume": 0.6,
            "fadeIn": 2.0,
            "fadeOut": 1.5
        }
    },
    "sounds": {
        "item_pickup": {
            "file": "sfx/gameplay/item_pickup.wav",
            "volume": 0.8,
            "variations": ["item_pickup_1.wav", "item_pickup_2.wav"]
        }
    },
    "scenes": {
        "bookstore": {
            "defaultMusic": "bookstore_day",
            "timeVariations": {
                "evening": "bookstore_evening",
                "night": "bookstore_night"
            },
            "weatherVariations": {
                "rainy": "bookstore_rainy"
            }
        }
    }
}
```

## 🤝 与其他成员协作

### 与后端架构师协调
- **事件系统**：订阅游戏事件以触发相应音效
- **资源管理**：集成到统一的资源管理系统
- **性能优化**：确保音频系统不影响游戏主循环

### 与前端工程师协调
- **音频控制**：提供前端音量控制接口
- **状态同步**：同步音频播放状态到前端界面
- **用户设置**：支持用户自定义音频设置

### 与游戏系统开发者协调
- **功能集成**：将日记系统集成到玩家系统
- **事件触发**：为游戏事件提供音频反馈
- **谜题接口**：为谜题系统提供游戏逻辑接口

## 📖 学习资源

### SFML音频
- [SFML Audio教程](https://www.sfml-dev.org/tutorials/2.5/audio-sounds.php)
- [SFML Audio API参考](https://www.sfml-dev.org/documentation/2.5.1/group__audio.php)

### 游戏音频设计
- [游戏音频实现技术](https://www.amazon.com/Audio-Implementation-Interactive-Brandon-Bridgman/dp/0415533317)
- [Wwise官方教程](https://www.audiokinetic.com/en/learn/wwise-tutorials/)

### 免费音频资源
- [Freesound.org](https://freesound.org/) - 音效库
- [OpenGameArt.org](https://opengameart.org/) - 游戏素材
- [YouTube Audio Library](https://www.youtube.com/audiolibrary) - 背景音乐

## 🆘 常见问题

### Q: SFML音频不播放？
A: 检查音频文件格式（推荐WAV、OGG），确认文件路径正确，检查音量设置。

### Q: 音频播放有延迟？
A: 使用sf::Sound预加载音效，sf::Music用于长音乐。避免在主线程中加载大文件。

### Q: 如何实现音频淡入淡出？
A: 使用定时器逐渐调整音量，或使用SFML的时间函数实现平滑过渡。

### Q: 多个音效同时播放？
A: 使用多个sf::Sound对象，管理活跃音效列表，及时清理播放完成的音效。

## 🎁 实用代码片段

### 音频淡入淡出
```cpp
class AudioFader {
private:
    sf::Music* music;
    float targetVolume;
    float fadeSpeed;
    sf::Clock clock;
    
public:
    void fadeIn(sf::Music* m, float target, float duration) {
        music = m;
        targetVolume = target;
        fadeSpeed = target / duration;
        music->setVolume(0);
        music->play();
        clock.restart();
    }
    
    void update() {
        if (music && music->getStatus() == sf::Music::Playing) {
            float currentVolume = music->getVolume();
            if (currentVolume < targetVolume) {
                float deltaTime = clock.restart().asSeconds();
                float newVolume = currentVolume + fadeSpeed * deltaTime;
                music->setVolume(std::min(newVolume, targetVolume));
            }
        }
    }
};
```

### 随机音效播放
```cpp
class RandomSoundPlayer {
private:
    std::vector<sf::SoundBuffer> variations;
    std::random_device rd;
    std::mt19937 gen;
    
public:
    RandomSoundPlayer() : gen(rd()) {}
    
    void addVariation(const std::string& filename) {
        sf::SoundBuffer buffer;
        if (buffer.loadFromFile(filename)) {
            variations.push_back(std::move(buffer));
        }
    }
    
    void playRandom() {
        if (!variations.empty()) {
            std::uniform_int_distribution<> dis(0, variations.size() - 1);
            int index = dis(gen);
            
            sf::Sound sound;
            sound.setBuffer(variations[index]);
            sound.play();
        }
    }
};
```

---

**记住**：音频是营造游戏氛围的关键！每一个音效、每一段音乐都在讲述故事。

用声音为玩家创造难忘的体验，让《时光信物》的世界真正活起来！🎵✨
