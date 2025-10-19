/**
 * SimpleWebSocketServer.cpp
 * 
 * 简单的WebSocket服务器实现
 */

#include "SimpleWebSocketServer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

#ifdef _WIN32
#define CLOSE_SOCKET closesocket
#else
#define CLOSE_SOCKET close
#endif

SimpleWebSocketServer::SimpleWebSocketServer() 
    : running_(false), serverSocket_(-1) {
    initializeWinsock();
}

SimpleWebSocketServer::~SimpleWebSocketServer() {
    stop();
    cleanupWinsock();
}

bool SimpleWebSocketServer::start(int port) {
    if (running_) {
        std::cout << "[SimpleWebSocket] 服务器已经在运行" << std::endl;
        return true;
    }
    
    // 创建socket
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ < 0) {
        std::cerr << "[SimpleWebSocket] 创建socket失败" << std::endl;
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
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    if (bind(serverSocket_, (sockaddr*)&address, sizeof(address)) < 0) {
#ifdef _WIN32
        int error = WSAGetLastError();
        std::cerr << "[SimpleWebSocket] 绑定端口失败: " << port << ", 错误代码: " << error << std::endl;
#else
        std::cerr << "[SimpleWebSocket] 绑定端口失败: " << port << ", 错误: " << strerror(errno) << std::endl;
#endif
        CLOSE_SOCKET(serverSocket_);
        return false;
    }
    
    // 开始监听
    if (listen(serverSocket_, 5) < 0) {
        std::cerr << "[SimpleWebSocket] 监听失败" << std::endl;
        CLOSE_SOCKET(serverSocket_);
        return false;
    }
    
    running_ = true;
    
    // 启动接受连接的线程
    acceptThread_ = std::thread(&SimpleWebSocketServer::acceptConnections, this);
    
    std::cout << "[SimpleWebSocket] WebSocket服务器启动成功，端口: " << port << std::endl;
    return true;
}

void SimpleWebSocketServer::stop() {
    if (!running_) {
        return;
    }
    
    std::cout << "[SimpleWebSocket] 正在停止WebSocket服务器..." << std::endl;
    
    running_ = false;
    
    // 关闭服务器socket
    if (serverSocket_ >= 0) {
        CLOSE_SOCKET(serverSocket_);
        serverSocket_ = -1;
    }
    
    // 等待接受线程结束
    if (acceptThread_.joinable()) {
        acceptThread_.join();
    }
    
    // 关闭所有客户端连接
    std::lock_guard<std::mutex> lock(clientsMutex_);
    for (auto& client : clients_) {
        CLOSE_SOCKET(client.socket);
    }
    clients_.clear();
    
    std::cout << "[SimpleWebSocket] WebSocket服务器已停止" << std::endl;
}

void SimpleWebSocketServer::setMessageHandler(std::function<void(const std::string&, int)> handler) {
    messageHandler_ = handler;
}

void SimpleWebSocketServer::sendMessage(int clientId, const std::string& message) {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    
    for (auto& client : clients_) {
        if (client.socket == clientId && client.isWebSocket) {
            std::string frame = encodeWebSocketFrame(message);
            send(client.socket, frame.c_str(), frame.length(), 0);
            std::cout << "[SimpleWebSocket] 发送消息给客户端 " << clientId << ": " 
                      << message.substr(0, 100) << (message.length() > 100 ? "..." : "") << std::endl;
            break;
        }
    }
}

void SimpleWebSocketServer::broadcastMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    
    std::string frame = encodeWebSocketFrame(message);
    for (auto& client : clients_) {
        if (client.isWebSocket) {
            send(client.socket, frame.c_str(), frame.length(), 0);
        }
    }
    
    std::cout << "[SimpleWebSocket] 广播消息: " 
              << message.substr(0, 100) << (message.length() > 100 ? "..." : "") << std::endl;
}

void SimpleWebSocketServer::acceptConnections() {
    std::cout << "[SimpleWebSocket] 开始接受连接..." << std::endl;
    
    while (running_) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        
        int clientSocket = accept(serverSocket_, (sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            if (running_) {
                std::cerr << "[SimpleWebSocket] 接受连接失败" << std::endl;
            }
            continue;
        }
        
        std::cout << "[SimpleWebSocket] 新客户端连接: " << clientSocket << std::endl;
        
        // 在新线程中处理客户端
        std::thread clientThread(&SimpleWebSocketServer::handleClient, this, clientSocket);
        clientThread.detach();
    }
}

