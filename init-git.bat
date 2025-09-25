@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

REM 时光信物项目Git初始化脚本 (Windows版本)
echo 🎮 时光信物项目 - Git初始化脚本
echo =================================

REM 检查是否已经是Git仓库
if exist ".git" (
    echo ⚠️  检测到已存在Git仓库
    set /p confirm="是否要重新初始化？这将删除现有的Git历史 (y/N): "
    if /i "!confirm!"=="y" (
        rmdir /s /q .git
        echo ✅ 已删除现有Git仓库
    ) else (
        echo ❌ 取消初始化
        pause
        exit /b 1
    )
)

REM 初始化Git仓库
echo 📁 初始化Git仓库...
git init

REM 检查Git用户配置
echo 🔧 检查Git配置...
for /f "tokens=*" %%i in ('git config --global user.name 2^>nul') do set git_name=%%i
for /f "tokens=*" %%i in ('git config --global user.email 2^>nul') do set git_email=%%i

if "!git_name!"=="" (
    echo ⚠️  Git用户信息未配置
    set /p user_name="请输入您的姓名: "
    git config --global user.name "!user_name!"
)

if "!git_email!"=="" (
    set /p user_email="请输入您的邮箱: "
    git config --global user.email "!user_email!"
)

if "!git_name!"=="" (
    echo ✅ Git用户信息配置完成
) else (
    echo ✅ Git用户信息已配置: !git_name! ^<!git_email!^>
)

REM 添加所有文件
echo 📦 添加项目文件...
git add .

REM 创建初始提交
echo 💾 创建初始提交...
git commit -m "🎉 初始化时光信物项目

📋 项目特性:
- 前后端分离架构 (C++ + Web)
- 完整的开发文档和指南
- 四人团队协作框架
- 现代化的技术栈

🎯 第一阶段目标:
- WebSocket实时通信
- 响应式游戏界面
- 核心游戏逻辑
- 音频系统集成

👥 团队角色:
- 后端架构师: C++引擎和WebSocket
- 前端工程师: Web界面和用户体验
- 游戏系统开发者: 玩家系统和游戏逻辑
- 音频工程师: 音频系统和特色功能"

echo.
echo 🎉 Git仓库初始化完成！
echo.
echo 📋 下一步操作：
echo 1. 在GitHub/Gitee创建远程仓库
echo 2. 连接远程仓库:
echo    git remote add origin https://github.com/用户名/TimeArtifacts.git
echo    git branch -M main
echo    git push -u origin main
echo.
echo 3. 邀请团队成员加入仓库
echo 4. 团队成员克隆项目:
echo    git clone https://github.com/用户名/TimeArtifacts.git
echo.
echo 📖 详细说明请查看: Git团队协作指南.md
echo.
pause
