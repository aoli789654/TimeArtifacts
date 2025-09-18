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

int main() {
    try {
        std::cout << "=== 时光信物游戏服务器 ===" << std::endl;
        std::cout << "版本: 1.0.0" << std::endl;
        std::cout << "启动中..." << std::endl;
        
        // 创建游戏引擎实例
        auto gameEngine = std::make_unique<GameEngine>();
        
        // 初始化游戏引擎
        if (!gameEngine->initialize()) {
            std::cerr << "错误: 游戏引擎初始化失败" << std::endl;
            return -1;
        }
        
        std::cout << "游戏引擎初始化成功" << std::endl;
        std::cout << "WebSocket服务器已启动，等待客户端连接..." << std::endl;
        std::cout << "按 Ctrl+C 退出程序" << std::endl;
        
        // 运行游戏主循环
        gameEngine->run();
        
        // 清理资源
        gameEngine->shutdown();
        
        std::cout << "游戏服务器已安全关闭" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "致命错误: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "发生未知错误" << std::endl;
        return -1;
    }
}
