/**
 * 时光信物 - 游戏服务器
 * 
 * 一个温馨的文字冒险游戏
 * 帮助周奶奶找回珍贵的时光信物
 */

#include <iostream>
#include <memory>
#include <exception>

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
<<<<<<< Updated upstream
    // 设置控制台输出为UTF-8编码
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // 设置C++流的编码
    std::locale::global(std::locale(""));
#endif
    try {
        std::cout << "=== Time Artifacts Game Server ===" << std::endl;
        std::cout << "Version: 1.0.0" << std::endl;
        std::cout << "Starting up..." << std::endl;
=======
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
>>>>>>> Stashed changes
        
        // 创建并启动游戏引擎
        auto gameEngine = std::make_unique<GameEngine>();
        
        if (!gameEngine->initialize()) {
            std::cerr << "ERROR: Game engine initialization failed" << std::endl;
            return -1;
        }
        
<<<<<<< Updated upstream
        std::cout << "Game engine initialized successfully" << std::endl;
        std::cout << "WebSocket server started, waiting for client connections..." << std::endl;
        std::cout << "Press Ctrl+C to exit" << std::endl;
=======
        std::cout << "游戏服务器启动成功！" << std::endl;
        std::cout << "等待玩家连接..." << std::endl;
        std::cout << "按 Ctrl+C 退出" << std::endl;
>>>>>>> Stashed changes
        
        // 开始游戏服务
        gameEngine->run();
        
        // 安全关闭
        gameEngine->shutdown();
        
<<<<<<< Updated upstream
        std::cout << "Game server shutdown safely" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
=======
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
>>>>>>> Stashed changes
        return -1;
    }
}
