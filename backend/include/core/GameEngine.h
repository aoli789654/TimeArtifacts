/**
 * 时光信物 - 游戏引擎
 * 
 * 游戏的核心控制器
 */

#pragma once

#include <memory>
#include <atomic>

// 前向声明
class WebSocketServer;
class HttpServer;
class DataLoader;
class GameState;

class GameEngine {
private:
    std::unique_ptr<WebSocketServer> webSocketServer;
    std::unique_ptr<HttpServer> httpServer;
    std::unique_ptr<DataLoader> dataLoader;
    std::unique_ptr<GameState> currentGameState;
    std::atomic<bool> initialized;
    std::atomic<bool> running;
    float targetFrameTime;

public:
    GameEngine();
    ~GameEngine();
    
    // 禁用拷贝
    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;
    
    bool initialize();
    void run();
    void requestShutdown();
    void shutdown();
    
    bool isInitialized() const { return initialized.load(); }
    bool isRunning() const { return running.load(); }
    
    WebSocketServer* getWebSocketServer() const;
    HttpServer* getHttpServer() const;
    DataLoader* getDataLoader() const;
    GameState* getCurrentGameState() const;
    void setTargetFPS(int fps);
    float getCurrentFPS() const;

private:
    bool initializeSubsystems();
    void update(float deltaTime);
    void cleanupSubsystems();
};