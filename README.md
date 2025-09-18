# 时光信物 (Time Artifacts)

一个基于C++后端和Web前端的文本冒险游戏

## 项目概述

《时光信物》是一个情感丰富的文本冒险游戏，采用现代化的前后端分离架构：
- **后端**：C++处理游戏逻辑和数据管理
- **前端**：Web技术提供现代化用户界面
- **通信**：WebSocket实现实时交互

## 快速开始

### 环境要求
- **后端开发**：C++17编译器、CMake、vcpkg
- **前端开发**：现代浏览器、Node.js（可选）

### 项目结构
```
TimeArtifacts/
├── backend/        # C++后端代码
├── frontend/       # Web前端代码  
├── shared/         # 共享资源和数据
├── docs/          # 项目文档
└── tools/         # 开发工具和脚本
```

### 团队成员入门指南
- [后端架构师入门指南](docs/backend-getting-started.md)
- [前端工程师入门指南](docs/frontend-getting-started.md)
- [游戏系统开发者指南](docs/gameplay-getting-started.md)
- [音频工程师入门指南](docs/audio-getting-started.md)

## 开发状态

### 第一阶段目标（第1-3周）
- [ ] 建立基础项目结构
- [ ] 实现WebSocket通信
- [ ] 创建基础游戏界面
- [ ] 实现核心游戏逻辑

## 贡献指南

1. 克隆项目到本地
2. 查看对应角色的入门指南
3. 创建功能分支进行开发
4. 提交Pull Request进行代码审查

## 技术栈

### 后端
- C++17/20
- nlohmann/json
- websocketpp
- SFML Audio
- CMake

### 前端
- HTML5/CSS3/JavaScript
- WebSocket API
- 响应式设计

## 联系方式

项目开发中如有问题，请通过以下方式联系：
- 创建Issue
- 团队内部沟通群组

---

**让我们一起打造一个令人印象深刻的游戏作品！** 🎮✨
