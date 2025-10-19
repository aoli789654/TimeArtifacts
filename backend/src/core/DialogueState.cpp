/**
 * DialogueState.cpp
 * 
 * 对话状态系统实现
 */

#include "../../include/core/DialogueState.h"
#include "../../include/core/GameEngine.h"
#include "../../include/core/DataLoader.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// === DialogueOption实现 ===

bool DialogueOption::isAvailable(const PlayerAttributes& attrs, 
                                const std::vector<std::string>& inventory,
                                const std::vector<std::string>& insights) const {
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
    
    // 检查洞察需求
    for (const std::string& requiredInsight : requirements.insights) {
        if (std::find(insights.begin(), insights.end(), requiredInsight) == insights.end()) {
            return false;
        }
    }
    
    return true;
}

// === DialogueState实现 ===

DialogueState::DialogueState(GameEngine* engine) 
    : GameState(GameStateType::DIALOGUE, engine)
    , dialogueActive_(false) {
    std::cout << "[DialogueState] 对话状态已创建" << std::endl;
}

void DialogueState::enter() {
    std::cout << "[DialogueState] 进入对话状态" << std::endl;
    initialized_ = true;
    dialogueActive_ = true;
    
    // 发送对话开始消息
    sendMessage(createMessage("dialogueStart", "\"对话开始\""));
}

bool DialogueState::handleInput(const std::string& input) {
    if (!initialized_ || !dialogueActive_) {
        return false;
    }
    
    std::cout << "[DialogueState] 处理对话输入: " << input << std::endl;
    
    // 解析输入 - 期望是选项索引
    try {
        int optionIndex = std::stoi(input) - 1; // 转换为0基索引
        return handleDialogueChoice(optionIndex);
    } catch (const std::exception& e) {
        std::cerr << "[DialogueState] 无效的选项输入: " << input << std::endl;
        sendMessage(createMessage("error", "\"请输入有效的选项编号\""));
        return false;
    }
}

void DialogueState::update(float deltaTime) {
    // 对话状态通常不需要持续更新
    // 这里可以添加对话超时等逻辑
}

std::string DialogueState::render() const {
    if (!dialogueActive_) {
        return "对话已结束";
    }
    
    std::ostringstream output;
    output << "=== 对话中 ===" << std::endl;
    output << generateDialogueDisplay() << std::endl;
    output << std::endl;
    output << "选择你的回应:" << std::endl;
    output << generateOptionsDisplay() << std::endl;
    
    return output.str();
}

void DialogueState::exit() {
    std::cout << "[DialogueState] 退出对话状态" << std::endl;
    dialogueActive_ = false;
    initialized_ = false;
    
    // 发送对话结束消息
    sendMessage(createMessage("dialogueEnd", "\"对话结束\""));
}

void DialogueState::startDialogue(const std::string& characterId, const std::string& dialogueId) {
    std::cout << "[DialogueState] 开始与 " << characterId << " 的对话" << std::endl;
    
    currentCharacterId_ = characterId;
    
    // 创建对话树
    auto dialogueTree = createDialogueTree(characterId);
    
    // 确定起始对话节点
    std::string startDialogueId = dialogueId.empty() ? "first_meet" : dialogueId;
    
    auto nodeIt = dialogueTree.find(startDialogueId);
    if (nodeIt != dialogueTree.end()) {
        currentNode_ = nodeIt->second;
        currentDialogueId_ = startDialogueId;
        
        // 发送对话内容
        std::ostringstream dialogueData;
        dialogueData << "{";
        dialogueData << "\"character\":\"" << characterId << "\",";
        dialogueData << "\"speaker\":\"" << currentNode_.speakerName << "\",";
        dialogueData << "\"text\":\"" << currentNode_.text << "\",";
        dialogueData << "\"options\":" << generateOptionsDisplay();
        dialogueData << "}";
        
        sendMessage(createMessage("dialogueContent", dialogueData.str()));
    } else {
        std::cerr << "[DialogueState] 找不到对话节点: " << startDialogueId << std::endl;
        dialogueActive_ = false;
    }
}

void DialogueState::setPlayerData(const PlayerAttributes& attributes, 
                                 const std::vector<std::string>& inventory,
                                 const std::vector<std::string>& insights) {
    playerAttributes_ = attributes;
    inventory_ = inventory;
    insights_ = insights;
    
    std::cout << "[DialogueState] 玩家数据已设置" << std::endl;
}

// === 私有方法实现 ===

