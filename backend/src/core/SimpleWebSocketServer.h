/**
 * SimpleWebSocketServer.h
 * 
 * 简单的WebSocket服务器实现
 * 使用原生socket和HTTP升级协议
 */

#pragma once

#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <functional>
#include <atomic>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

class SimpleWebSocketServer {
public:
    SimpleWebSocketServer();
    ~SimpleWebSocketServer();
    
    bool start(int port = 8080);
    void stop();
    
    void setMessageHandler(std::function<void(const std::string&, int)> handler);
    void sendMessage(int clientId, const std::string& message);
    void broadcastMessage(const std::string& message);
    
    bool isRunning() const { return running_; }

private:
    struct Client {
        int socket;
        bool isWebSocket;
        std::string buffer;
    };
    
    std::atomic<bool> running_;
    int serverSocket_;
    std::thread acceptThread_;
    std::vector<Client> clients_;
    std::mutex clientsMutex_;
    std::function<void(const std::string&, int)> messageHandler_;
    
    void acceptConnections();
    void handleClient(int clientSocket);
    bool performWebSocketHandshake(int clientSocket);
    std::string decodeWebSocketFrame(const std::string& frame);
    std::string encodeWebSocketFrame(const std::string& message);
    std::string generateWebSocketAccept(const std::string& key);
    std::string base64Encode(const std::string& input);
    std::string sha1Hash(const std::string& input);
    
    void removeClient(int clientSocket);
    void initializeWinsock();
    void cleanupWinsock();
};
