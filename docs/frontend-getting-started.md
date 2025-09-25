# 前端UI/UX工程师入门指南

欢迎加入时光信物项目！作为前端工程师，您将负责创建现代化、美观的游戏界面，为玩家提供优秀的用户体验。

## 🎯 您的职责

### 第一阶段任务（第1-3周）
1. **创建WebSocket客户端** - 与后端建立实时通信
2. **设计响应式界面** - 支持不同屏幕尺寸的现代化UI
3. **实现UI状态管理** - 管理界面的动态更新
4. **添加视觉效果** - 主题切换、动画、过渡效果
5. **优化用户体验** - 交互反馈、错误处理、无障碍设计

## 🚀 环境搭建

### 1. 安装Node.js（推荐但非必需）
```bash
# 下载并安装Node.js
# https://nodejs.org/

# 验证安装
node --version
npm --version
```

### 2. 项目初始化
```bash
cd frontend

# 安装依赖（如果使用Node.js）
npm install

# 启动开发服务器（方式1：使用Vite）
npm run dev

# 启动开发服务器（方式2：使用Python）
python -m http.server 3000

# 启动开发服务器（方式3：使用Node.js）
npx serve .
```

### 3. 在浏览器中访问
- 打开 `http://localhost:3000`
- 或直接用浏览器打开 `index.html`

## 📚 核心技术栈

### 1. WebSocket客户端通信（重点）
```javascript
class GameClient {
    constructor() {
        this.ws = null;
        this.isConnected = false;
    }
    
    connect(url = 'ws://localhost:8080') {
        this.ws = new WebSocket(url);
        this.setupEventHandlers();
    }
    
    sendCommand(action, data) {
        const message = {
            action: action,
            data: data,
            timestamp: Date.now()
        };
        this.ws.send(JSON.stringify(message));
    }
}
```

### 2. 动态UI更新
```javascript
// 更新游戏状态
updateGameState(data) {
    // 更新位置显示
    document.getElementById('location-name').textContent = data.currentLocation;
    
    // 更新玩家属性
    this.updatePlayerAttributes(data.playerAttributes);
    
    // 更新可用操作
    this.updateAvailableActions(data.availableActions);
}

// 打字机效果
async typewriterText(element, text, speed = 50) {
    element.textContent = '';
    for (let i = 0; i < text.length; i++) {
        element.textContent += text.charAt(i);
        await new Promise(resolve => setTimeout(resolve, speed));
    }
}
```

### 3. CSS动画和主题系统
```css
/* CSS变量用于主题切换 */
:root {
    --color-primary: #1a1a1a;
    --color-accent: #d4af37;
}

[data-theme="vintage"] {
    --color-primary: #2c1810;
    --color-accent: #cd853f;
}

/* 平滑过渡动画 */
.panel {
    transition: all 0.3s ease;
}

/* 打字机效果 */
.typing {
    border-right: 2px solid var(--color-accent);
    animation: blink-caret 1s step-end infinite;
}
```

## 📋 第一周任务清单

### 任务1：WebSocket客户端实现
- [ ] 完善 `js/gameClient.js` 文件
- [ ] 实现连接管理和重连机制
- [ ] 添加消息处理和错误处理
- [ ] 测试与后端的通信

**参考代码**：查看现有的 `frontend/js/gameClient.js` 框架

### 任务2：UI组件系统
- [ ] 创建 `js/uiManager.js` - UI状态管理器
- [ ] 创建 `js/components/` 目录
- [ ] 实现对话框、通知、属性条等组件
- [ ] 添加组件的CSS样式

**UI组件示例**：
```javascript
class UIManager {
    showDialogue(speaker, text, options) {
        // 显示对话框
    }
    
    updateAttributes(attributes) {
        // 更新属性显示
    }
    
    showNotification(message, type) {
        // 显示通知消息
    }
}
```

### 任务3：主题和视觉效果
- [ ] 完善 `css/themes.css` - 主题系统
- [ ] 创建 `css/components.css` - 组件样式
- [ ] 实现 `js/themeManager.js` - 主题管理
- [ ] 添加过渡动画和视觉反馈

## 🎨 设计指南

### 1. 界面布局
```
┌─────────────────────────────────────────┐
│           游戏标题栏                    │
├─────────────────────────┬───────────────┤
│                         │   玩家状态    │
│      主要故事区域       │               │
│                         ├───────────────┤
│                         │   操作面板    │
│                         │               │
│                         ├───────────────┤
│                         │   设置面板    │
└─────────────────────────┴───────────────┘
```

### 2. 颜色方案
- **默认主题**：深色背景 + 金色重点
- **怀旧主题**：棕色温暖色调
- **暗夜主题**：纯黑背景 + 蓝色重点