const CharacterData* DialogueState::loadCharacterData(const std::string& characterId) {
    // 这里应该从DataLoader加载角色数据
    // 目前返回nullptr，表示使用内置对话树
    return nullptr;
}

bool DialogueState::handleDialogueChoice(int optionIndex) {
    if (optionIndex < 0 || optionIndex >= static_cast<int>(currentNode_.options.size())) {
        sendMessage(createMessage("error", "\"无效的选项编号\""));
        return false;
    }
    
    const DialogueOption& selectedOption = currentNode_.options[optionIndex];
    
    // 检查选项是否可用
    if (!selectedOption.isAvailable(playerAttributes_, inventory_, insights_)) {
        sendMessage(createMessage("error", "\"你还不满足这个选项的条件\""));
        return false;
    }
    
    std::cout << "[DialogueState] 玩家选择: " << selectedOption.text << std::endl;
    
    // 应用选择结果
    applyDialogueResults(selectedOption.results);
    
    // 发送NPC回应
    std::ostringstream responseData;
    responseData << "{";
    responseData << "\"character\":\"" << currentCharacterId_ << "\",";
    responseData << "\"text\":\"" << selectedOption.results.responseText << "\"";
    responseData << "}";
    
    sendMessage(createMessage("dialogueResponse", responseData.str()));
    
    // 检查是否结束对话
    if (selectedOption.results.endDialogue) {
        dialogueActive_ = false;
        return true;
    }
    
    // 转到下一个对话节点
    if (!selectedOption.results.nextDialogue.empty()) {
        auto dialogueTree = createDialogueTree(currentCharacterId_);
        auto nextNodeIt = dialogueTree.find(selectedOption.results.nextDialogue);
        if (nextNodeIt != dialogueTree.end()) {
            currentNode_ = nextNodeIt->second;
            currentDialogueId_ = selectedOption.results.nextDialogue;
            
            // 发送新的对话内容
            std::ostringstream newDialogueData;
            newDialogueData << "{";
            newDialogueData << "\"character\":\"" << currentCharacterId_ << "\",";
            newDialogueData << "\"speaker\":\"" << currentNode_.speakerName << "\",";
            newDialogueData << "\"text\":\"" << currentNode_.text << "\",";
            newDialogueData << "\"options\":" << generateOptionsDisplay();
            newDialogueData << "}";
            
            sendMessage(createMessage("dialogueContent", newDialogueData.str()));
        }
    }
    
    return true;
}

void DialogueState::applyDialogueResults(const DialogueOption::Results& results) {
    // 应用物品变化
    for (const std::string& item : results.items_gained) {
        inventory_.push_back(item);
        std::cout << "[DialogueState] 获得物品: " << item << std::endl;
    }
    
    for (const std::string& item : results.items_lost) {
        auto it = std::find(inventory_.begin(), inventory_.end(), item);
        if (it != inventory_.end()) {
            inventory_.erase(it);
            std::cout << "[DialogueState] 失去物品: " << item << std::endl;
        }
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
        
        std::cout << "[DialogueState] 属性变化: " << attrChange.first << " +" << attrChange.second << std::endl;
    }
    
    // 应用洞察
    for (const std::string& insight : results.insights) {
        insights_.push_back(insight);
        std::cout << "[DialogueState] 获得洞察: " << insight << std::endl;
    }
}

std::string DialogueState::generateDialogueDisplay() const {
    std::ostringstream display;
    display << currentNode_.speakerName << ": \"" << currentNode_.text << "\"";
    return display.str();
}

std::string DialogueState::generateOptionsDisplay() const {
    std::ostringstream options;
    options << "[";
    
    bool first = true;
    for (size_t i = 0; i < currentNode_.options.size(); ++i) {
        const DialogueOption& option = currentNode_.options[i];
        
        if (option.isAvailable(playerAttributes_, inventory_, insights_)) {
            if (!first) options << ",";
            options << "{";
            options << "\"index\":" << (i + 1) << ",";
            options << "\"text\":\"" << option.text << "\",";
            options << "\"description\":\"" << option.description << "\"";
            options << "}";
            first = false;
        }
    }
    
    options << "]";
    return options.str();
}

std::map<std::string, DialogueNode> DialogueState::createDialogueTree(const std::string& characterId) {
    if (characterId == "chen_boss") {
        return createChenBossDialogue();
    } else if (characterId == "li_teacher") {
        return createLiTeacherDialogue();
    } else if (characterId == "zhang_teacher") {
        return createZhangTeacherDialogue();
    }
    
    // 默认对话树
    std::map<std::string, DialogueNode> defaultTree;
    DialogueNode defaultNode;
    defaultNode.id = "first_meet";
    defaultNode.speakerName = "未知角色";
    defaultNode.text = "你好。";
    defaultNode.isEnd = true;
    defaultTree["first_meet"] = defaultNode;
    
    return defaultTree;
}

