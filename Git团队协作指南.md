# Git团队协作指南

## 🎯 为什么使用Git而不是压缩包？

### Git的优势：
- ✅ **版本控制** - 完整的修改历史，可以回退任何版本
- ✅ **并行开发** - 多人同时开发不同功能，自动合并
- ✅ **冲突解决** - 智能处理代码冲突
- ✅ **分支管理** - 每个功能独立开发，稳定合并
- ✅ **专业标准** - 业界标准的协作方式
- ✅ **备份安全** - 代码保存在云端，永不丢失

### 压缩包的问题：
- ❌ **版本混乱** - 不知道谁改了什么
- ❌ **文件覆盖** - 容易丢失他人的修改
- ❌ **无法合并** - 多人修改同一文件时冲突
- ❌ **无备份** - 文件丢失就完全丢失了

---

## 🚀 方案A：使用GitHub/Gitee（推荐）

### 第一步：项目负责人创建仓库

#### 1.1 在本地初始化Git仓库
```bash
# 在项目根目录执行
cd TimeArtifacts
git init

# 添加所有文件
git add .

# 创建首次提交
git commit -m "🎉 初始化时光信物项目

- 创建前后端分离架构
- 添加完整的开发文档
- 设置项目基础框架
- 准备团队协作环境"
```

#### 1.2 在GitHub/Gitee创建远程仓库

