# 后端架构师入门指南

欢迎加入时光信物项目！作为后端架构师，您将负责构建游戏的核心逻辑和通信系统。

## 🎯 您的职责

### 第一阶段任务（第1-3周）
1. **建立WebSocket服务器** - 实现前后端实时通信
2. **设计API接口规范** - 定义前后端数据交换格式
3. **实现状态机系统** - 管理游戏的不同状态
4. **创建事件管理器** - 实现模块间解耦通信
5. **建立数据加载系统** - 从JSON文件加载游戏数据

## 🚀 环境搭建

### 1. 安装依赖
```bash
# 安装vcpkg（如果还没有）
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh  # Linux/Mac
./bootstrap-vcpkg.bat  # Windows

# 安装项目依赖
./vcpkg install nlohmann-json websocketpp sfml
```

### 2. 构建项目
```bash
cd backend
mkdir build
cd build

# 配置CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg路径]/scripts/buildsystems/vcpkg.cmake

# 编译
make  # Linux/Mac
# 或者在Windows上
cmake --build . --config Release
```

### 3. 运行测试
```bash
# 运行程序
./TimeArtifacts

# 应该看到类似输出：
# === 时光信物游戏服务器 ===
# 版本: 1.0.0
# 启动中...
# 游戏引擎初始化成功
# WebSocket服务器已启动，等待客户端连接...
```

## 📚 核心技术栈

### 1. WebSocket通信（重点）
```cpp
// 推荐使用 websocketpp 库
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;

class WebSocketServer {
    server wsServer;
    
public:
    void start(uint16_t port = 8080);
    void onMessage(websocketpp::connection_hdl hdl, server::message_ptr msg);
    void sendToClient(websocketpp::connection_hdl hdl, const std::string& data);
};
```

### 2. JSON数据处理
```cpp
#include <nlohmann/json.hpp>

// 解析客户端消息
nlohmann::json request = nlohmann::json::parse(message);
std::string action = request["action"];
auto data = request["data"];

// 构建响应
nlohmann::json response = {
    {"type", "gameState"},
    {"data", {
        {"currentLocation", "bookstore"},
        {"playerAttributes", {{"observation", 3}}}
    }}
};
```

### 3. 状态机设计
```cpp
class GameState {
public:
    virtual ~GameState() = default;
    virtual void enter() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void handleMessage(const nlohmann::json& message) = 0;
    virtual void exit() = 0;
};

class StateManager {
    std::unique_ptr<GameState> currentState;
public:
    void changeState(std::unique_ptr<GameState> newState);
    void update(float deltaTime);
};
```

## 📋 第一周任务清单

### 任务1：WebSocket服务器基础框架
- [ ] 创建 `src/core/WebSocketServer.h/.cpp`
- [ ] 实现基础的连接管理
- [ ] 添加消息接收和发送功能
- [ ] 测试与前端的连接

**参考代码位置**：`快速上手指南.md` 中的WebSocket服务器示例

### 任务2：API接口设计
- [ ] 创建 `include/core/APITypes.h`
- [ ] 定义消息格式规范
- [ ] 创建 `src/core/APIHandler.h/.cpp`
- [ ] 实现消息路由系统

**API消息格式示例**：
```cpp
// 命令消息
{
    "action": "move",
    "data": {"direction": "north"},
    "timestamp": 1234567890
}

// 响应消息
{
    "type": "locationChange", 
    "data": {"newLocation": "old_street", "description": "..."}
}
```

### 任务3：集成到主引擎
- [ ] 在 `GameEngine.cpp` 中集成WebSocket服务器
- [ ] 修改 `initializeSubsystems()` 函数
- [ ] 实现基础的消息处理流程

## 🔧 开发建议

### 1. 渐进式开发
- **第一步**：实现基础连接，能与前端建立WebSocket连接
- **第二步**：添加简单的ping/pong消息
- **第三步**：实现游戏状态同步
- **第四步**：添加完整的API功能

### 2. 调试技巧
```cpp
// 添加详细的日志输出
std::cout << "[WebSocket] 收到消息: " << message << std::endl;

// 使用JSON pretty print
std::cout << response.dump(2) << std::endl;  // 格式化输出
```

### 3. 错误处理
```cpp
try {
    nlohmann::json json = nlohmann::json::parse(message);
    // 处理消息...
} catch (const nlohmann::json::parse_error& e) {
    std::cerr << "JSON解析错误: " << e.what() << std::endl;
    // 发送错误响应给客户端
}
```

## 🤝 与其他成员协作

### 与前端工程师协调
- **API接口**：共同确定消息格式
- **测试**：使用前端界面测试后端功能
- **调试**：协助排查连接问题

### 与游戏系统开发者协调
- **数据接口**：为游戏逻辑提供网络通信接口
- **事件系统**：设计游戏事件的网络传输

### 与音频工程师协调
- **资源管理**：提供音频文件的加载接口
- **事件触发**：支持音频事件的网络同步

## 📖 学习资源

### 推荐文档
- [websocketpp 官方文档](https://github.com/zaphoyd/websocketpp)
- [nlohmann/json 文档](https://json.nlohmann.me/)
- [现代C++最佳实践](https://isocpp.github.io/CppCoreGuidelines/)

### 示例代码
- 查看 `快速上手指南.md` 中的完整WebSocket服务器示例
- 参考 `shared/data/` 中的JSON数据格式

## 🆘 常见问题

### Q: WebSocket连接失败怎么办？
A: 检查防火墙设置，确认端口8080未被占用，查看控制台错误信息。

### Q: JSON解析出错？
A: 添加异常处理，输出原始消息内容，检查消息格式是否符合规范。

### Q: 如何调试网络通信？
A: 使用浏览器开发者工具的Network面板，或者WebSocket测试工具。

---

**记住**：作为架构师，您的代码是整个项目的基础。保持代码的清晰性和可扩展性非常重要！

有任何问题随时在团队群里询问。加油！🚀