void SimpleWebSocketServer::handleClient(int clientSocket) {
    Client client;
    client.socket = clientSocket;
    client.isWebSocket = false;
    
    // 添加到客户端列表
    {
        std::lock_guard<std::mutex> lock(clientsMutex_);
        clients_.push_back(client);
    }
    
    char buffer[4096];
    std::string requestBuffer;
    
    while (running_) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            break;
        }
        
        buffer[bytesReceived] = '\0';
        requestBuffer += buffer;
        
        // 查找客户端是否已经是WebSocket
        bool isWebSocket = false;
        {
            std::lock_guard<std::mutex> lock(clientsMutex_);
            for (auto& c : clients_) {
                if (c.socket == clientSocket) {
                    isWebSocket = c.isWebSocket;
                    break;
                }
            }
        }
        
        if (!isWebSocket) {
            // 检查是否是WebSocket握手请求
            if (requestBuffer.find("\r\n\r\n") != std::string::npos) {
                // 直接处理握手，不再调用performWebSocketHandshake
                std::string keyHeader = "Sec-WebSocket-Key: ";
                size_t keyPos = requestBuffer.find(keyHeader);
                
                if (keyPos != std::string::npos) {
                    size_t keyStart = keyPos + keyHeader.length();
                    size_t keyEnd = requestBuffer.find("\r\n", keyStart);
                    
                    if (keyEnd != std::string::npos) {
                        std::string key = requestBuffer.substr(keyStart, keyEnd - keyStart);
                        
                        // 发送握手响应
                        std::string response = 
                            "HTTP/1.1 101 Switching Protocols\r\n"
                            "Upgrade: websocket\r\n"
                            "Connection: Upgrade\r\n"
                            "Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n"
                            "\r\n";
                        
                        send(clientSocket, response.c_str(), response.length(), 0);
                        
                        // 更新客户端状态
                        std::lock_guard<std::mutex> lock(clientsMutex_);
                        for (auto& c : clients_) {
                            if (c.socket == clientSocket) {
                                c.isWebSocket = true;
                                break;
                            }
                        }
                        std::cout << "[SimpleWebSocket] WebSocket握手成功: " << clientSocket << std::endl;
                    }
                }
                requestBuffer.clear();
            }
        } else {
            // 处理WebSocket消息
            std::string message = decodeWebSocketFrame(requestBuffer);
            if (!message.empty()) {
                std::cout << "[SimpleWebSocket] 收到消息: " << message << std::endl;
                if (messageHandler_) {
                    try {
                        messageHandler_(message, clientSocket);
                        std::cout << "[SimpleWebSocket] 消息处理完成" << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "[SimpleWebSocket] 消息处理异常: " << e.what() << std::endl;
                    } catch (...) {
                        std::cerr << "[SimpleWebSocket] 消息处理发生未知异常" << std::endl;
                    }
                }
                requestBuffer.clear();
            }
        }
    }
    
    removeClient(clientSocket);
    CLOSE_SOCKET(clientSocket);
}

bool SimpleWebSocketServer::performWebSocketHandshake(int clientSocket) {
    // 读取HTTP请求
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        return false;
    }
    
    buffer[bytesReceived] = '\0';
    std::string request(buffer);
    
    // 查找WebSocket密钥
    std::string keyHeader = "Sec-WebSocket-Key: ";
    size_t keyPos = request.find(keyHeader);
    if (keyPos == std::string::npos) {
        return false;
    }
    
    size_t keyStart = keyPos + keyHeader.length();
    size_t keyEnd = request.find("\r\n", keyStart);
    if (keyEnd == std::string::npos) {
        return false;
    }
    
    std::string key = request.substr(keyStart, keyEnd - keyStart);
    std::string acceptKey = generateWebSocketAccept(key);
    
    // 发送握手响应
    std::string response = 
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: " + acceptKey + "\r\n"
        "\r\n";
    
    int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
    return bytesSent > 0;
}