**GitHub方式（国际）：**
1. 访问 [GitHub.com](https://github.com)
2. 点击右上角 "+" → "New repository"
3. 仓库名：`TimeArtifacts` 或 `time-artifacts`
4. 描述：`时光信物 - 一个关于记忆与遗忘的文本冒险游戏`
5. 选择 Public 或 Private（建议Private用于团队项目）
6. **不要**勾选 "Add a README file"（我们已经有了）
7. 点击 "Create repository"

**Gitee方式（国内，速度更快）：**
1. 访问 [Gitee.com](https://gitee.com)
2. 点击右上角 "+" → "新建仓库"
3. 仓库名称：`TimeArtifacts`
4. 仓库介绍：`时光信物游戏项目`
5. 选择私有或公开
6. **不要**勾选使用Readme文件初始化
7. 点击 "创建"

#### 1.3 连接本地仓库到远程
```bash
# GitHub示例（替换为你的用户名）
git remote add origin https://github.com/你的用户名/TimeArtifacts.git

# 或者Gitee示例
git remote add origin https://gitee.com/你的用户名/TimeArtifacts.git

# 推送到远程仓库
git branch -M main
git push -u origin main
```

### 第二步：邀请团队成员

#### 2.1 添加协作者
**GitHub：**
1. 进入仓库页面
2. 点击 "Settings" → "Manage access"
3. 点击 "Invite a collaborator"
4. 输入团队成员的GitHub用户名或邮箱

**Gitee：**
1. 进入仓库页面
2. 点击 "管理" → "仓库成员管理"
3. 点击 "添加仓库成员"
4. 输入成员的Gitee用户名

#### 2.2 发送仓库地址给团队成员
```
仓库地址：https://github.com/用户名/TimeArtifacts
或者：https://gitee.com/用户名/TimeArtifacts

团队成员克隆命令：
git clone https://github.com/用户名/TimeArtifacts.git
```

### 第三步：团队成员获取项目

```bash
# 克隆项目到本地
git clone https://github.com/用户名/TimeArtifacts.git

# 进入项目目录
cd TimeArtifacts

# 查看项目结构
ls -la

# 查看当前分支
git branch

# 查看项目状态
git status
```

---

## 🌿 分支管理策略

### 分支结构
```
main (主分支)
├── develop (开发分支)
├── feature/backend-websocket (后端架构师)
├── feature/frontend-ui (前端工程师)
├── feature/gameplay-player (游戏系统开发者)
└── feature/audio-system (音频工程师)
```

### 创建功能分支
```bash
# 每个成员创建自己的功能分支
git checkout -b feature/你的功能名称

# 例如：
git checkout -b feature/backend-websocket     # 后端架构师
git checkout -b feature/frontend-ui           # 前端工程师
git checkout -b feature/gameplay-player       # 游戏系统开发者
git checkout -b feature/audio-system          # 音频工程师
```

### 日常开发流程
```bash
# 1. 确保在自己的分支上
git branch

# 2. 开始工作前，拉取最新代码
git pull origin main

# 3. 进行开发...

# 4. 添加修改的文件
git add .
# 或者只添加特定文件
git add src/core/WebSocketServer.cpp

# 5. 提交修改
git commit -m "✨ 实现WebSocket服务器基础功能

- 添加连接管理
- 实现消息收发
- 添加错误处理"

# 6. 推送到远程分支
git push origin feature/你的分支名
```

### 合并到主分支
```bash
# 1. 切换到主分支
git checkout main

# 2. 拉取最新的主分支代码
git pull origin main

# 3. 合并你的功能分支
git merge feature/你的分支名

# 4. 推送合并后的代码
git push origin main

# 5. 删除已合并的功能分支（可选）
git branch -d feature/你的分支名
git push origin --delete feature/你的分支名
```

---

## 🤝 团队协作规范

### 提交信息规范
使用语义化的提交信息：

```bash
# 新功能
git commit -m "✨ 添加WebSocket通信模块"

# 修复bug
git commit -m "🐛 修复音频播放时的内存泄漏"

# 文档更新
git commit -m "📝 更新API接口文档"

# 性能优化
git commit -m "⚡ 优化JSON数据加载性能"

# 重构代码
git commit -m "♻️ 重构Player类的属性管理系统"

# 测试相关
git commit -m "✅ 添加物品系统单元测试"

# 构建相关
git commit -m "🔧 更新CMake配置支持新的依赖库"
```

### 代码审查流程
```bash
# 1. 创建Pull Request (GitHub) 或 Merge Request (Gitee)
# 2. 团队成员进行代码审查
# 3. 修复审查中发现的问题
# 4. 审查通过后合并到主分支
```

### 冲突解决
```bash
# 当出现合并冲突时
git pull origin main
# 手动编辑冲突文件，选择要保留的代码
# 删除冲突标记 <<<<<<< ======= >>>>>>>
git add 冲突文件名
git commit -m "🔀 解决合并冲突"
git push origin 你的分支名
```

---

## 📋 团队成员快速上手指南

### 对于没有Git经验的成员

#### 1. 安装Git
- **Windows**: 下载 [Git for Windows](https://git-scm.com/download/win)
- **Mac**: `brew install git` 或从官网下载
- **Linux**: `sudo apt install git` (Ubuntu) 或 `sudo yum install git` (CentOS)

#### 2. 配置Git
```bash
git config --global user.name "你的姓名"
git config --global user.email "你的邮箱@example.com"
```

#### 3. 常用命令速查
```bash
# 查看状态
git status

# 查看修改内容
git diff

# 查看提交历史
git log --oneline

# 切换分支
git checkout 分支名

# 查看所有分支
git branch -a

# 拉取远程更新
git pull

# 推送本地修改
git push
```

### 推荐的Git GUI工具
- **VS Code** - 内置Git支持，推荐！
- **GitHub Desktop** - 简单易用的图形界面
- **SourceTree** - 功能强大的Git客户端
- **GitKraken** - 美观的Git可视化工具

---

## 🆘 常见问题解决

### Q: 忘记切换分支，在main分支上修改了代码？
```bash
# 保存当前修改
git stash

# 切换到正确的分支
git checkout feature/你的分支名

# 恢复修改
git stash pop
```

### Q: 推送时提示权限不足？
```bash
# 检查远程地址
git remote -v

# 重新设置远程地址（使用你的用户名）
git remote set-url origin https://github.com/你的用户名/TimeArtifacts.git
```

### Q: 本地文件与远程冲突？
```bash
# 强制拉取远程版本（会丢失本地修改，慎用）
git reset --hard origin/main

# 或者保存本地修改后再拉取
git stash
git pull
git stash pop
```

### Q: 误提交了敏感文件？
```bash
# 从Git历史中完全删除文件
git rm --cached 敏感文件名
echo "敏感文件名" >> .gitignore
git commit -m "🔒 移除敏感文件并添加到忽略列表"
```

---

## 💡 最佳实践建议

### 1. 每日同步
```bash
# 每天开始工作前
git checkout main
git pull origin main
git checkout feature/你的分支
git merge main  # 或者 git rebase main
```

### 2. 小步提交
- 每完成一个小功能就提交
- 避免一次提交大量不相关的修改
- 提交信息要清晰描述做了什么

### 3. 备份重要工作
```bash
# 定期推送你的分支
git push origin feature/你的分支名
```

### 4. 保持代码整洁
```bash
# 提交前检查代码格式
git diff --check

# 使用 .gitignore 忽略不需要的文件
```

---

## 🎯 项目特定的工作流程

### 第一阶段开发流程（第1-3周）

#### 第1周：基础架构
```bash
# 每个成员的任务分支
git checkout -b feature/week1-backend-架构师姓名
git checkout -b feature/week1-frontend-工程师姓名
git checkout -b feature/week1-gameplay-开发者姓名
git checkout -b feature/week1-audio-工程师姓名
```

#### 第2周：功能实现
```bash
# 基于第1周的成果继续开发
git checkout -b feature/week2-integration-团队
```

#### 第3周：系统集成
```bash
# 合并所有功能到开发分支
git checkout develop
git merge feature/week1-backend-架构师姓名
git merge feature/week1-frontend-工程师姓名
# ... 其他分支
```

### 紧急修复流程
```bash
# 发现严重bug时
git checkout main
git checkout -b hotfix/修复描述
# 修复bug...
git commit -m "🚑 紧急修复: 修复描述"
git checkout main
git merge hotfix/修复描述
git push origin main
```

---

使用Git不仅让团队协作更高效，也是专业软件开发的标准做法。这将为你们的项目增加很多专业分！
