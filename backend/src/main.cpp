/**
 * 时光信物 - 主程序入口
 * 
 * 这是游戏的启动点，负责初始化所有核心系统
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
        
        // 创建游戏引擎实例
        auto gameEngine = std::make_unique<GameEngine>();
        
        // 初始化游戏引擎
        if (!gameEngine->initialize()) {
            std::cerr << "ERROR: Game engine initialization failed" << std::endl;
            return -1;
        }
        
        std::cout << "Game engine initialized successfully" << std::endl;
        std::cout << "WebSocket server started, waiting for client connections..." << std::endl;
        std::cout << "Press Ctrl+C to exit" << std::endl;
        
        // 运行游戏主循环
        gameEngine->run();
        
        // 清理资源
        gameEngine->shutdown();
        
        std::cout << "Game server shutdown safely" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return -1;
    }
}
