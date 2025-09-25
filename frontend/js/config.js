/**
 * 游戏配置文件
 * 
 * 包含所有可配置的常量和设置
 */

// 游戏配置对象
window.GameConfig = {
    // 网络配置
    network: {
        // WebSocket服务器地址
        serverUrl: 'ws://localhost:8080',
        
        // 重连配置
        reconnect: {
            maxAttempts: 5,
            delayMs: 2000,
            backoffMultiplier: 1.5
        },
        
        // 超时配置
        timeout: {
            connectionMs: 10000,
            responseMs: 5000
        }
    },
    
    // UI配置
    ui: {
        // 文字显示速度 (字符/秒)
        textSpeed: {
            min: 10,
            max: 100,
            default: 50
        },
        
        // 动画持续时间
        animation: {
            fadeMs: 300,
            slideMs: 250,
            bounceMs: 400
        },
        
        // 主题配置
        themes: {
            default: 'default',
            available: ['default', 'vintage', 'dark']
        },
        
        // 通知配置
        notifications: {
            durationMs: 3000,
            maxCount: 5
        }
    },
    
    // 游戏设置
    game: {
        // 自动保存间隔 (毫秒)
        autoSaveIntervalMs: 30000,
        
        // 历史记录最大条数
        maxHistoryEntries: 50,
        
        // 调试模式
        debug: false
    },
    
    // 本地存储键名
    storage: {
        theme: 'timeArtifacts_theme',
        textSpeed: 'timeArtifacts_textSpeed',
        gameState: 'timeArtifacts_gameState',
        settings: 'timeArtifacts_settings'
    },
    
    // 音效配置 (供音频工程师使用)
    audio: {
        enabled: true,
        volume: {
            master: 0.7,
            music: 0.5,
            sfx: 0.8
        }
    },
    
    // API消息类型
    messageTypes: {
        // 客户端发送
        COMMAND: 'command',
        DIALOGUE_CHOICE: 'dialogue_choice',
        PING: 'ping',
        
        // 服务器发送
        GAME_STATE: 'gameState',
        DIALOGUE: 'dialogue',
        LOCATION_CHANGE: 'locationChange',
        EXAMINATION: 'examination',
        ATTRIBUTE_UPDATE: 'attributeUpdate',
        ITEM_ACQUIRED: 'itemAcquired',
        INSIGHT_GAINED: 'insightGained',
        ERROR: 'error',
        PONG: 'pong'
    },
    
    // 游戏动作类型
    actions: {
        MOVE: 'move',
        EXAMINE: 'examine',
        TALK: 'talk',
        USE_ITEM: 'use_item',
        LOOK_AROUND: 'look_around'
    },
    
    // 属性类型
    attributes: {
        OBSERVATION: 'observation',
        COMMUNICATION: 'communication', 
        ACTION: 'action',
        EMPATHY: 'empathy'
    },
    
    // 属性显示名称映射
    attributeNames: {
        observation: '观察力',
        communication: '沟通力',
        action: '行动力',
        empathy: '共情值'
    },
    
    // 地点显示名称映射
    locationNames: {
        bookstore: '时光角落书店',
        old_street: '古老街道',
        harbor: '旧港码头',
        library: '市立图书馆',
        park: '梧桐公园'
    },
    
    // 操作显示名称映射
    actionNames: {
        examine_bookshelf: '检查书架',
        talk_to_owner: '与店主交谈',
        look_around: '环顾四周',
        examine_street_lamp: '查看路灯',
        enter_bookstore: '进入书店',
        walk_to_harbor: '前往港口',
        read_book: '阅读书籍',
        sit_on_bench: '坐在长椅上'
    },
    
    // 错误消息
    errors: {
        CONNECTION_FAILED: '无法连接到游戏服务器',
        CONNECTION_LOST: '与服务器的连接已断开',
        INVALID_MESSAGE: '收到无效的服务器消息',
        COMMAND_FAILED: '命令执行失败',
        TIMEOUT: '请求超时',
        UNKNOWN_ERROR: '发生未知错误'
    },
    
    // 成功消息
    messages: {
        CONNECTED: '已成功连接到游戏服务器',
        RECONNECTED: '重新连接成功',
        GAME_SAVED: '游戏进度已保存',
        SETTINGS_SAVED: '设置已保存'
    }
};

// 冻结配置对象，防止意外修改
Object.freeze(window.GameConfig);

// 便利函数：获取配置值
window.getConfig = function(path, defaultValue = null) {
    const keys = path.split('.');
    let value = window.GameConfig;
    
    for (const key of keys) {
        if (value && typeof value === 'object' && key in value) {
            value = value[key];
        } else {
            return defaultValue;
        }
    }
    
    return value;
};

// 便利函数：检查调试模式
window.isDebugMode = function() {
    return window.getConfig('game.debug', false);
};

// 便利函数：记录调试信息
window.debugLog = function(...args) {
    if (window.isDebugMode()) {
        console.log('[DEBUG]', ...args);
    }
};

// 便利函数：记录错误信息
window.errorLog = function(...args) {
    console.error('[ERROR]', ...args);
};

// 便利函数：记录警告信息
window.warnLog = function(...args) {
    console.warn('[WARN]', ...args);
};

console.log('游戏配置已加载:', window.GameConfig);
