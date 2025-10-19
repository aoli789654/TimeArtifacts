/**
 * 时光信物 - 游戏服务器
 * 
 * 一个温馨的文字冒险游戏
 * 帮助周奶奶找回珍贵的时光信物
 */

#include <iostream>
#include <memory>
#include <exception>
#include <locale>

// 核心系统头文件
#include "core/GameEngine.h"

// Windows下设置控制台编码
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

int main() {
#ifdef _WIN32
    // 设置控制台支持中文显示
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    
    // 启用控制台颜色支持
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
    
    // 设置系统语言环境
    try {
        std::locale::global(std::locale("zh_CN.UTF-8"));
    } catch (...) {
        std::locale::global(std::locale(""));
    }
#endif
    try {
        std::cout << "=== 时光信物游戏服务器 ===" << std::endl;
        std::cout << "版本: 1.0.0" << std::endl;
        std::cout << "正在启动..." << std::endl;
        
        // 创建并启动游戏引擎
        auto gameEngine = std::make_unique<GameEngine>();
        
        if (!gameEngine->initialize()) {
            std::cerr << "错误: 游戏引擎初始化失败" << std::endl;
            return -1;
        }
        
        std::cout << "游戏服务器启动成功！" << std::endl;
        std::cout << "等待玩家连接..." << std::endl;
        std::cout << "按 Ctrl+C 退出" << std::endl;
        
        // 开始游戏服务
        gameEngine->run();
        
        // 安全关闭
        gameEngine->shutdown();
        
        std::cout << "游戏服务器安全关闭" << std::endl;
        std::cout << "按任意键退出..." << std::endl;
        std::cin.get();
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "致命错误: " << e.what() << std::endl;
        std::cout << "按任意键退出..." << std::endl;
        std::cin.get();
        return -1;
    } catch (...) {
        std::cerr << "发生未知错误" << std::endl;
        std::cout << "按任意键退出..." << std::endl;
        std::cin.get();
        return -1;
    }
}