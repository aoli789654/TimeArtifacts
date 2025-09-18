/**
 * 主程序入口
 * 
 * 负责初始化整个前端应用程序
 */

// 全局变量
let gameClient = null;
let uiManager = null;
let themeManager = null;

/**
 * 页面加载完成后的初始化函数
 */
document.addEventListener('DOMContentLoaded', function() {
    console.log('时光信物前端客户端启动中...');
    
    try {
        // 初始化各个管理器
        initializeManagers();
        
        // 设置事件监听器
        setupEventListeners();
        
        // 加载用户设置
        loadUserSettings();
        
        // 连接到游戏服务器
        connectToServer();
        
        console.log('前端客户端初始化完成');
        
    } catch (error) {
        console.error('初始化失败:', error);
        showNotification('初始化失败: ' + error.message, 'error');
    }
});

/**
 * 初始化各个管理器
 */
function initializeManagers() {
    console.log('初始化管理器...');
    
    // 初始化主题管理器
    if (typeof ThemeManager !== 'undefined') {
        themeManager = new ThemeManager();
        console.log('主题管理器已初始化');
    }
    
    // 初始化UI管理器
    if (typeof UIManager !== 'undefined') {
        uiManager = new UIManager();
        console.log('UI管理器已初始化');
    }
    
    // 初始化游戏客户端
    if (typeof GameClient !== 'undefined') {
        gameClient = new GameClient();
        console.log('游戏客户端已初始化');
        
        // 将UI管理器注册到游戏客户端
        if (uiManager) {
            gameClient.setUIManager(uiManager);
        }
    }
}

/**
 * 设置全局事件监听器
 */
function setupEventListeners() {
    console.log('设置事件监听器...');
    
    // 窗口关闭前清理
    window.addEventListener('beforeunload', function(event) {
        console.log('页面即将关闭，清理资源...');
        
        if (gameClient) {
            gameClient.disconnect();
        }
        
        // 保存用户设置
        saveUserSettings();
    });
    
    // 窗口失去焦点时暂停
    window.addEventListener('blur', function() {
        console.log('窗口失去焦点');
        // TODO: 暂停游戏音效等
    });
    
    // 窗口获得焦点时恢复
    window.addEventListener('focus', function() {
        console.log('窗口获得焦点');
        // TODO: 恢复游戏音效等
    });
    
    // 键盘快捷键
    setupKeyboardShortcuts();
    
    // UI控件事件
    setupUIEvents();
}

/**
 * 设置键盘快捷键
 */
function setupKeyboardShortcuts() {
    document.addEventListener('keydown', function(event) {
        // ESC键：关闭模态对话框
        if (event.key === 'Escape') {
            const dialogueModal = document.getElementById('dialogue-modal');
            if (dialogueModal && !dialogueModal.classList.contains('hidden')) {
                closeDialogue();
                event.preventDefault();
            }
        }
        
        // 数字键1-9：选择对话选项
        if (event.key >= '1' && event.key <= '9') {
            const optionIndex = parseInt(event.key) - 1;
            const dialogueOptions = document.querySelectorAll('.dialogue-option');
            if (dialogueOptions[optionIndex]) {
                dialogueOptions[optionIndex].click();
                event.preventDefault();
            }
        }
        
        // Ctrl+R：重新连接
        if (event.ctrlKey && (event.key === 'r' || event.key === 'R')) {
            if (gameClient && !gameClient.isConnected()) {
                console.log('手动重新连接...');
                connectToServer();
                event.preventDefault();
            }
        }
        
        // F1：显示帮助（将来实现）
        if (event.key === 'F1') {
            // TODO: 显示帮助界面
            event.preventDefault();
        }
    });
}

/**
 * 设置UI控件事件
 */
