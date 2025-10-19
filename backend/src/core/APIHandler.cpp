/**
 * APIHandler.cpp
 * 
 * API消息处理器实现（简化版本）
 */

#include "APIHandler.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>

APIHandler::APIHandler() {
    std::cout << "[APIHandler] 正在创建API消息处理器" << std::endl;
    
    // 初始化默认游戏状态
    currentLocation = "时光角落书店";
    playerAttributes["observation"] = 1;
    playerAttributes["communication"] = 1;
    playerAttributes["action"] = 1;
    playerAttributes["empathy"] = 1;
    
    // 添加初始物品
    addItem("quest_letter", "委托信", "苏晓委托寻找三件信物的信件", ItemGrade::STORY);
    availableActions.push_back("整理书架");
    availableActions.push_back("查看记忆寻回处");
    availableActions.push_back("前往老街区");
    
    std::cout << "[APIHandler] 默认游戏状态已初始化" << std::endl;
}

std::string APIHandler::handleMessage(const std::string& rawMessage) {
    std::cout << "[APIHandler] 正在处理消息: " << rawMessage << std::endl;

    try {
        // 处理初始化消息
        if (rawMessage.find("\"type\":\"init\"") != std::string::npos || 
            rawMessage.find("\"type\": \"init\"") != std::string::npos) {
            std::cout << "[APIHandler] 处理初始化消息" << std::endl;
            return generateGameStateResponse();
        }
        
        // 处理命令消息
        if (rawMessage.find("\"type\":\"command\"") != std::string::npos || 
            rawMessage.find("\"type\": \"command\"") != std::string::npos) {
            std::cout << "[APIHandler] 处理命令消息" << std::endl;
            return generateGameStateResponse();
        }
        
        // 处理行动消息
        if (rawMessage.find("\"type\":\"action\"") != std::string::npos || 
            rawMessage.find("\"type\": \"action\"") != std::string::npos) {
            std::cout << "[APIHandler] 处理行动消息: " << rawMessage << std::endl;
            if (rawMessage.find("move") != std::string::npos || rawMessage.find("前往") != std::string::npos || rawMessage.find("离开") != std::string::npos || rawMessage.find("回到") != std::string::npos) {
                std::cout << "[APIHandler] 识别为移动命令" << std::endl;
                return handleMoveCommand(rawMessage);
            } else if (rawMessage.find("examine") != std::string::npos || rawMessage.find("观察") != std::string::npos) {
                std::cout << "[APIHandler] 识别为观察命令，当前位置: " << currentLocation << std::endl;
                return handleExamineCommand(rawMessage);
            } else if (rawMessage.find("talk") != std::string::npos || rawMessage.find("对话") != std::string::npos || rawMessage.find("交谈") != std::string::npos || rawMessage.find("询问") != std::string::npos || rawMessage.find("说明来意") != std::string::npos || rawMessage.find("交付时光信物") != std::string::npos) {
                std::cout << "[APIHandler] 识别为对话命令" << std::endl;
                return handleTalkCommand(rawMessage);
            } else if (rawMessage.find("查看属性") != std::string::npos) {
                std::cout << "[APIHandler] 识别为查看属性命令" << std::endl;
                return handleShowAttributesCommand();
            } else if (rawMessage.find("查看物品") != std::string::npos) {
                std::cout << "[APIHandler] 识别为查看物品命令" << std::endl;
                return handleShowInventoryCommand();
            }
            std::cout << "[APIHandler] 未识别的行动类型，返回默认游戏状态" << std::endl;
            return generateGameStateResponse();
        }
        
        // 处理对话选择消息
        if (rawMessage.find("\"type\":\"dialogue_choice\"") != std::string::npos || 
            rawMessage.find("\"type\": \"dialogue_choice\"") != std::string::npos) {
            std::cout << "[APIHandler] 处理对话选择消息" << std::endl;
            return handleDialogueChoice(rawMessage);
        }
        
        // 处理保存游戏消息
        if (rawMessage.find("\"type\":\"save_game\"") != std::string::npos || 
            rawMessage.find("\"type\": \"save_game\"") != std::string::npos) {
            std::cout << "[APIHandler] 处理保存游戏消息" << std::endl;
            return handleSaveGame(rawMessage);
        }
        
        // 处理读取游戏消息
        if (rawMessage.find("\"type\":\"load_game\"") != std::string::npos || 
            rawMessage.find("\"type\": \"load_game\"") != std::string::npos) {
            std::cout << "[APIHandler] 处理读取游戏消息" << std::endl;
            return handleLoadGame(rawMessage);
        }
        
        // 处理对话选择
        if (rawMessage.find("\"type\":\"dialogueChoice\"") != std::string::npos || 
            rawMessage.find("\"type\": \"dialogueChoice\"") != std::string::npos) {
            return handleDialogueChoice(rawMessage);
        }
        
        // 简化版本：解析基本的JSON消息（向后兼容）
        if (rawMessage.find("\"action\"") != std::string::npos) {
            if (rawMessage.find("move") != std::string::npos || rawMessage.find("离开") != std::string::npos || rawMessage.find("前往") != std::string::npos) {
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
        std::cout << "[APIHandler] 未知消息类型，返回默认状态" << std::endl;
        return generateGameStateResponse();
        
    } catch (const std::exception& e) {
        std::cerr << "[APIHandler] 处理消息时发生错误: " << e.what() << std::endl;
        return generateErrorResponse("Failed to process message: " + std::string(e.what()));
    }
}

std::string APIHandler::handleMoveCommand(const std::string& message) {
    std::cout << "[APIHandler] 正在处理移动命令" << std::endl;
    
    // 解析目标地点
    std::string targetLocation = "";
    if (message.find("前往老街区") != std::string::npos || message.find("老街区") != std::string::npos) {
        targetLocation = "老街区";
        currentLocation = "老街区";
        availableActions.clear();
        availableActions.push_back("前往钟表铺");
        availableActions.push_back("前往乐器行");
        availableActions.push_back("前往育才中学");
        availableActions.push_back("前往和平路32号茶馆");
        availableActions.push_back("回到书店");
        return generateSceneUpdateResponse("老街区", "狭窄的巷子里，石板路被岁月磨得光滑。两旁是各种老店铺，承载着这个城市的记忆。");
    }
    else if (message.find("离开钟表铺") != std::string::npos) {
        std::cout << "[APIHandler] 离开钟表铺，返回老街区" << std::endl;
        targetLocation = "老街区";
        currentLocation = "老街区";
        availableActions.clear();
        availableActions.push_back("前往钟表铺");
        availableActions.push_back("前往乐器行");
        availableActions.push_back("前往育才中学");
        availableActions.push_back("前往和平路32号茶馆");
        availableActions.push_back("回到书店");
        return generateSceneUpdateResponse("老街区", "你离开了钟表铺，回到了老街区的石板路上。");
    }
    else if (message.find("前往钟表铺") != std::string::npos || (message.find("钟表铺") != std::string::npos && message.find("离开") == std::string::npos)) {
        targetLocation = "钟表铺";
        currentLocation = "钟表铺";
        updateAvailableActions();
        return generateSceneUpdateResponse("钟表铺", "老街区狭窄的巷子里，\\\"老周钟表铺\\\"的招牌已经歪斜，玻璃窗上蒙着一层薄灰。你缓步走进店内，店主陈老板悠然的躺在太师椅上看着报，似乎并没有注意到你。");
    }
    else if (message.find("离开乐器行") != std::string::npos) {
        std::cout << "[APIHandler] 离开乐器行，返回老街区" << std::endl;
        targetLocation = "老街区";
        currentLocation = "老街区";
        availableActions.clear();
        availableActions.push_back("前往钟表铺");
        availableActions.push_back("前往乐器行");
        availableActions.push_back("前往育才中学");
        availableActions.push_back("前往和平路32号茶馆");
        availableActions.push_back("回到书店");
        return generateSceneUpdateResponse("老街区", "你离开了乐器行，回到了老街区的石板路上。");
    }
    else if (message.find("前往乐器店") != std::string::npos || message.find("前往乐器行") != std::string::npos || (message.find("乐器") != std::string::npos && message.find("离开") == std::string::npos)) {
        targetLocation = "乐器行";
        currentLocation = "乐器行";
        availableActions.clear();
        availableActions.push_back("询问乐谱的事情");
        availableActions.push_back("向老人解释对这首乐曲很熟悉");
        availableActions.push_back("欣赏乐曲");
        availableActions.push_back("离开乐器行");
        return generateSceneUpdateResponse("乐器行", "天空中阴雨绵绵，似乎也在为这段感情感伤，街边响起了阵阵悠扬的琴声，你走进一家乐器店，老板是一位白发苍苍的老人，看岁数像是与周奶奶相近，不过却多了几分精神。\\n\\n坐在店面内，思索着委托的事情，不知该从何处下手，听着耳边的旋律，你愈发觉得熟悉，这不就是梧桐雨吗！这首曲子你只在小时候听到苏先生演奏过，此后便没再听到过了。");
    }
    else if (message.find("离开中学") != std::string::npos || message.find("离开育才中学") != std::string::npos) {
        std::cout << "[APIHandler] 离开育才中学，返回老街区" << std::endl;
        targetLocation = "老街区";
        currentLocation = "老街区";
        availableActions.clear();
        availableActions.push_back("前往钟表铺");
        availableActions.push_back("前往乐器行");
        availableActions.push_back("前往育才中学");
        availableActions.push_back("前往和平路32号茶馆");
        availableActions.push_back("回到书店");
        return generateSceneUpdateResponse("老街区", "你离开了育才中学，回到了老街区的石板路上。");
    }
    else if (message.find("前往育才中学") != std::string::npos || (message.find("育才中学") != std::string::npos && message.find("离开") == std::string::npos)) {
        targetLocation = "育才中学";
        currentLocation = "育才中学";
        availableActions.clear();
        availableActions.push_back("向保安大叔说明来意");
        availableActions.push_back("离开中学");
        return generateSceneUpdateResponse("育才中学", "育才中学的大门有些斑驳，充满了年代感。正值下午，能听到校园里传来隐约的读书声和欢笑声。传达室里，一位保安大叔正靠在椅子上，悠闲地听着收音机。");
    }
    else if (message.find("离开茶馆") != std::string::npos) {
        std::cout << "[APIHandler] 离开茶馆，返回老街区" << std::endl;
        targetLocation = "老街区";
        currentLocation = "老街区";
        availableActions.clear();
        availableActions.push_back("前往钟表铺");
        availableActions.push_back("前往乐器行");
        availableActions.push_back("前往育才中学");
        availableActions.push_back("前往和平路32号茶馆");
        availableActions.push_back("回到书店");
        return generateSceneUpdateResponse("老街区", "你离开了茶馆，回到了老街区的石板路上。");
    }
    else if (message.find("前往茶馆") != std::string::npos || (message.find("茶馆") != std::string::npos && message.find("离开") == std::string::npos)) {
        targetLocation = "和平路32号茶馆";
        currentLocation = "和平路32号茶馆";
        availableActions.clear();
        availableActions.push_back("与周奶奶对话");
        availableActions.push_back("与苏晓对话");
        availableActions.push_back("交付时光信物");
        availableActions.push_back("离开茶馆");
        return generateSceneUpdateResponse("和平路32号茶馆", "一间古朴的茶馆，周奶奶正端坐在窗前，苏晓陪着她默默等待着。温暖的灯光洒在她们身上。");
    }
    else if (message.find("书店") != std::string::npos) {
        targetLocation = "时光角落书店";
        currentLocation = "时光角落书店";
        availableActions.clear();
        availableActions.push_back("整理书架");
        availableActions.push_back("查看记忆寻回处");
        availableActions.push_back("前往老街区");
        return generateSceneUpdateResponse("时光角落书店", "推开木门，门楣上的铜铃发出清脆的声响。书店里弥漫着旧纸张和油墨的特殊香气，阳光透过百叶窗在布满灰尘的空气中划出几道光柱。");
    }
    
    return generateGameStateResponse();
}

std::string APIHandler::handleExamineCommand(const std::string& message) {
    std::cout << "[APIHandler] 正在处理检查命令，当前位置: " << currentLocation << "，命令: " << message << std::endl;
    
    // 钟表铺观察 - 使用布尔值防止重复刷属性
    if (currentLocation == "钟表铺") {
        if (message.find("观察店内的摆设") != std::string::npos || message.find("观察店内") != std::string::npos) {
            // 检查是否已经观察过店内摆设
            if (getSceneState("clockshop_observed_items")) {
                updateAvailableActions();
                return generateSceneUpdateResponse(currentLocation, "你已经仔细观察过店内的摆设了，没有发现新的线索。");
            }
            
            // 第一次观察：发现进货单，观察力+1
            increaseAttribute("observation", 1);
            setSceneState("clockshop_observed_items", true);
            
            std::vector<std::pair<std::string, std::string>> choices = {
                {"帮助店主一块收拾", "help_boss"},
                {"继续观察店内", "continue_observe"}
            };
            return generateDialogueResponse("系统", "你注意到柜台角落有个被揉皱的纸团，展开后发现是钟表铺去年的进货单，上面标注着\\\"寄存一块刻'梅'字的怀表\\\"。（观察力+1）\\n\\n店主缓缓起身，准备将店外支起的修表小摊收回屋。", choices);
        } 
        else if (message.find("仔细观察店铺") != std::string::npos) {
            // 检查是否已经仔细观察过店铺
            if (getSceneState("clockshop_detailed_observed")) {
                updateAvailableActions();
                return generateSceneUpdateResponse(currentLocation, "你已经非常仔细地观察过这家店铺了，没有遗漏任何细节。");
            }
            
            // 仔细观察逻辑
            if (checkAttribute("observation", 2)) {
                increaseAttribute("observation", 1);
                setSceneState("clockshop_detailed_observed", true);
                updateAvailableActions();
                return generateSceneUpdateResponse(currentLocation, "你的目光越过满墙的钟表，落在工作台后一张不起眼的老照片上。照片里，陈老板和一个更年长的师傅站在一起，笑容灿烂。这似乎是很重要的回忆。（观察力+1）");
            } else {
                setSceneState("clockshop_detailed_observed", true);
                updateAvailableActions();
                return generateSceneUpdateResponse(currentLocation, "店铺里的一切都显得井井有条，但除了钟表，似乎没有什么特别的东西。");
            }
        } 
        else {
            // 其他观察行为
            if (getSceneState("clockshop_general_observed")) {
                updateAvailableActions();
                return generateSceneUpdateResponse(currentLocation, "你已经观察过这里了。");
            }
            
            increaseAttribute("observation", 1);
            setSceneState("clockshop_general_observed", true);
            updateAvailableActions();
            return generateSceneUpdateResponse(currentLocation, "你仔细观察钟表铺，发现这里的每一件钟表都有着自己的故事。（观察力+1）");
        }
    } else if (currentLocation == "乐器行") {
        increaseAttribute("observation", 1);
        // 确保使用正确的乐器行操作列表
        availableActions.clear();
        availableActions.push_back("询问乐谱的事情");
        availableActions.push_back("向老人解释对这首乐曲很熟悉");
        availableActions.push_back("欣赏乐曲");
        availableActions.push_back("离开乐器行");
        return generateSceneUpdateResponse(currentLocation, "乐器行里摆放着各种乐器，墙上挂着许多老照片。");
    } else if (currentLocation == "育才中学") {
        increaseAttribute("observation", 1);
        // 确保使用正确的育才中学操作列表
        availableActions.clear();
        availableActions.push_back("向保安大叔说明来意");
        availableActions.push_back("离开中学");
        return generateSceneUpdateResponse(currentLocation, "学校的走廊里贴着许多老照片，记录着学校的历史。");
    }
    
    // 默认观察 - 保持在当前场景，确保使用正确的操作列表
    increaseAttribute("observation", 1);
    
    // 根据当前场景设置正确的操作列表
    if (currentLocation == "钟表铺") {
        availableActions.clear();
        availableActions.push_back("询问店主有没有见到过怀表");
        availableActions.push_back("观察店内的摆设");
        availableActions.push_back("仔细观察店铺");
        availableActions.push_back("离开钟表铺");
    } else if (currentLocation == "乐器行") {
        availableActions.clear();
        availableActions.push_back("询问乐谱的事情");
        availableActions.push_back("向老人解释对这首乐曲很熟悉");
        availableActions.push_back("欣赏乐曲");
        availableActions.push_back("离开乐器行");
    } else if (currentLocation == "育才中学") {
        availableActions.clear();
        availableActions.push_back("向保安大叔说明来意");
        availableActions.push_back("离开中学");
    }
    
    return generateSceneUpdateResponse(currentLocation, "你仔细观察周围，但没有发现什么特别的东西。");
}

std::string APIHandler::handleTalkCommand(const std::string& message) {
    std::cout << "[APIHandler] 正在处理对话命令" << std::endl;
    
    // 钟表铺对话 - 按照游戏内容文档完整实现
    if (currentLocation == "钟表铺" && (message.find("询问") != std::string::npos || message.find("怀表") != std::string::npos || message.find("店主") != std::string::npos || message.find("询问店主有没有见到过怀表") != std::string::npos)) {
        // 检查是否已经完成了怀表任务
        if (getSceneState("clockshop_got_watch")) {
            return generateSceneUpdateResponse(currentLocation, "陈老板已经把怀表给你了，没有其他事情了。");
        }
        
        // 检查是否已经询问过
        if (getSceneState("clockshop_asked_about_watch")) {
            return generateSceneUpdateResponse(currentLocation, "你已经询问过店主关于怀表的事情了。");
        }
        
        setSceneState("clockshop_asked_about_watch", true);
        std::vector<std::pair<std::string, std::string>> choices = {
            {"我是受人之托，为一位姓周的奶奶找回它。", "clockshop_explain"}
        };
        return generateDialogueResponse("陈老板", "你想做什么，我们这倒是有各式各样的老怀表，不知道你说的是哪一种", choices);
    }
    // 乐器行对话 - 按照游戏内容文档完整实现
    else if (currentLocation == "乐器行" && (message.find("询问乐谱") != std::string::npos || message.find("乐谱的事情") != std::string::npos)) {
        std::vector<std::pair<std::string, std::string>> choices = {
            {"向老人解释委托的事情", "music_explain_commission"},
            {"向老人解释对这首乐曲很熟悉", "music_explain_familiar"}
        };
        return generateDialogueResponse("李老师", "您好，您是不是正在弹奏《梧桐雨》？\\n\\n老人停下演奏，有些意外的看着你\\\"你为什么知道这首曲子？\\\"", choices);
    }
    else if (currentLocation == "乐器行" && (message.find("向老人解释对这首乐曲很熟悉") != std::string::npos || message.find("music_explain_familiar") != std::string::npos)) {
        increaseAttribute("communication", 1);
        std::vector<std::pair<std::string, std::string>> choices = {
            {"了解更多故事", "learn_story"}
        };
        return generateDialogueResponse("李老师", "\\\"我小时候常听到一位苏先生演奏这首曲子，当时就觉得它很美，不过后来就没再听到过了，很想知道它里面的故事\\\"\\n\\n老人温和的笑了笑\\\"我给你讲讲便是\\\"（沟通力+1）", choices);
    }
    else if (currentLocation == "乐器行" && message.find("欣赏乐曲") != std::string::npos) {
        increaseAttribute("communication", 1);
        std::vector<std::pair<std::string, std::string>> choices = {
            {"了解更多故事", "learn_story"}
        };
        return generateDialogueResponse("李老师", "琴音渐渐消散，\\\"这首曲子很美，里面有什么故事吗？\\\"\\n\\n老人露出了温和的笑容，\\\"年轻人懂得欣赏\\\"（沟通力+1）", choices);
    }
    else if (currentLocation == "育才中学" && (message.find("保安大叔") != std::string::npos || message.find("说明来意") != std::string::npos || message.find("向保安大叔说明来意") != std::string::npos)) {
        // 使用新的属性检查系统
        std::vector<DialogueOption> options = {
            DialogueOption("我在找1975届的集体照。", "school_guard_2"),
            DialogueOption("您对那个年代很了解吗？", "school_guard_communicate", "communication", 3),
            DialogueOption("观察档案室里的撕掉一角的照片", "school_guard_observe", "observation", 3)
        };
        
        auto filteredOptions = filterDialogueOptions(options);
        return generateDialogueResponse("保安大叔", "年轻人，你来学校有什么事？我在这里工作很多年了，对这里的历史还算了解。", filteredOptions);
    }
    else if (currentLocation == "和平路32号茶馆") {
        if (message.find("交付时光信物") != std::string::npos) {
            // 直接处理交付时光信物
            return handleDialogueChoice("{\"optionId\":\"deliver_items\"}");
        }
        else if (message.find("周奶奶") != std::string::npos || message.find("与周奶奶对话") != std::string::npos) {
            std::vector<std::pair<std::string, std::string>> choices = {
                {"交付时光信物", "deliver_items"}
            };
            return generateDialogueResponse("周奶奶", "周奶奶端坐在窗前，模糊的视线中透着期待。", choices);
        }
        else if (message.find("苏晓") != std::string::npos || message.find("与苏晓对话") != std::string::npos) {
            std::vector<std::pair<std::string, std::string>> choices = {
                {"了解更多关于周奶奶的故事", "learn_more"}
            };
            return generateDialogueResponse("苏晓", "苏晓看着你，眼中闪烁着感激的泪光。谢谢你来帮助我们。", choices);
        }
    }
    
    // 如果没有匹配到对话，返回当前场景状态而不是默认状态
    // 确保使用正确的场景操作列表
    if (currentLocation == "育才中学") {
        availableActions.clear();
        availableActions.push_back("向保安大叔说明来意");
        availableActions.push_back("离开中学");
        return generateSceneUpdateResponse(currentLocation, "没有人可以对话。");
    } else if (currentLocation == "钟表铺") {
        availableActions.clear();
        availableActions.push_back("询问店主有没有见到过怀表");
        availableActions.push_back("观察店内的摆设");
        availableActions.push_back("仔细观察店铺");
        availableActions.push_back("离开钟表铺");
        return generateSceneUpdateResponse(currentLocation, "没有人可以对话。");
    } else if (currentLocation == "乐器行") {
        availableActions.clear();
        availableActions.push_back("询问乐谱的事情");
        availableActions.push_back("向老人解释对这首乐曲很熟悉");
        availableActions.push_back("欣赏乐曲");
        availableActions.push_back("离开乐器行");
        return generateSceneUpdateResponse(currentLocation, "没有人可以对话。");
    } else {
        return generateSceneUpdateResponse(currentLocation, "没有人可以对话。");
    }
}

std::string APIHandler::handleDialogueChoice(const std::string& message) {
    std::cout << "[APIHandler] 正在处理对话选择" << std::endl;
    
    // 钟表铺对话选择 - 按照游戏内容文档完整实现
    if (message.find("clockshop_explain") != std::string::npos) {
        // 检查是否已经解释过
        if (getSceneState("clockshop_explained")) {
            return generateSceneUpdateResponse(currentLocation, "你已经向店主解释过来意了。");
        }
        
        setSceneState("clockshop_explained", true);
        // 陈老板听到解释后进入关键剧情
        std::vector<std::pair<std::string, std::string>> choices = {
            {"让老板修好表盘", "fix_watch"},
            {"原封不动拿回", "take_original"}
        };
        return generateDialogueResponse("陈老板", "哦，你是问这块怀表吧。店长回到后屋中，窸窸窣窣翻找了一番，拿着一只老旧的梅花表走了出来，轻声叹了口气说：\\\"这块表是去年周姐留在这的，我也不知道为什么，就说要把这块表留在这，说等到她孙女来这取，我也不好拒绝，后来才知道她得了健忘症，这块表我也一直压在箱底，忘记还给她了，说起来，这块表还是她丈夫在我这买的，当时正好店里有台相机，还拍了下来\\\"...\\n\\n我拿起表盘，仔细端详着，指针已经不在转动了，表面也落下许多灰尘。", choices);
    }
    else if (message.find("help_boss") != std::string::npos) {
        // 检查是否已经帮助过店主
        if (getSceneState("clockshop_helped_boss")) {
            return generateSceneUpdateResponse(currentLocation, "你已经帮助过店主收拾了。");
        }
        
        // 帮助店主收拾 - 按照文档：行动力+1
        increaseAttribute("action", 1);
        setSceneState("clockshop_helped_boss", true);
        updateAvailableActions();
        
        std::vector<std::pair<std::string, std::string>> choices = {
            {"向店主解释来意", "explain_after_help"}
        };
        return generateDialogueResponse("系统", "一阵忙活之后，店主明显热情了不少，你向店主解释了你的来意...（行动力+1）", choices);
    }
    else if (message.find("continue_observe") != std::string::npos && checkAttribute("observation", 2)) {
        // 继续观察（需要观察力>=2）- 按照文档发现相框
        std::vector<std::pair<std::string, std::string>> choices = {
            {"询问相框中的照片", "ask_about_photo"}
        };
        return generateDialogueResponse("系统", "发现一个柜台上摆放着一个相框，框中是一个青年男子手捧着一块梅花状怀表，面容灿烂。", choices);
    }
    else if (message.find("explain_after_help") != std::string::npos || message.find("ask_about_photo") != std::string::npos) {
        // 解释来意后进入关键剧情 - 按照文档内容
        std::vector<std::pair<std::string, std::string>> choices = {
            {"让老板修好表盘", "fix_watch"},
            {"原封不动拿回", "take_original"}
        };
        return generateDialogueResponse("陈老板", "哦，你是问这块怀表吧。店长回到后屋中，窸窸窣窣翻找了一番，拿着一只老旧的梅花表走了出来，轻声叹了口气说：\\\"这块表是去年周姐留在这的，我也不知道为什么，就说要把这块表留在这，说等到她孙女来这取，我也不好拒绝，后来才知道她得了健忘症，这块表我也一直压在箱底，忘记还给她了，说起来，这块表还是她丈夫在我这买的，当时正好店里有台相机，还拍了下来\\\"...\\n\\n我拿起表盘，仔细端详着，指针已经不在转动了，表面也落下许多灰尘。", choices);
    }
    else if (message.find("fix_watch") != std::string::npos) {
        // 检查是否已经获得怀表
        if (getSceneState("clockshop_got_watch")) {
            return generateSceneUpdateResponse(currentLocation, "你已经获得了怀表。");
        }
        
        // 修好表盘路线 - 按照文档：沟通力+1 共情值+2
        increaseAttribute("communication", 1);
        increaseAttribute("empathy", 2);
        addItem("watch_perfect", "刻有'梅'字样的怀表", "修复一新的怀表，指针重新开始走动，刻着'梅'字的表盖闪闪发光", ItemGrade::PERFECT);
        addItem("old_photo", "老照片", "陈老板送的珍贵照片，记录着周奶奶丈夫当年购买怀表时的灿烂笑容", ItemGrade::STORY);
        setSceneState("clockshop_got_watch", true);
        updateAvailableActions();
        return generateSceneUpdateResponse(currentLocation, "\\\"老板，能否请您修复好这块怀表？我想让他回到当初的样子，让记忆流动起来\\\"\\n\\n忙碌了一阵，终于修复完成，店长注意到了架子上的相片，\\\"那张照片你也拿走吧，老人家也去世有些时日了，给周姐也能留个念想\\\"...\\n\\n——获得\\\"刻有'梅'字样的怀表\\\"（沟通力+1 共情值+2）\\n——获得\\\"老照片\\\"");
    }
    else if (message.find("take_original") != std::string::npos) {
        // 检查是否已经获得怀表
        if (getSceneState("clockshop_got_watch")) {
            return generateSceneUpdateResponse(currentLocation, "你已经获得了怀表。");
        }
        
        // 原封不动拿回路线 - 按照文档：共情值+1
        increaseAttribute("empathy", 1);
        addItem("watch_basic", "刻有'梅'字样的怀表", "一块古老的怀表，表盖上刻着'梅'字，保持着岁月的痕迹", ItemGrade::BASIC);
        setSceneState("clockshop_got_watch", true);
        updateAvailableActions();
        return generateSceneUpdateResponse(currentLocation, "\\\"或许保持原样更能体现岁月的痕迹，我直接带回去吧\\\"\\n\\n——获得\\\"刻有'梅'字样的怀表\\\"（共情值+1）");
    }
    
    // 乐器行对话选择 - 按照游戏内容文档重新实现
    else if (message.find("music_explain_commission") != std::string::npos || message.find("explain_commission") != std::string::npos) {
        std::vector<std::pair<std::string, std::string>> choices = {
            {"了解更多故事", "learn_story"}
        };
        return generateDialogueResponse("李老师", "我收到了一份苏家的委托，来寻找《梧桐雨》的手稿，你向老人娓娓道来...\\n\\n老人若有所思的点了点头，向你解释到", choices);
    }
    else if (message.find("music_explain_familiar") != std::string::npos || message.find("explain_familiar") != std::string::npos) {
        increaseAttribute("communication", 1);
        std::vector<std::pair<std::string, std::string>> choices = {
            {"了解更多故事", "learn_story"}
        };
        return generateDialogueResponse("李老师", "我小时候常听到一位苏先生演奏这首曲子，当时就觉得它很美，不过后来就没再听到过了，很想知道它里面的故事。\\n\\n老人温和的笑了笑\\\"我给你讲讲便是\\\"（沟通力+1）", choices);
    }
    else if (message.find("learn_story") != std::string::npos) {
        // 关键剧情 - 李老师自我介绍和故事
        std::vector<std::pair<std::string, std::string>> choices;
        if (checkAttribute("observation", 2)) {
            choices.push_back({"询问未完成的段落", "ask_unfinished"});
        }
        choices.push_back({"说明周奶奶的情况", "explain_situation"});
        
        return generateDialogueResponse("李老师", "老人自我介绍叫李老师，是位退休音乐教师，他缓缓道来：\\n\\n\\\"这首《梧桐雨》是周梅女士的丈夫苏先生创作的。去年周女士来这里，听到我弹奏相似旋律，激动地拿出了原谱。\\\"\\n\\n他指向钢琴方向：\\\"她说希望这首曲子能被更多人听到，就把乐谱留在这里，说这是她丈夫的遗愿。\\\"", choices);
    }
    else if (message.find("ask_unfinished") != std::string::npos) {
        // 观察力>=2的选项
        std::vector<std::pair<std::string, std::string>> choices = {
            {"说明周奶奶的情况", "explain_situation"}
        };
        return generateDialogueResponse("李老师", "你注意到乐谱最后一页有未完成的段落，李先生透露：\"这曲子还差最后一段，但她丈夫没来得及完成\"", choices);
    }
    else if (message.find("explain_situation") != std::string::npos) {
        // 按照文档：先获得基础乐谱，然后询问关系
        increaseAttribute("empathy", 1);
        addItem("score_basic", "《梧桐雨》手写乐谱", "苏先生创作的未完成乐谱，记录着一段美好的爱情", ItemGrade::BASIC);
        
        std::vector<std::pair<std::string, std::string>> choices;
        if (checkAttribute("communication", 2)) {
            choices.push_back({"请李先生录制乐曲", "record_music"});
        }
        choices.push_back({"询问与苏先生的关系", "ask_relationship"});
        
        return generateDialogueResponse("李老师", "\\\"周奶奶得了健忘症，他的孩子特地委托我来找回丢失的乐谱\\\"\\n\\n李老先生略带遗憾的看了看手中的谱子，最终还是长叹了口气，\\\"拿去吧\\\"\\n\\n——获得\\\"《梧桐雨》手写乐谱\\\"（共情值+1）", choices);
    }
    else if (message.find("ask_relationship") != std::string::npos) {
        // 询问与苏先生的关系 - 按照文档完整实现
        std::vector<std::pair<std::string, std::string>> choices;
        if (checkAttribute("communication", 2)) {
            choices.push_back({"请李先生录制乐曲", "record_music"});
        }
        choices.push_back({"表示感谢", "thank_teacher"});
        
        return generateDialogueResponse("李老师", "\\\"您和苏先生是什么关系呢\\\"，我突然想到这个问题。\\n\\n\\\"我和他是同一届音乐学院的学生，说起来，当时还是我怂恿他去追求的周梅，真是岁月催人啊。这间店，也是当时我和他一起开的，后来他走了，就只留了一把琴在这。\\\"\\n\\n李先生走向一把颇有年代的吉他旁，郑重地将它取了下来，轻轻摩梭着琴弦，\\\"对了，那张乐谱我也补齐了，你把它带走吧。\\\"", choices);
    }
    else if (message.find("record_music") != std::string::npos) {
        // 检查是否已经录制过
        if (getSceneState("music_recorded")) {
            return generateSceneUpdateResponse(currentLocation, "李老师已经为你录制了音乐。");
        }
        
        // 高沟通力路线 - 完美结局
        increaseAttribute("communication", 1);
        increaseAttribute("empathy", 2);
        
        // 移除基础乐谱，添加完美乐谱和录音
        inventory.erase(std::remove_if(inventory.begin(), inventory.end(), 
            [](const GameItem& item) { return item.itemId == "score_basic"; }), inventory.end());
        
        addItem("score_perfect", "《梧桐雨》完整乐谱", "李老师补完的完整乐谱，每一个音符都充满了爱与思念", ItemGrade::PERFECT);
        addItem("music_recording", "《梧桐雨》音乐光盘", "李老师亲自演奏录制的音乐光盘，承载着跨越时空的旋律", ItemGrade::STORY);
        
        setSceneState("music_recorded", true);
        updateAvailableActions();
        return generateSceneUpdateResponse(currentLocation, "\\\"李先生，能否请您将这曲子录下来让我带回去，也好让周女士时常听一听\\\"\\n\\n\\\"也好，也好，这可是他平生最得意的曲子了。\\\"...\\n\\n——获得\\\"《梧桐雨》完整乐谱\\\"\\n——获得\\\"《梧桐雨》音乐光盘\\\"（沟通力+1 共情值+2）");
    }
    else if (message.find("take_score_basic") != std::string::npos) {
        // 基础路线
        increaseAttribute("empathy", 1);
        addItem("score_basic", "《梧桐雨》手写乐谱", "一份珍贵的手写乐谱，承载着苏先生对周梅的深情", ItemGrade::BASIC);
        return generateSceneUpdateResponse(currentLocation, "——获得\\\"《梧桐雨》手写乐谱\\\"（共情值+1）");
    }
    // 保留旧的对话选择作为向后兼容
    else if (message.find("ask_about_score") != std::string::npos || message.find("familiar_music") != std::string::npos || message.find("appreciate_music") != std::string::npos) {
        // 重定向到新的对话系统
        return handleDialogueChoice("{\"optionId\":\"music_master_2\"}");
    }
    
    // 学校对话选择 - 按照设计文档重新实现
    else if (message.find("school_guard_2") != std::string::npos) {
        std::vector<DialogueOption> options = {
            DialogueOption("我是受人之托来找的。", "school_guard_3a"),
            DialogueOption("聊起撕掉一角的照片", "school_guard_observe", "observation", 3)
        };
        auto filteredOptions = filterDialogueOptions(options);
        return generateDialogueResponse("保安大叔", "1975届啊...那是我们学校的黄金年代。你说的周梅，我记得她，很优秀的女孩子。", filteredOptions);
    }
    else if (message.find("school_guard_3a") != std::string::npos) {
        std::vector<DialogueOption> options = {
            DialogueOption("谢谢您。", "school_guard_end_basic"),
            DialogueOption("聊起撕掉一角的照片", "school_guard_observe", "observation", 3),
            DialogueOption("帮忙整理档案室", "school_guard_help", "action", 2)
        };
        auto filteredOptions = filterDialogueOptions(options);
        return generateDialogueResponse("保安大叔", "原来如此...档案室里应该有那张集体照，我带你去找找。", filteredOptions);
    }
    else if (message.find("school_guard_communicate") != std::string::npos) {
        increaseAttribute("communication", 1);
        return generateDialogueResponse("保安大叔", "是啊，我当年也是这个学校的学生，和周梅是同班同学。那个年代的人都很纯真，友谊和爱情都很珍贵...", {{"继续", "school_guard_2"}});
    }
    else if (message.find("school_guard_observe") != std::string::npos) {
        // 高属性奖励路线 - 完美版信物 + 故事物品
        increaseAttribute("observation", 1);
        increaseAttribute("communication", 1);
        addItem("photo_perfect", "1975届集体照（完整版）", "一张完整的集体照，记录着青春岁月的美好", ItemGrade::PERFECT);
        addItem("dance_invitation", "舞会的邀请函", "保安大叔珍藏的毕业舞会邀请函，见证了那段青春往事", ItemGrade::STORY);
        return generateDialogueResponse("保安大叔", "你观察得很仔细...这张照片确实被撕掉了一角。那是因为...（陷入回忆）当年毕业舞会的时候，我暗恋着周梅，但她和苏先生在一起了。这张邀请函我一直保存着，现在送给你吧，希望能帮到她。", {{"继续", "END_DIALOGUE"}});
    }
    else if (message.find("school_guard_help") != std::string::npos) {
        // 行动力>=2的奖励路线 - 获得获奖证明
        increaseAttribute("action", 1);
        increaseAttribute("empathy", 1);
        addItem("photo_basic", "1975届集体照", "一张泛黄的集体照，记录着青春岁月", ItemGrade::BASIC);
        addItem("award_certificate", "周奶奶的获奖证明", "周梅的诗歌作品获奖证明，见证了她的才华", ItemGrade::STORY);
        return generateDialogueResponse("保安大叔", "你真是个热心的年轻人！在整理档案的时候，我找到了这张照片，还有这个...\\n\\n\\\"这是当年周梅的诗歌作品获奖了，可惜还没等奖状下来，她便走了，只好把它留在这，等她回来拿。\\\"\\n\\n——获得\\\"1975届集体照\\\"\\n——获得\\\"周奶奶的获奖证明\\\"（行动力+1 共情值+1）", {{"继续", "END_DIALOGUE"}});
    }
    else if (message.find("school_guard_end_basic") != std::string::npos) {
        // 基础奖励路线 - 标准版信物
        increaseAttribute("empathy", 1);
        addItem("photo_basic", "1975届集体照", "一张泛黄的集体照，记录着青春岁月", ItemGrade::BASIC);
        return generateSceneUpdateResponse(currentLocation, "你获得了1975届集体照。保安大叔说：\'青春总是美好的，希望这能帮到你。\'");
    }
    // 保留旧的对话选择作为向后兼容
    else if (message.find("help_find_photo") != std::string::npos || message.find("search_files") != std::string::npos) {
        // 重定向到新的对话系统
        return handleDialogueChoice("{\"optionId\":\"school_guard_2\"}");
    }
    
    // 茶馆对话选择 - 按照设计文档实现多重结局
    else if (message.find("deliver_items") != std::string::npos) {
        // 检查是否有所有三件时光信物
        bool hasWatch = hasItem("watch_basic") || hasItem("watch_perfect");
        bool hasScore = hasItem("score_basic") || hasItem("score_perfect");
        bool hasPhoto = hasItem("photo_basic") || hasItem("photo_perfect");
        
        if (hasWatch && hasScore && hasPhoto) {
            // 根据共情值决定可用的结局选项
            int empathyValue = playerAttributes["empathy"];
            
            // 检查是否有所有故事物品
            bool hasAllStoryItems = hasStoryItem("old_photo") && 
                                   hasStoryItem("music_recording") && 
                                   hasStoryItem("dance_invitation");
            
            std::vector<DialogueOption> options;
            
            if (empathyValue >= 6 && hasAllStoryItems) {
                // 共情值≥6且有故事物品：可选择完美结局
                std::vector<std::pair<std::string, std::string>> choices = {
                    {"简单交付：'您要的东西，我都找齐了。'", "ending_simple"},
                    {"故事拼凑：'我找到的，不只是三件物品，而是一个完整的故事。'", "ending_perfect"}
                };
                return generateDialogueResponse("系统提示", "你已经收集了所有的信物和故事物品，并且深深理解了它们的意义。你要如何向周奶奶讲述这个故事？", choices);
            } else if (empathyValue >= 4) {
                // 共情值4-5：良好结局
                return handleDialogueChoice("{\"optionId\":\"ending_good\"}");
            } else if (empathyValue == 3) {
                // 共情值=3：普通结局
                return handleDialogueChoice("{\"optionId\":\"ending_normal\"}");
            } else {
                // 共情值不足
                return generateSceneUpdateResponse(currentLocation, "你感觉自己对这些信物的理解还不够深入，或许需要更多地了解它们背后的故事和情感。（共情值不足，需要至少3点共情值）");
            }
        } else {
            return generateSceneUpdateResponse(currentLocation, "你还没有收集齐所有的时光信物。你需要找到：刻有梅字的旧怀表、梧桐雨乐谱、集体照。");
        }
    }
    else if (message.find("ending_normal") != std::string::npos) {
        // 结局一：普通结局（共情值=3）
        return generateSceneUpdateResponse(currentLocation, "你将三件信物放在周奶奶面前。她看了看这些物品，眼中闪过一丝困惑，似乎在努力回忆着什么，但很快又陷入了茫然。\\n\\n苏晓轻抚着母亲的手：\\\"谢谢你帮我找到这些，虽然妈妈的记忆还是...\\\"她的声音有些哽咽，\\\"但至少这些东西回来了。\\\"\\n\\n你完成了委托，但感觉自己做得还远远不够。信物稳住了周奶奶的病情，但始终无法找回全部的记忆。苏晓对你表示感谢但难掩失望，你逐渐意识到你做的远远达不到爷爷的标准，有些事情也就慢慢被忘却在记忆之中。\\n\\n【游戏结束 - 普通结局】");
    }
    else if (message.find("ending_good") != std::string::npos) {
        // 结局二：良好结局（共情值=4或5）
        return generateSceneUpdateResponse(currentLocation, "你小心翼翼地将三件信物递给周奶奶，并轻声讲述着每一件物品背后的故事。\\n\\n周奶奶的眼神逐渐变得专注，她轻抚着怀表，聆听着乐谱的旋律，凝视着集体照中年轻的自己。虽然记忆依然模糊，但她的脸上浮现出久违的安详笑容。\\n\\n\\\"谢谢你...\\\"周奶奶轻声说道，这是她很久以来第一次如此清晰地说话。\\n\\n苏晓激动地握住你的手：\\\"你给的信物帮妈妈稳定了部分记忆，我们会经常来书店坐坐，听她讲讲当年发生过的趣事。\\\"\\n\\n此后，书店的生意也越来越好，作为\\\"时光角落\\\"的负责人，你也在这条路上愈走愈远...\\n\\n【游戏结束 - 良好结局】");
    }
    else if (message.find("ending_simple") != std::string::npos) {
        // 结局三：慰藉结局（基础交付）
        return generateSceneUpdateResponse(currentLocation, "你将三件信物——停摆的怀表、残缺的乐谱、泛黄的集体照——轻轻放在周奶奶面前。\\n\\n周奶奶的目光逐一扫过这些物品，浑浊的眼睛里闪过一丝短暂的亮光，嘴角浮现出一抹安详的微笑，仿佛在遥远的回忆里看到了熟悉的影子。\\n\\n苏晓对你深深地鞠了一躬：\\\"谢谢你，真的谢谢你。能让她再次看到这些东西，已经很好了。\\\"你完成了委托，但心中似乎留有一丝淡淡的遗憾。\\n\\n【游戏结束 - 慰藉结局】");
    }
    else if (message.find("ending_perfect") != std::string::npos) {
        // 结局四：重现结局（完美结局）
        return generateSceneUpdateResponse(currentLocation, "你没有立刻拿出信物，而是选择坐下，开始讲述你的发现：关于一个信守承诺的徒弟，一个珍藏挚友乐曲的伙伴，还有一个默默守护着青春秘密的见证者……\\n\\n随着你的讲述，你将修复一新的怀表戴在周奶奶的手腕上，时间的\\\"滴答\\\"声再次响起。你播放了那首完整的《梧桐雨》，悠扬的旋律在茶馆中回荡。\\n\\n最后，你把集体照、舞会邀请函和那张合照的复刻版一并呈现。在音乐和故事的交织中，周奶奶空茫的眼神逐渐变得清晰、湿润。她颤抖地抚摸着怀表，口中清晰地、温柔地念出了一个名字：\\\"阿哲……\\\"\\n\\n一滴眼泪滑过她布满皱纹的脸颊。在那一刻，时光倒流，记忆重现。苏晓已经热泪盈眶，她握住你的手，无言地表达着最深的感激。你不仅找回了信物，更传递了时光的温度。\\n\\n此后的时光里，你一直致力于打理好爷爷留下的书店，周奶奶的病情也逐渐好转，为了感谢你的帮助，苏晓成立了一个\\\"时光记忆\\\"空间，你也成了主理人，越来越多的人愿意来你的书店分享故事，你也愈发感到爷爷为何对这件事念念不忘，决定在这条路上一直走下去...\\n\\n【游戏结束 - 重现结局】");
    }
    
    return generateGameStateResponse();
}

std::string APIHandler::generateGameStateResponse() {
    std::ostringstream json;
    json << "{\n";
    json << "  \"type\": \"gameState\",\n";
    json << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    json << "  \"data\": {\n";
    json << "    \"location\": \"" << currentLocation << "\",\n";
    json << "    \"description\": \"" << getLocationDescription(currentLocation) << "\",\n";
    json << "    \"attributes\": {\n";
    json << "      \"observation\": " << playerAttributes["observation"] << ",\n";
    json << "      \"communication\": " << playerAttributes["communication"] << ",\n";
    json << "      \"action\": " << playerAttributes["action"] << ",\n";
    json << "      \"empathy\": " << playerAttributes["empathy"] << "\n";
    json << "    },\n";
    json << "    \"inventory\": [";
    for (size_t i = 0; i < inventory.size(); ++i) {
        json << "{";
        json << "\"name\":\"" << inventory[i].name << "\",";
        json << "\"description\":\"" << inventory[i].description << "\",";
        json << "\"grade\":" << static_cast<int>(inventory[i].grade);
        json << "}";
        if (i < inventory.size() - 1) json << ",";
    }
    json << "],\n";
    json << "    \"mementoCount\": " << countMementos() << ",\n";
    json << "    \"storyItemCount\": " << countStoryItems() << ",\n";
    json << "    \"actions\": [";
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
        json << "      \"id\": \"" << options[i].second << "\",\n";
        json << "      \"text\": \"" << options[i].first << "\"\n";
        json << "    }";
        if (i < options.size() - 1) json << ",";
    }
    json << "]\n";
    json << "  }\n";
    json << "}";
    
    return json.str();
}

std::string APIHandler::generateSceneUpdateResponse(const std::string& location, const std::string& description) {
    std::cout << "[APIHandler] 生成场景更新响应，位置: " << location << "，可用操作数量: " << availableActions.size() << std::endl;
    for (const auto& action : availableActions) {
        std::cout << "[APIHandler] 可用操作: " << action << std::endl;
    }
    
    std::ostringstream json;
    json << "{\n";
    json << "  \"type\": \"sceneUpdate\",\n";
    json << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    json << "  \"data\": {\n";
    json << "    \"location\": \"" << location << "\",\n";
    json << "    \"description\": \"" << description << "\",\n";
    json << "    \"attributes\": {\n";
    json << "      \"observation\": " << playerAttributes["observation"] << ",\n";
    json << "      \"communication\": " << playerAttributes["communication"] << ",\n";
    json << "      \"action\": " << playerAttributes["action"] << ",\n";
    json << "      \"empathy\": " << playerAttributes["empathy"] << "\n";
    json << "    },\n";
    json << "    \"inventory\": [";
    for (size_t i = 0; i < inventory.size(); ++i) {
        json << "{";
        json << "\"name\":\"" << inventory[i].name << "\",";
        json << "\"description\":\"" << inventory[i].description << "\",";
        json << "\"grade\":" << static_cast<int>(inventory[i].grade);
        json << "}";
        if (i < inventory.size() - 1) json << ",";
    }
    json << "],\n";
    json << "    \"actions\": [";
    for (size_t i = 0; i < availableActions.size(); ++i) {
        json << "\"" << availableActions[i] << "\"";
        if (i < availableActions.size() - 1) json << ",";
    }
    json << "],\n";
    json << "    \"mementoCount\": " << countMementos() << ",\n";
    json << "    \"storyItemCount\": " << countStoryItems() << ",\n";
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

std::string APIHandler::handleSaveGame(const std::string& message) {
    std::cout << "[APIHandler] 正在处理保存游戏" << std::endl;
    
    // 这里可以实现服务器端存档
    // 目前游戏存档主要在前端处理
    
    std::ostringstream json;
    json << "{\n";
    json << "  \"type\": \"saveResult\",\n";
    json << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    json << "  \"success\": true,\n";
    json << "  \"message\": \"游戏已保存\"\n";
    json << "}";
    
    return json.str();
}

std::string APIHandler::handleLoadGame(const std::string& message) {
    std::cout << "[APIHandler] 正在处理读取游戏" << std::endl;
    
    // 解析存档数据（简化版本，实际应该解析JSON）
    if (message.find("saveData") != std::string::npos) {
        std::cout << "[APIHandler] 检测到存档数据，恢复游戏状态" << std::endl;
        
        // 解析位置信息
        if (message.find("老街区") != std::string::npos) {
            currentLocation = "老街区";
        } else if (message.find("钟表铺") != std::string::npos) {
            currentLocation = "钟表铺";
        } else if (message.find("乐器行") != std::string::npos) {
            currentLocation = "乐器行";
        } else if (message.find("育才中学") != std::string::npos) {
            currentLocation = "育才中学";
        } else if (message.find("和平路32号茶馆") != std::string::npos) {
            currentLocation = "和平路32号茶馆";
        } else {
            currentLocation = "时光角落书店";
        }
        
        std::cout << "[APIHandler] 恢复位置到: " << currentLocation << std::endl;
    } else {
        std::cout << "[APIHandler] 未检测到存档数据，重置到初始状态" << std::endl;
    }
    
    // 重置游戏状态到初始状态（清理之前的状态）
    playerAttributes.clear();
    playerAttributes["observation"] = 1;
    playerAttributes["communication"] = 1;
    playerAttributes["action"] = 1;
    playerAttributes["empathy"] = 1;
    
    // 清空物品栏并重新添加初始物品
    inventory.clear();
    addItem("quest_letter", "委托信", "苏晓委托寻找三件信物的信件", ItemGrade::STORY);
    
    // 根据当前位置设置可用操作
    availableActions.clear();
    if (currentLocation == "时光角落书店") {
        availableActions.push_back("整理书架");
        availableActions.push_back("查看记忆寻回处");
        availableActions.push_back("前往老街区");
    } else if (currentLocation == "老街区") {
        availableActions.push_back("前往钟表铺");
        availableActions.push_back("前往乐器店");
        availableActions.push_back("前往育才中学");
        availableActions.push_back("前往茶馆");
        availableActions.push_back("回到书店");
    } else if (currentLocation == "钟表铺") {
        availableActions.push_back("询问店主有没有见到过怀表");
        availableActions.push_back("观察店内的摆设");
        availableActions.push_back("仔细观察店铺");
        availableActions.push_back("离开钟表铺");
    } else if (currentLocation == "乐器行") {
        availableActions.push_back("询问乐谱的事情");
        availableActions.push_back("向老人解释对这首乐曲很熟悉");
        availableActions.push_back("欣赏乐曲");
        availableActions.push_back("离开乐器行");
    } else if (currentLocation == "育才中学") {
        availableActions.push_back("向保安大叔说明来意");
        availableActions.push_back("离开中学");
    } else if (currentLocation == "和平路32号茶馆") {
        availableActions.push_back("交付时光信物");
        availableActions.push_back("离开茶馆");
    }
    
    std::cout << "[APIHandler] 游戏状态已重置，当前位置: " << currentLocation << "，可用操作数量: " << availableActions.size() << std::endl;
    
    // 返回重置后的游戏状态
    return generateGameStateResponse();
}

// 属性检查系统实现
bool APIHandler::checkAttribute(const std::string& attribute, int requiredLevel) {
    auto it = playerAttributes.find(attribute);
    if (it != playerAttributes.end()) {
        return it->second >= requiredLevel;
    }
    return false;
}

void APIHandler::increaseAttribute(const std::string& attribute, int amount) {
    auto it = playerAttributes.find(attribute);
    if (it != playerAttributes.end()) {
        it->second += amount;
        std::cout << "[APIHandler] " << attribute << " 属性增加 " << amount << "，当前值: " << it->second << std::endl;
    }
}

std::vector<std::pair<std::string, std::string>> APIHandler::filterDialogueOptions(
    const std::vector<DialogueOption>& options) {
    
    std::vector<std::pair<std::string, std::string>> filteredOptions;
    
    for (const auto& option : options) {
        // 检查是否满足属性要求
        if (option.requiredAttribute.empty() || 
            checkAttribute(option.requiredAttribute, option.requiredLevel)) {
            
            // 如果有属性要求，在选项文本前添加标记
            std::string displayText = option.text;
            if (!option.requiredAttribute.empty()) {
                displayText = "(" + option.requiredAttribute + "≥" + 
                             std::to_string(option.requiredLevel) + ") " + option.text;
            }
            
            filteredOptions.push_back({displayText, option.optionId});
        }
    }
    
    return filteredOptions;
}

// 对话状态管理实现
void APIHandler::markDialogueCompleted(const std::string& dialogueId) {
    if (std::find(completedDialogues.begin(), completedDialogues.end(), dialogueId) == completedDialogues.end()) {
        completedDialogues.push_back(dialogueId);
        std::cout << "[APIHandler] 标记对话已完成: " << dialogueId << std::endl;
    }
}

bool APIHandler::isDialogueCompleted(const std::string& dialogueId) {
    return std::find(completedDialogues.begin(), completedDialogues.end(), dialogueId) != completedDialogues.end();
}

// 场景状态管理实现
void APIHandler::setSceneState(const std::string& stateKey, bool value) {
    sceneStates[stateKey] = value;
    std::cout << "[APIHandler] 设置场景状态: " << stateKey << " = " << (value ? "true" : "false") << std::endl;
}

bool APIHandler::getSceneState(const std::string& stateKey) {
    auto it = sceneStates.find(stateKey);
    return (it != sceneStates.end()) ? it->second : false;
}

// 动态生成可用操作（基于对话状态）
void APIHandler::updateAvailableActions() {
    availableActions.clear();
    
    if (currentLocation == "时光角落书店") {
        availableActions.push_back("整理书架");
        availableActions.push_back("查看记忆寻回处");
        availableActions.push_back("前往老街区");
    } else if (currentLocation == "老街区") {
        availableActions.push_back("前往钟表铺");
        availableActions.push_back("前往乐器店");
        availableActions.push_back("前往育才中学");
        availableActions.push_back("前往茶馆");
        availableActions.push_back("回到书店");
    } else if (currentLocation == "钟表铺") {
        // 根据状态动态显示询问选项
        if (!getSceneState("clockshop_got_watch") && !getSceneState("clockshop_asked_about_watch")) {
            availableActions.push_back("询问店主有没有见到过怀表");
        }
        
        // 根据状态动态显示观察选项
        if (!getSceneState("clockshop_observed_items")) {
            availableActions.push_back("观察店内的摆设");
        }
        
        if (!getSceneState("clockshop_detailed_observed")) {
            availableActions.push_back("仔细观察店铺");
        }
        
        availableActions.push_back("离开钟表铺");
    } else if (currentLocation == "乐器行") {
        availableActions.push_back("询问乐谱的事情");
        availableActions.push_back("向老人解释对这首乐曲很熟悉");
        availableActions.push_back("欣赏乐曲");
        availableActions.push_back("离开乐器行");
    } else if (currentLocation == "育才中学") {
        availableActions.push_back("向保安大叔说明来意");
        availableActions.push_back("离开中学");
    } else if (currentLocation == "和平路32号茶馆") {
        availableActions.push_back("交付时光信物");
        availableActions.push_back("离开茶馆");
    }
    
    std::cout << "[APIHandler] 更新可用操作，位置: " << currentLocation << "，操作数量: " << availableActions.size() << std::endl;
}

// 简单的JSON字符串处理
std::string APIHandler::escapeJsonString(const std::string& input) {
    std::string output = input;
    // 只处理最基本的转义
    size_t pos = 0;
    while ((pos = output.find("\"", pos)) != std::string::npos) {
        output.replace(pos, 1, "\\\"");
        pos += 2;
    }
    return output;
}

// 获取位置描述
std::string APIHandler::getLocationDescription(const std::string& location) {
    if (location == "时光角落书店") {
        return "你站在时光角落书店中，这里充满了古老书籍的香味。";
    } else if (location == "老街区") {
        return "狭窄的巷子里，石板路被岁月磨得光滑。两旁是各种老店铺，承载着这个城市的记忆。";
    } else if (location == "钟表铺") {
        return "老街区狭窄的巷子里，\\\"老周钟表铺\\\"的招牌已经歪斜，玻璃窗上蒙着一层薄灰。你缓步走进店内，店主陈老板悠然的躺在太师椅上看着报，似乎并没有注意到你。";
    } else if (location == "乐器行") {
        return "天空中阴雨绵绵，似乎也在为这段感情感伤，街边响起了阵阵悠扬的琴声，你走进一家乐器店，老板是一位白发苍苍的老人，看岁数像是与周奶奶相近，不过却多了几分精神。\\n\\n坐在店面内，思索着委托的事情，不知该从何处下手，听着耳边的旋律，你愈发觉得熟悉，这不就是梧桐雨吗！这首曲子你只在小时候听到苏先生演奏过，此后便没再听到过了。";
    } else if (location == "育才中学") {
        return "育才中学的大门有些斑驳，充满了年代感。正值下午，能听到校园里传来隐约的读书声和欢笑声。传达室里，一位保安大叔正靠在椅子上，悠闲地听着收音机。";
    } else if (location == "和平路32号茶馆") {
        return "一间古朴的茶馆，周奶奶正端坐在窗前，苏晓陪着她默默等待着。温暖的灯光洒在她们身上。";
    } else {
        return "你站在一个未知的地方。";
    }
}

// 物品管理系统实现
void APIHandler::addItem(const std::string& itemId, const std::string& name, const std::string& description, ItemGrade grade) {
    // 检查是否已经拥有该物品
    for (const auto& item : inventory) {
        if (item.itemId == itemId) {
            std::cout << "[APIHandler] 物品 " << itemId << " 已存在，跳过添加" << std::endl;
            return;
        }
    }
    
    inventory.emplace_back(itemId, name, description, grade);
    
    // 如果是故事物品，也添加到故事物品列表
    if (grade == ItemGrade::STORY) {
        storyItems.push_back(itemId);
    }
    
    std::cout << "[APIHandler] 添加物品: " << name << " (等级: " << static_cast<int>(grade) << ")" << std::endl;
}

bool APIHandler::hasItem(const std::string& itemId) {
    for (const auto& item : inventory) {
        if (item.itemId == itemId) {
            return true;
        }
    }
    return false;
}

bool APIHandler::hasStoryItem(const std::string& itemId) {
    for (const auto& item : inventory) {
        if (item.itemId == itemId && item.grade == ItemGrade::STORY) {
            return true;
        }
    }
    return false;
}

int APIHandler::countMementos() {
    int count = 0;
    for (const auto& item : inventory) {
        if (item.grade == ItemGrade::BASIC || item.grade == ItemGrade::PERFECT) {
            if (item.itemId.find("watch") != std::string::npos || 
                item.itemId.find("score") != std::string::npos || 
                item.itemId.find("photo") != std::string::npos) {
                count++;
            }
        }
    }
    return count;
}

int APIHandler::countStoryItems() {
    return static_cast<int>(storyItems.size()) - 1; // 减去委托信
}

// 查看属性命令处理
std::string APIHandler::handleShowAttributesCommand() {
    std::cout << "[APIHandler] 显示属性面板" << std::endl;
    
    std::ostringstream attributeText;
    attributeText << "📊 玩家属性\\n\\n";
    attributeText << "👁️ 观察力: " << playerAttributes["observation"] << "\\n";
    attributeText << "💬 沟通力: " << playerAttributes["communication"] << "\\n";
    attributeText << "⚡ 行动力: " << playerAttributes["action"] << "\\n";
    attributeText << "❤️ 共情值: " << playerAttributes["empathy"] << "\\n\\n";
    attributeText << "属性值越高，解锁的对话选项越多！";
    
    return generateDialogueResponse("属性面板", attributeText.str(), {{"继续", "close_panel"}});
}

// 查看物品命令处理
std::string APIHandler::handleShowInventoryCommand() {
    std::cout << "[APIHandler] 显示物品栏" << std::endl;
    
    std::ostringstream inventoryText;
    inventoryText << "🎒 物品栏\\n\\n";
    
    if (inventory.empty()) {
        inventoryText << "物品栏是空的。";
    } else {
        for (size_t i = 0; i < inventory.size(); ++i) {
            const auto& item = inventory[i];
            std::string gradeIcon = "";
            switch (item.grade) {
                case ItemGrade::PERFECT: gradeIcon = "✨"; break;
                case ItemGrade::STORY: gradeIcon = "📖"; break;
                default: gradeIcon = ""; break;
            }
            
            inventoryText << "📦 " << item.name << " " << gradeIcon << "\\n";
            inventoryText << "   " << item.description << "\\n";
            if (i < inventory.size() - 1) inventoryText << "\\n";
        }
    }
    
    inventoryText << "\\n\\n信物收集进度: " << countMementos() << "/3";
    inventoryText << "\\n故事物品: " << countStoryItems() << "/3";
    
    return generateDialogueResponse("物品栏", inventoryText.str(), {{"继续", "close_panel"}});
}