#include "HttpServer.h"
#include "../../include/core/GameEngine.h"
#include "APIHandler.h"
#include <iostream>
#include <sstream>
#include <algorithm>

HttpServer::HttpServer(GameEngine* gameEngine) 
    : serverSocket_(INVALID_SOCKET)
    , running_(false)
    , gameEngine_(gameEngine)
    , port_(8081)
    , apiHandler_(nullptr) {
    
#ifdef _WIN32
    initializeWinsock();
#endif
    // 创建持久的APIHandler实例
    apiHandler_ = new APIHandler();
    std::cout << "[HttpServer] HTTP服务器已创建" << std::endl;
}

HttpServer::~HttpServer() {
    stop();
    // 清理APIHandler实例
    if (apiHandler_) {
        delete apiHandler_;
        apiHandler_ = nullptr;
    }
#ifdef _WIN32
    cleanupWinsock();
#endif
    std::cout << "[HttpServer] HTTP服务器已销毁" << std::endl;
}

bool HttpServer::start(int port) {
    if (running_.load()) {
        std::cout << "[HttpServer] 服务器已在运行" << std::endl;
        return true;
    }
    
    port_ = port;
    
    // 创建socket
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ == INVALID_SOCKET) {
        std::cerr << "[HttpServer] 创建socket失败" << std::endl;
        return false;
    }
    
    // 设置socket选项
    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
#else
    setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
    
    // 绑定地址
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port_);
    
    if (bind(serverSocket_, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[HttpServer] 绑定端口失败: " << port_ << std::endl;
        closesocket(serverSocket_);
        return false;
    }
    
    // 开始监听
    if (listen(serverSocket_, 5) == SOCKET_ERROR) {
        std::cerr << "[HttpServer] 监听失败" << std::endl;
        closesocket(serverSocket_);
        return false;
    }
    
    running_.store(true);
    serverThread_ = std::thread(&HttpServer::serverLoop, this);
    
    std::cout << "[HttpServer] HTTP服务器启动成功，端口: " << port_ << std::endl;
    return true;
}

void HttpServer::stop() {
    if (!running_.load()) {
        return;
    }
    
    running_.store(false);
    
    if (serverSocket_ != INVALID_SOCKET) {
        closesocket(serverSocket_);
        serverSocket_ = INVALID_SOCKET;
    }
    
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
    
    std::cout << "[HttpServer] HTTP服务器已停止" << std::endl;
}

void HttpServer::setGameEngine(GameEngine* engine) {
    gameEngine_ = engine;
    std::cout << "[HttpServer] 游戏引擎引用已设置" << std::endl;
}

bool HttpServer::isRunning() const {
    return running_.load();
}

void HttpServer::serverLoop() {
    std::cout << "[HttpServer] 开始接受连接..." << std::endl;
    
    while (running_.load()) {
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        
        SOCKET clientSocket = accept(serverSocket_, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            if (running_.load()) {
                std::cerr << "[HttpServer] 接受连接失败" << std::endl;
            }
            continue;
        }
        
        std::cout << "[HttpServer] 新客户端连接: " << clientSocket << std::endl;
        
        // 在新线程中处理客户端
        std::thread clientThread(&HttpServer::handleClient, this, clientSocket);
        clientThread.detach();
    }
}

void HttpServer::handleClient(SOCKET clientSocket) {
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesReceived <= 0) {
        closesocket(clientSocket);
        return;
    }
    
    buffer[bytesReceived] = '\0';
    std::string request(buffer);
    
    std::cout << "[HttpServer] 收到请求: " << request.substr(0, 100) << "..." << std::endl;
    
    std::string response = handleHttpRequest(request);
    
    send(clientSocket, response.c_str(), static_cast<int>(response.length()), 0);
    closesocket(clientSocket);
    
    std::cout << "[HttpServer] 请求处理完成" << std::endl;
}

std::string HttpServer::handleHttpRequest(const std::string& request) {
    std::string method, path, body;
    
    if (!parseHttpRequest(request, method, path, body)) {
        return generateHttpResponse("{\"error\":\"Invalid request\"}", "application/json");
    }
    
    // 处理CORS预检请求
    if (method == "OPTIONS") {
        return generateHttpResponse("", "text/plain");
    }
    
    // 处理API请求
    if (path.find("/api/") == 0) {
        return handleApiRequest(method, path, body);
    }
    
    // 默认响应
    return generateHttpResponse("{\"message\":\"Hello from TimeArtifacts API\"}", "application/json");
}

std::string HttpServer::handleApiRequest(const std::string& method, const std::string& path, const std::string& body) {
    std::cout << "[HttpServer] 处理API请求: " << method << " " << path << std::endl;
    
    if (!gameEngine_) {
        return generateHttpResponse("{\"error\":\"Game engine not available\"}", "application/json");
    }
    
    // 处理游戏状态请求
    if (path == "/api/gamestate" && method == "GET") {
        if (apiHandler_) {
            std::lock_guard<std::mutex> lock(apiHandlerMutex_);
            std::string response = apiHandler_->generateGameStateResponse();
            return generateHttpResponse(response, "application/json");
        }
        return generateHttpResponse("{\"error\":\"APIHandler not initialized\"}", "application/json");
    }
    
    // 处理游戏命令
    if (path == "/api/command" && method == "POST") {
        if (apiHandler_) {
            std::lock_guard<std::mutex> lock(apiHandlerMutex_);
            std::string response = apiHandler_->handleMessage(body);
            return generateHttpResponse(response, "application/json");
        }
        return generateHttpResponse("{\"error\":\"APIHandler not initialized\"}", "application/json");
    }
    
    return generateHttpResponse("{\"error\":\"API endpoint not found\"}", "application/json");
}

std::string HttpServer::generateHttpResponse(const std::string& body, const std::string& contentType) {
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    response += "Access-Control-Allow-Headers: Content-Type\r\n";
    response += "Content-Type: application/json\r\n";
    response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
    response += "\r\n";
    response += body;
    
    return response;
}

bool HttpServer::parseHttpRequest(const std::string& request, std::string& method, std::string& path, std::string& body) {
    std::istringstream stream(request);
    std::string line;
    
    // 解析第一行 (方法和路径)
    if (!std::getline(stream, line)) {
        return false;
    }
    
    std::istringstream firstLine(line);
    if (!(firstLine >> method >> path)) {
        return false;
    }
    
    // 跳过头部
    bool foundEmptyLine = false;
    while (std::getline(stream, line)) {
        if (line == "\r" || line.empty()) {
            foundEmptyLine = true;
            break;
        }
    }
    
    // 读取请求体
    if (foundEmptyLine) {
        std::ostringstream bodyStream;
        while (std::getline(stream, line)) {
            bodyStream << line << "\n";
        }
        body = bodyStream.str();
        if (!body.empty() && body.back() == '\n') {
            body.pop_back();
        }
    }
    
    return true;
}

#ifdef _WIN32
void HttpServer::initializeWinsock() {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData_) != 0) {
        std::cerr << "[HttpServer] Winsock初始化失败" << std::endl;
    }
}

void HttpServer::cleanupWinsock() {
    WSACleanup();
}
#else
void HttpServer::initializeWinsock() {}
void HttpServer::cleanupWinsock() {}
#endif