function setupUIEvents() {
    // 主题选择器
    const themeSelector = document.getElementById('theme-selector');
    if (themeSelector) {
        themeSelector.addEventListener('change', function() {
            const selectedTheme = this.value;
            if (themeManager) {
                themeManager.setTheme(selectedTheme);
            }
        });
    }
    
    // 文字速度滑块
    const textSpeedSlider = document.getElementById('text-speed');
    if (textSpeedSlider) {
        textSpeedSlider.addEventListener('input', function() {
            const speed = parseInt(this.value);
            if (uiManager) {
                uiManager.setTextSpeed(speed);
            }
            
            // 保存设置
            localStorage.setItem(getConfig('storage.textSpeed'), speed);
        });
    }
    
    // 重新连接按钮
    const reconnectBtn = document.getElementById('reconnect-btn');
    if (reconnectBtn) {
        reconnectBtn.addEventListener('click', function() {
            connectToServer();
        });
    }
    
    // 对话框关闭按钮
    const closeDialogueBtn = document.getElementById('close-dialogue');
    if (closeDialogueBtn) {
        closeDialogueBtn.addEventListener('click', closeDialogue);
    }
    
    // 模态对话框背景点击关闭
    const dialogueModal = document.getElementById('dialogue-modal');
    if (dialogueModal) {
        const backdrop = dialogueModal.querySelector('.modal-backdrop');
        if (backdrop) {
            backdrop.addEventListener('click', closeDialogue);
        }
    }
}

/**
 * 加载用户设置
 */
function loadUserSettings() {
    console.log('加载用户设置...');
    
    try {
        // 加载主题设置
        const savedTheme = localStorage.getItem(getConfig('storage.theme'));
        if (savedTheme && themeManager) {
            themeManager.setTheme(savedTheme);
            
            const themeSelector = document.getElementById('theme-selector');
            if (themeSelector) {
                themeSelector.value = savedTheme;
            }
        }
        
        // 加载文字速度设置
        const savedTextSpeed = localStorage.getItem(getConfig('storage.textSpeed'));
        if (savedTextSpeed) {
            const speed = parseInt(savedTextSpeed);
            if (uiManager) {
                uiManager.setTextSpeed(speed);
            }
            
            const textSpeedSlider = document.getElementById('text-speed');
            if (textSpeedSlider) {
                textSpeedSlider.value = speed;
            }
        }
        
        console.log('用户设置加载完成');
        
    } catch (error) {
        console.warn('加载用户设置时出错:', error);
    }
}

/**
 * 保存用户设置
 */
function saveUserSettings() {
    try {
        // 这里会自动保存，因为设置更改时就会写入localStorage
        console.log('用户设置已保存');
    } catch (error) {
        console.warn('保存用户设置时出错:', error);
    }
}

/**
 * 连接到游戏服务器
 */
function connectToServer() {
    if (!gameClient) {
        console.error('游戏客户端未初始化');
        return;
    }
    
    console.log('连接到游戏服务器...');
    
    const serverUrl = getConfig('network.serverUrl');
    gameClient.connect(serverUrl);
    
    // 更新重新连接按钮状态
    const reconnectBtn = document.getElementById('reconnect-btn');
    if (reconnectBtn) {
        reconnectBtn.disabled = true;
        reconnectBtn.textContent = '连接中...';
        
        // 5秒后重新启用按钮
        setTimeout(() => {
            if (!gameClient.isConnected()) {
                reconnectBtn.disabled = false;
                reconnectBtn.textContent = '重新连接';
            }
        }, 5000);
    }
}

/**
 * 关闭对话框
 */
function closeDialogue() {
    const dialogueModal = document.getElementById('dialogue-modal');
    if (dialogueModal) {
        dialogueModal.classList.add('hidden');
    }
}

/**
 * 显示通知消息
 * @param {string} message 消息内容
 * @param {string} type 消息类型：success, error, warning, info
 * @param {number} duration 显示持续时间（毫秒）
 */
function showNotification(message, type = 'info', duration = null) {
    if (uiManager && uiManager.showNotification) {
        uiManager.showNotification(message, type, duration);
    } else {
        // 备用方案：直接显示alert
        console.log(`[${type.toUpperCase()}] ${message}`);
    }
}

/**
 * 全局错误处理
 */
window.addEventListener('error', function(event) {
    console.error('全局错误:', event.error);
    showNotification('发生意外错误，请刷新页面重试', 'error');
});

/**
 * Promise异常处理
 */
window.addEventListener('unhandledrejection', function(event) {
    console.error('未处理的Promise异常:', event.reason);
    showNotification('操作失败，请重试', 'error');
});

// 导出全局函数供其他模块使用
window.showNotification = showNotification;
window.closeDialogue = closeDialogue;

console.log('主程序模块已加载');