std::map<std::string, DialogueNode> DialogueState::createChenBossDialogue() {
    std::map<std::string, DialogueNode> tree;
    
    // 第一次见面
    DialogueNode firstMeet;
    firstMeet.id = "first_meet";
    firstMeet.speakerName = "陈老板";
    firstMeet.text = "你想做什么，我们这倒是有各式各样的老怀表，不知道你说的是哪一种。";
    
    // 选项1：解释委托
    DialogueOption explainCommission;
    explainCommission.id = "explain_commission";
    explainCommission.text = "我是来找东西的，周奶奶的儿子托我来找一块圆形的刻有'梅'字样的老式怀表";
    explainCommission.description = "详细解释委托的来意";
    explainCommission.results.responseText = "哦，你是问这块怀表吧。这块表是去年周姐留在这的，我也不知道为什么，就说要把这块表留在这，说等到她孙女来这取。";
    explainCommission.results.nextDialogue = "about_watch";
    explainCommission.results.insights.push_back("chen_knows_watch");
    
    firstMeet.options.push_back(explainCommission);
    tree["first_meet"] = firstMeet;
    
    // 关于怀表的对话
    DialogueNode aboutWatch;
    aboutWatch.id = "about_watch";
    aboutWatch.speakerName = "陈老板";
    aboutWatch.text = "这块表还是她丈夫在我这买的，当时正好店里有台相机，还拍了下来。后来才知道她得了健忘症，这块表我也一直压在箱底，忘记还给她了。";
    
    // 选项1：请求修理怀表
    DialogueOption repairWatch;
    repairWatch.id = "repair_watch";
    repairWatch.text = "老板，能否请您修复好这块怀表？我想让它回到当初的样子，让记忆流动起来";
    repairWatch.description = "请求修理怀表，让它重新运转";
    repairWatch.results.responseText = "好的，好的。忙碌了一阵，终于修复完成。那张照片你也拿走吧，老人家也去世有些时日了，给周姐也能留个念想。";
    repairWatch.results.items_gained.push_back("mei_watch");
    repairWatch.results.items_gained.push_back("photo_frame");
    repairWatch.results.attribute_changes["communication"] = 1;
    repairWatch.results.attribute_changes["empathy"] = 2;
    repairWatch.results.endDialogue = true;
    
    // 选项2：原样带走
    DialogueOption takeAsIs;
    takeAsIs.id = "take_as_is";
    takeAsIs.text = "或许保持原样更能体现岁月的痕迹，我直接带回去吧";
    takeAsIs.description = "不修理，直接带走怀表";
    takeAsIs.results.responseText = "也好，这样也有它的意义。";
    takeAsIs.results.items_gained.push_back("mei_watch_broken");
    takeAsIs.results.attribute_changes["empathy"] = 1;
    takeAsIs.results.endDialogue = true;
    
    aboutWatch.options.push_back(repairWatch);
    aboutWatch.options.push_back(takeAsIs);
    tree["about_watch"] = aboutWatch;
    
    return tree;
}

