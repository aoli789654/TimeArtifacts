/**
 * 游戏客户端
 * 负责与后端WebSocket服务器通信
 */

class GameClient {
    constructor() {
        this.ws = null;
        this.isConnectedFlag = false;
        this.reconnectAttempts = 0;
        this.maxReconnectAttempts = 5;
        this.uiManager = null;
        
        console.log('[GameClient] 游戏客户端已创建');
    }
    
    /**
     * 设置UI管理器
     */
    setUIManager(uiManager) {
        this.uiManager = uiManager;
        console.log('[GameClient] UI管理器已设置');
    }
    
    /**
     * 连接到游戏服务器
     */
    connect(serverUrl = 'ws://localhost:8080') {
        console.log('[GameClient] 尝试连接到:', serverUrl);
        
        try {
            this.ws = new WebSocket(serverUrl);
            this.setupEventHandlers();
        } catch (error) {
            console.error('[GameClient] 连接失败:', error);
            this.handleConnectionError();
        }
    }
    
    /**
     * 设置WebSocket事件处理器
     */
    setupEventHandlers() {
        this.ws.onopen = () => {
            console.log('[GameClient] 已连接到游戏服务器');
            this.isConnectedFlag = true;
            this.reconnectAttempts = 0;
            this.updateConnectionStatus('已连接', 'connected');
            
            if (this.uiManager) {
                this.uiManager.onConnectionEstablished();
            }
        };
        
        this.ws.onmessage = (event) => {
            try {
                const message = JSON.parse(event.data);
                console.log('[GameClient] 收到消息:', message);
                this.handleGameMessage(message);
            } catch (error) {
                console.error('[GameClient] 解析消息失败:', error);
            }
        };
        
        this.ws.onclose = () => {
            console.log('[GameClient] 与服务器断开连接');
            this.isConnectedFlag = false;
            this.updateConnectionStatus('连接断开', 'disconnected');
            
            if (this.uiManager) {
                this.uiManager.onConnectionLost();
            }
            
            this.attemptReconnect();
        };
        
        this.ws.onerror = (error) => {
            console.error('[GameClient] WebSocket错误:', error);
            this.handleConnectionError();
        };
    }
    
    /**
     * 处理游戏消息
     */
    handleGameMessage(message) {
        if (!this.uiManager) {
            console.warn('[GameClient] UI管理器未设置，无法处理消息');
            return;
        }
        
        switch (message.type) {
            case 'gameState':
                this.uiManager.updateGameState(message.data);
                break;
                
            case 'dialogue':
                this.uiManager.showDialogue(message.data);
                break;
                
            case 'sceneUpdate':
                this.uiManager.updateScene(message.data);
                break;
                
            case 'examination':
                this.uiManager.showExaminationResult(message.data);
                break;
                
            case 'notification':
                this.uiManager.showNotification(message.data.content, message.data.level);
                break;
                
            case 'error':
                this.uiManager.showNotification(message.data.errorMessage, 'error');
                break;
                
            default:
                console.warn('[GameClient] 未知消息类型:', message.type);
        }
    }
    
    /**
     * 发送命令到服务器
     */
    sendCommand(action, data = {}) {
        if (!this.isConnected()) {
            console.warn('[GameClient] 未连接到服务器，无法发送命令');
            if (this.uiManager) {
                this.uiManager.showNotification('未连接到服务器', 'error');
            }
            return;
        }
        
        const command = {
            action: action,
            data: data,
            timestamp: Date.now()
        };
        
        console.log('[GameClient] 发送命令:', command);
        this.ws.send(JSON.stringify(command));
    }
    
    /**
     * 发送对话选择
     */
    sendDialogueChoice(optionId) {
        if (!this.isConnected()) {
            console.warn('[GameClient] 未连接到服务器，无法发送对话选择');
            return;
        }
        
        const choice = {
            optionId: optionId,
            timestamp: Date.now()
        };
        
        console.log('[GameClient] 发送对话选择:', choice);
        this.ws.send(JSON.stringify(choice));
    }
    
    /**
     * 检查是否已连接
     */
    isConnected() {
        return this.isConnectedFlag && this.ws && this.ws.readyState === WebSocket.OPEN;
    }
    
    /**
     * 断开连接
     */
    disconnect() {
        if (this.ws) {
            this.ws.close();
        }
    }
    
    /**
     * 更新连接状态显示
     */
    updateConnectionStatus(status, type) {
        const statusElement = document.getElementById('connection-status');
        if (statusElement) {
            const statusText = statusElement.querySelector('.status-text');
            if (statusText) {
                statusText.textContent = status;
            }
            
            // 更新样式类
            statusElement.className = `connection-status ${type}`;
        }
        
        // 更新重连按钮
        const reconnectBtn = document.getElementById('reconnect-btn');
        if (reconnectBtn) {
            reconnectBtn.disabled = this.isConnected();
            reconnectBtn.textContent = this.isConnected() ? '已连接' : '重新连接';
        }
    }
    
    /**
     * 尝试重新连接
     */
    attemptReconnect() {
        if (this.reconnectAttempts < this.maxReconnectAttempts) {
            this.reconnectAttempts++;
            const delay = 2000 * this.reconnectAttempts; // 递增延迟
            
            console.log(`[GameClient] ${delay/1000}秒后尝试重连... (${this.reconnectAttempts}/${this.maxReconnectAttempts})`);
            this.updateConnectionStatus(`${delay/1000}秒后重连...`, 'reconnecting');
            
            setTimeout(() => {
                if (!this.isConnected()) {
                    this.connect();
                }
            }, delay);
        } else {
            console.error('[GameClient] 重连失败，请手动刷新页面');
            this.updateConnectionStatus('连接失败', 'failed');
            
            if (this.uiManager) {
                this.uiManager.showNotification('连接失败，请刷新页面重试', 'error');
            }
        }
    }
    
    /**
     * 处理连接错误
     */
    handleConnectionError() {
        this.updateConnectionStatus('连接错误', 'error');
        
        if (this.uiManager) {
            this.uiManager.showNotification('连接服务器时发生错误', 'error');
        }
    }
}

// 导出到全局
window.GameClient = GameClient;
console.log('[GameClient] 游戏客户端模块已加载');