### 3. 字体选择
- **主字体**：衬线字体（Georgia, 思源宋体）
- **界面字体**：无衬线字体（Arial, 苹方）

### 4. 动画原则
- **功能性动画**：状态变化、加载、反馈
- **装饰性动画**：打字机效果、粒子背景
- **性能优化**：使用CSS动画，避免JavaScript动画

## 📱 响应式设计

### 桌面版（>1024px）
- 侧边栏固定宽度320px
- 主内容区域自适应
- 完整功能显示

### 平板版（768px-1024px）
- 侧边栏宽度调整为280px
- 字体稍微放大
- 保持主要功能

### 手机版（<768px）
- 垂直布局，侧边栏移到底部
- 水平滚动的小面板
- 优化触摸操作

```css
@media (max-width: 768px) {
    .game-layout {
        flex-direction: column;
    }
    
    .sidebar {
        flex-direction: row;
        overflow-x: auto;
    }
}
```

## 🔧 开发工具和技巧

### 1. 浏览器开发者工具
- **Elements面板**：调试CSS和DOM结构
- **Console面板**：查看JavaScript错误和日志
- **Network面板**：监控WebSocket连接
- **Application面板**：查看localStorage数据

### 2. 实时调试
```javascript
// 开启调试模式
window.GameConfig.game.debug = true;

// 添加调试信息
console.log('[DEBUG] WebSocket消息:', message);

// 测试UI组件
window.testDialogue = function() {
    uiManager.showDialogue('测试角色', '这是一条测试消息', [
        {text: '选项1', id: 'option1'},
        {text: '选项2', id: 'option2'}
    ]);
};
```

### 3. 性能优化
- 使用 `requestAnimationFrame` 进行动画
- 避免频繁的DOM操作
- 使用CSS `transform` 和 `opacity` 进行动画
- 合理使用防抖和节流

## 🤝 与其他成员协作

### 与后端架构师协调
- **API接口**：确认消息格式和数据结构
- **测试**：共同测试前后端通信
- **调试**：协助定位网络问题

### 与游戏系统开发者协调
- **数据展示**：确认游戏数据的显示格式
- **交互流程**：设计用户操作到游戏逻辑的流程

### 与音频工程师协调
- **音效触发**：在UI事件中触发音效
- **音频控制**：提供音量控制界面

## 📖 学习资源

### HTML/CSS/JavaScript基础
- [MDN Web文档](https://developer.mozilla.org/zh-CN/)
- [CSS-Tricks](https://css-tricks.com/)
- [JavaScript.info](https://javascript.info/)

### 现代前端技术
- [WebSocket API文档](https://developer.mozilla.org/en-US/docs/Web/API/WebSocket)
- [CSS Grid布局](https://css-tricks.com/snippets/css/complete-guide-grid/)
- [CSS动画指南](https://css-tricks.com/almanac/properties/a/animation/)

### UI/UX设计
- [Material Design指南](https://material.io/design)
- [Apple Human Interface Guidelines](https://developer.apple.com/design/human-interface-guidelines/)

## 🆘 常见问题

### Q: WebSocket连接不上后端？
A: 
1. 确认后端服务器已启动
2. 检查端口号是否正确（默认8080）
3. 查看浏览器开发者工具的Console和Network面板
4. 尝试关闭防火墙或杀毒软件

### Q: CSS样式不生效？
A: 
1. 检查CSS选择器是否正确
2. 查看是否有样式冲突
3. 确认CSS文件是否正确引入
4. 使用浏览器开发者工具检查计算样式

### Q: JavaScript报错怎么办？
A: 
1. 查看浏览器Console面板的错误信息
2. 检查变量名是否拼写正确
3. 确认函数调用时机是否正确
4. 添加try-catch语句进行错误处理

### Q: 响应式设计测试？
A: 
1. 使用浏览器开发者工具的设备模拟
2. 调整浏览器窗口大小
3. 在真实设备上测试
4. 使用在线响应式测试工具

## 🎁 实用代码片段

### 简单的通知系统
```javascript
function showNotification(message, type = 'info') {
    const notification = document.createElement('div');
    notification.className = `notification ${type}`;
    notification.textContent = message;
    
    document.body.appendChild(notification);
    
    setTimeout(() => {
        notification.remove();
    }, 3000);
}
```

### 本地存储管理
```javascript
const Storage = {
    set(key, value) {
        localStorage.setItem(key, JSON.stringify(value));
    },
    
    get(key, defaultValue = null) {
        try {
            const value = localStorage.getItem(key);
            return value ? JSON.parse(value) : defaultValue;
        } catch {
            return defaultValue;
        }
    }
};
```

---

**记住**：前端是玩家与游戏交互的第一界面，优秀的用户体验将直接影响游戏的成功！

专注于细节，追求完美的用户体验。有任何问题随时询问！✨
