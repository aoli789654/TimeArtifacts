#!/bin/bash

# 时光信物项目Git初始化脚本
# 运行此脚本来初始化Git仓库

echo "🎮 时光信物项目 - Git初始化脚本"
echo "================================="

# 检查是否已经是Git仓库
if [ -d ".git" ]; then
    echo "⚠️  检测到已存在Git仓库"
    read -p "是否要重新初始化？这将删除现有的Git历史 (y/N): " confirm
    if [ "$confirm" = "y" ] || [ "$confirm" = "Y" ]; then
        rm -rf .git
        echo "✅ 已删除现有Git仓库"
    else
        echo "❌ 取消初始化"
        exit 1
    fi
fi

# 初始化Git仓库
echo "📁 初始化Git仓库..."
git init

# 检查Git用户配置
echo "🔧 检查Git配置..."
git_name=$(git config --global user.name)
git_email=$(git config --global user.email)

if [ -z "$git_name" ] || [ -z "$git_email" ]; then
    echo "⚠️  Git用户信息未配置"
    echo "请输入您的信息："
    
    if [ -z "$git_name" ]; then
        read -p "姓名: " user_name
        git config --global user.name "$user_name"
    fi
    
    if [ -z "$git_email" ]; then
        read -p "邮箱: " user_email
        git config --global user.email "$user_email"
    fi
    
    echo "✅ Git用户信息配置完成"
else
    echo "✅ Git用户信息已配置: $git_name <$git_email>"
fi

# 添加所有文件
echo "📦 添加项目文件..."
git add .

# 创建初始提交
echo "💾 创建初始提交..."
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

echo ""
echo "🎉 Git仓库初始化完成！"
echo ""
echo "📋 下一步操作："
echo "1. 在GitHub/Gitee创建远程仓库"
echo "2. 连接远程仓库:"
echo "   git remote add origin https://github.com/用户名/TimeArtifacts.git"
echo "   git branch -M main"
echo "   git push -u origin main"
echo ""
echo "3. 邀请团队成员加入仓库"
echo "4. 团队成员克隆项目:"
echo "   git clone https://github.com/用户名/TimeArtifacts.git"
echo ""
echo "📖 详细说明请查看: Git团队协作指南.md"