std::map<std::string, DialogueNode> DialogueState::createLiTeacherDialogue() {
    std::map<std::string, DialogueNode> tree;
    
    // 第一次见面
    DialogueNode firstMeet;
    firstMeet.id = "first_meet";
    firstMeet.speakerName = "李老师";
    firstMeet.text = "你为什么知道这首曲子？";
    
    // 选项1：解释委托
    DialogueOption explainCommission;
    explainCommission.id = "explain_commission";
    explainCommission.text = "我收到了一份苏家的委托，来寻找《梧桐雨》的手稿";
    explainCommission.description = "说明寻找乐谱的目的";
    explainCommission.results.responseText = "原来如此。这首《梧桐雨》是周梅女士的丈夫苏先生创作的。去年周女士来这里，听到我弹奏相似旋律，激动地拿出了原谱。";
    explainCommission.results.nextDialogue = "music_story";
    
    // 选项2：说明熟悉这首曲子
    DialogueOption familiarMusic;
    familiarMusic.id = "familiar_music";
    familiarMusic.text = "我小时候常听到一位苏先生演奏这首曲子，当时就觉得它很美";
    familiarMusic.description = "表达对音乐的熟悉和喜爱";
    familiarMusic.results.responseText = "我给你讲讲便是。";
    familiarMusic.results.attribute_changes["communication"] = 1;
    familiarMusic.results.nextDialogue = "music_story";
    
    firstMeet.options.push_back(explainCommission);
    firstMeet.options.push_back(familiarMusic);
    tree["first_meet"] = firstMeet;
    
    // 音乐故事
    DialogueNode musicStory;
    musicStory.id = "music_story";
    musicStory.speakerName = "李老师";
    musicStory.text = "她说希望这首曲子能被更多人听到，就把乐谱留在这里，说这是她丈夫的遗愿。我和他是同一届音乐学院的学生，说起来，当时还是我怂恿他去追求的周梅。";
    
    // 选项1：请求乐谱
    DialogueOption requestSheet;
    requestSheet.id = "request_sheet";
    requestSheet.text = "周奶奶得了健忘症，她的孩子特地委托我来找回丢失的乐谱";
    requestSheet.description = "说明周奶奶的情况，请求乐谱";
    requestSheet.results.responseText = "拿去吧。对了，那张乐谱我也补齐了，你把它带走吧。";
    requestSheet.results.items_gained.push_back("wutongyu_sheet");
    requestSheet.results.attribute_changes["empathy"] = 1;
    requestSheet.results.nextDialogue = "final_choice";
    
    musicStory.options.push_back(requestSheet);
    tree["music_story"] = musicStory;
    
    // 最终选择
    DialogueNode finalChoice;
    finalChoice.id = "final_choice";
    finalChoice.speakerName = "李老师";
    finalChoice.text = "这可是他平生最得意的曲子了。";
    
    // 选项1：请求录制（需要沟通力>=2）
    DialogueOption requestRecording;
    requestRecording.id = "request_recording";
    requestRecording.text = "李先生，能否请您将这曲子录下来让我带回去，也好让周女士时常听一听";
    requestRecording.description = "请求录制音乐光盘";
    requestRecording.requirements.attribute = "communication";
    requestRecording.requirements.threshold = 2;
    requestRecording.results.responseText = "也好，也好，这可是他平生最得意的曲子了。";
    requestRecording.results.items_gained.push_back("wutongyu_cd");
    requestRecording.results.attribute_changes["communication"] = 1;
    requestRecording.results.attribute_changes["empathy"] = 2;
    requestRecording.results.endDialogue = true;
    
    // 选项2：只要乐谱
    DialogueOption justSheet;
    justSheet.id = "just_sheet";
    justSheet.text = "谢谢您，有乐谱就足够了";
    justSheet.description = "只要乐谱，不要录音";
    justSheet.results.responseText = "好的，希望这能帮到周女士。";
    justSheet.results.endDialogue = true;
    
    finalChoice.options.push_back(requestRecording);
    finalChoice.options.push_back(justSheet);
    tree["final_choice"] = finalChoice;
    
    return tree;
}

std::map<std::string, DialogueNode> DialogueState::createZhangTeacherDialogue() {
    std::map<std::string, DialogueNode> tree;
    
    // 第一次见面
    DialogueNode firstMeet;
    firstMeet.id = "first_meet";
    firstMeet.speakerName = "张老师";
    firstMeet.text = "1975届的毕业照？这东西可有年头了啊。你说的周梅是我们班当年的同学，人长得美，成绩也优秀，可惜后来家里出了变故便辍学了。";
    
    // 选项1：请求照片
    DialogueOption requestPhoto;
    requestPhoto.id = "request_photo";
    requestPhoto.text = "她的家人委托我来寻找这张照片，希望能帮她找回一些回忆";
    requestPhoto.description = "说明寻找照片的目的";
    requestPhoto.results.responseText = "她的集体照也一直在我这保管着。好了，照片你拿回去吧，替我向她问个好。";
    requestPhoto.results.items_gained.push_back("class_photo");
    requestPhoto.results.attribute_changes["empathy"] = 1;
    
    // 如果行动力>=2，可以找到获奖证明
    if (playerAttributes_.action >= 2) {
        requestPhoto.results.responseText += " 对了，这是当年周梅的诗歌作品获奖证明，可惜还没等奖状下来，她便走了，只好把它留在这。";
        requestPhoto.results.items_gained.push_back("award_certificate");
        requestPhoto.results.attribute_changes["empathy"] = 2; // 额外共情值
    }
    
    requestPhoto.results.endDialogue = true;
    
    firstMeet.options.push_back(requestPhoto);
    tree["first_meet"] = firstMeet;
    
    return tree;
}