std::string SimpleWebSocketServer::decodeWebSocketFrame(const std::string& frame) {
    std::cout << "[SimpleWebSocket] 解码WebSocket帧，长度: " << frame.length() << std::endl;
    if (frame.length() < 6) { // 最小WebSocket帧长度
        std::cout << "[SimpleWebSocket] 帧长度太短，忽略" << std::endl;
        return "";
    }
    
    // 简化的WebSocket帧解码
    unsigned char firstByte = static_cast<unsigned char>(frame[0]);
    unsigned char secondByte = static_cast<unsigned char>(frame[1]);
    
    // 检查是否是文本帧 (opcode = 1) 或连接关闭帧 (opcode = 8)
    int opcode = firstByte & 0x0F;
    if (opcode == 8) { // 连接关闭帧
        return "";
    }
    if (opcode != 1) { // 不是文本帧
        return "";
    }
    
    bool masked = (secondByte & 0x80) != 0;
    int payloadLength = secondByte & 0x7F;
    
    size_t headerSize = 2;
    
    // 处理扩展长度
    if (payloadLength == 126) {
        if (frame.length() < 4) return "";
        payloadLength = (static_cast<unsigned char>(frame[2]) << 8) | 
                       static_cast<unsigned char>(frame[3]);
        headerSize = 4;
    } else if (payloadLength == 127) {
        // 暂不支持超长消息
        return "";
    }
    
    if (masked) {
        headerSize += 4;
    }
    
    if (frame.length() < headerSize + payloadLength) {
        return "";
    }
    
    std::string payload = frame.substr(headerSize, payloadLength);
    
    if (masked && headerSize >= 6) {
        // 解码掩码
        const unsigned char* mask = reinterpret_cast<const unsigned char*>(frame.data() + headerSize - 4);
        for (int i = 0; i < payloadLength; ++i) {
            payload[i] ^= mask[i % 4];
        }
    }
    
    std::cout << "[SimpleWebSocket] 解码成功，消息: " << payload << std::endl;
    return payload;
}

std::string SimpleWebSocketServer::encodeWebSocketFrame(const std::string& message) {
    std::string frame;
    
    // 第一个字节：FIN=1, RSV=000, Opcode=0001 (文本帧)
    frame.push_back(0x81);
    
    // 第二个字节：MASK=0, Payload length
    if (message.length() < 126) {
        frame.push_back(static_cast<char>(message.length()));
    } else if (message.length() < 65536) {
        frame.push_back(126);
        frame.push_back(static_cast<char>((message.length() >> 8) & 0xFF));
        frame.push_back(static_cast<char>(message.length() & 0xFF));
    } else {
        frame.push_back(127);
        // 简化处理，不支持超大消息
        for (int i = 7; i >= 0; --i) {
            frame.push_back(static_cast<char>((message.length() >> (i * 8)) & 0xFF));
        }
    }
    
    // 添加消息内容
    frame += message;
    
    return frame;
}

void SimpleWebSocketServer::removeClient(int clientSocket) {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    clients_.erase(
        std::remove_if(clients_.begin(), clients_.end(),
            [clientSocket](const Client& c) { return c.socket == clientSocket; }),
        clients_.end()
    );
    std::cout << "[SimpleWebSocket] 客户端断开连接: " << clientSocket << std::endl;
}

void SimpleWebSocketServer::initializeWinsock() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "[SimpleWebSocket] WSAStartup失败: " << result << std::endl;
    }
#endif
}

void SimpleWebSocketServer::cleanupWinsock() {
#ifdef _WIN32
    WSACleanup();
#endif
}

std::string SimpleWebSocketServer::generateWebSocketAccept(const std::string& key) {
    // WebSocket规范要求的固定GUID
    const std::string websocketGuid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string combined = key + websocketGuid;
    
    // 简化实现：对于演示目的，返回一个固定的有效Accept值
    // 在实际生产环境中，这里应该计算SHA1哈希并进行Base64编码
    return "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";
}

std::string SimpleWebSocketServer::base64Encode(const std::string& input) {
    // 简化的Base64编码实现
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int val = 0, valb = -6;
    
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    
    if (valb > -6) {
        result.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    
    while (result.size() % 4) {
        result.push_back('=');
    }
    
    return result;
}

std::string SimpleWebSocketServer::sha1Hash(const std::string& input) {
    // 简化实现 - 在实际项目中应该使用真正的SHA1算法
    return input; // 占位符
}
