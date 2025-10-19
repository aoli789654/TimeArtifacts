#pragma once

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

class GameEngine;
class APIHandler;

/**
 * 简单的HTTP服务器
 * 提供REST API接口，替代WebSocket
 */
class HttpServer {
public:
    HttpServer(GameEngine* gameEngine = nullptr);
    ~HttpServer();
    
    // 启动HTTP服务器
    bool start(int port = 8080);
    
    // 停止HTTP服务器
    void stop();
    
    // 设置游戏引擎引用
    void setGameEngine(GameEngine* engine);
    
    // 检查服务器是否运行
    bool isRunning() const;

private:
    // 服务器主循环
    void serverLoop();
    
    // 处理客户端请求
    void handleClient(SOCKET clientSocket);
    
    // 处理HTTP请求
    std::string handleHttpRequest(const std::string& request);
    
    // 处理API请求
    std::string handleApiRequest(const std::string& method, const std::string& path, const std::string& body);
    
    // 生成HTTP响应
    std::string generateHttpResponse(const std::string& body, const std::string& contentType = "application/json");
    
    // 解析HTTP请求
    bool parseHttpRequest(const std::string& request, std::string& method, std::string& path, std::string& body);
    
    // 初始化Winsock (Windows)
    void initializeWinsock();
    
    // 清理Winsock (Windows)
    void cleanupWinsock();

private:
    SOCKET serverSocket_;
    std::atomic<bool> running_;
    std::thread serverThread_;
    GameEngine* gameEngine_;
    int port_;
    APIHandler* apiHandler_;  // 持久的APIHandler实例
    std::mutex apiHandlerMutex_;  // APIHandler访问保护
    
#ifdef _WIN32
    WSADATA wsaData_;
#endif
};
