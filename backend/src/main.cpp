/**
 * 时光信物 - 主程序入口
 * 
 * 这是游戏的启动点，负责初始化所有核心系统
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
    // 设置控制台为UTF-8编码 - 增强版
    SetConsoleOutputCP(65001);  // UTF-8 code page
    SetConsoleCP(65001);        // UTF-8 code page
    
    // 设置控制台模式支持ANSI转义序列（用于更好的UTF-8支持）
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
    
    // 尝试设置locale为中文UTF-8
    try {
        std::locale::global(std::locale("zh_CN.UTF-8"));
    } catch (...) {
        // 如果失败，使用系统默认locale
        std::locale::global(std::locale(""));
    }
#endif
    try {
        // UTF-8测试输出
        std::cout << "UTF-8 encoding test: 中文测试 中文字符" << std::endl;
        std::cout << "=== 时光信物游戏服务器 ===" << std::endl;
        std::cout << "版本: 1.0.0" << std::endl;
        std::cout << "正在启动..." << std::endl;
        
        // 创建游戏引擎实例
        auto gameEngine = std::make_unique<GameEngine>();
        
        // 初始化游戏引擎
        if (!gameEngine->initialize()) {
            std::cerr << "错误: 游戏引擎初始化失败" << std::endl;
            return -1;
        }
        
        std::cout << "游戏引擎初始化成功" << std::endl;
        std::cout << "WebSocket服务器已启动，正在等待客户端连接..." << std::endl;
        std::cout << "按 Ctrl+C 退出" << std::endl;
        
        // 运行游戏主循环
        gameEngine->run();
        
        // 清理资源
        gameEngine->shutdown();
        
        std::cout << "游戏服务器安全关闭" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "致命错误: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "发生未知错误" << std::endl;
        return -1;
    }
}
