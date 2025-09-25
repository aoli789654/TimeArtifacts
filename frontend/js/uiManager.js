/**
 * UI管理器
 * 负责管理游戏界面的所有UI更新和交互
 */

class UIManager {
    constructor() {
        this.textSpeed = 5; // 文字显示速度 (1-10)
        this.isTyping = false;
        this.gameClient = null;
        
        console.log('[UIManager] UI管理器已创建');
    }
    
    /**
     * 设置游戏客户端引用
     */
    setGameClient(gameClient) {
        this.gameClient = gameClient;
    }
    
    /**
     * 连接建立时的处理
     */
    onConnectionEstablished() {
        console.log('[UIManager] 连接已建立，初始化UI');
        this.showNotification('已连接到游戏服务器', 'success');
        
        // 清除加载状态
        this.clearLoadingStates();
    }
    
    /**
     * 连接丢失时的处理
     */
    onConnectionLost() {
        console.log('[UIManager] 连接已丢失');
        this.showNotification('与服务器失去连接', 'warning');
        
        // 显示加载状态
        this.showLoadingStates();
    }
    
    /**
     * 更新游戏状态
     */
    updateGameState(gameState) {
        console.log('[UIManager] 更新游戏状态:', gameState);
        
        // 更新位置信息
        if (gameState.currentLocation) {
            this.updateLocation(gameState.currentLocation);
        }
        
        // 更新玩家属性
        if (gameState.playerAttributes) {
            this.updatePlayerAttributes(gameState.playerAttributes);
        }
        
        // 更新物品栏
        if (gameState.inventory) {
            this.updateInventory(gameState.inventory);
        }
        
        // 更新可用操作
        if (gameState.availableActions) {
            this.updateAvailableActions(gameState.availableActions);
        }
    }
    
    /**
     * 更新场景
     */
    updateScene(sceneData) {
        console.log('[UIManager] 更新场景:', sceneData);
        
        // 更新位置
        if (sceneData.location) {
            this.updateLocation(sceneData.location);
        }
        
        // 更新场景描述
        if (sceneData.description) {
            this.updateSceneDescription(sceneData.description);
        }
        
        // 添加到故事日志
        this.addToStoryLog('场景变化', sceneData.description);
    }
    
    /**
     * 显示对话
     */
    showDialogue(dialogueData) {
        console.log('[UIManager] 显示对话:', dialogueData);
        
        const modal = document.getElementById('dialogue-modal');
        const speakerName = document.getElementById('speaker-name');
        const dialogueText = document.getElementById('dialogue-text');
        const dialogueOptions = document.getElementById('dialogue-options');
        
        if (!modal || !speakerName || !dialogueText || !dialogueOptions) {
            console.error('[UIManager] 对话UI元素未找到');
            return;
        }
        
        // 设置说话者姓名
        speakerName.textContent = dialogueData.speaker || '未知';
        
        // 清空选项
        dialogueOptions.innerHTML = '';
        
        // 显示对话框
        modal.classList.remove('hidden');
        
        // 使用打字机效果显示文本
        this.typewriterText(dialogueText, dialogueData.text || '', () => {
            // 文本显示完成后，显示选项
            this.showDialogueOptions(dialogueOptions, dialogueData.options || []);
        });
        
        // 添加到故事日志
        this.addToStoryLog(dialogueData.speaker, dialogueData.text);
    }
    
    /**
     * 显示对话选项
     */
    showDialogueOptions(container, options) {
        options.forEach((option, index) => {
            const button = document.createElement('button');
            button.className = 'dialogue-option';
            button.innerHTML = `<span class="option-number">${index + 1}</span> ${option.text}`;
            
            // 检查是否满足要求
            if (option.requirement) {
                button.classList.add('requirement');
                button.title = `需要: ${option.requirement}`;
            }
            
            button.addEventListener('click', () => {
                this.selectDialogueOption(option.id);
            });
            
            container.appendChild(button);
        });
    }
    
    /**
     * 选择对话选项
     */
    selectDialogueOption(optionId) {
        console.log('[UIManager] 选择对话选项:', optionId);
        
        // 关闭对话框
        const modal = document.getElementById('dialogue-modal');
        if (modal) {
            modal.classList.add('hidden');
        }
        
        // 发送选择到服务器
        if (this.gameClient) {
            this.gameClient.sendDialogueChoice(optionId);
        }
    }
    
    /**
     * 显示检查结果
     */
    showExaminationResult(examData) {
        console.log('[UIManager] 显示检查结果:', examData);
        
        // 更新场景描述
        this.updateSceneDescription(examData.description);
        
        // 显示属性增长
        if (examData.attributeGain) {
            for (const [attr, gain] of Object.entries(examData.attributeGain)) {
                this.showNotification(`${attr} +${gain}`, 'success');
            }
        }
        
        // 显示新洞察
        if (examData.newInsights && examData.newInsights.length > 0) {
            examData.newInsights.forEach(insight => {
                this.showNotification(`获得新洞察: ${insight}`, 'info');
            });
        }
        
        // 添加到故事日志
        this.addToStoryLog('检查', examData.description);
    }
    
    /**
     * 更新位置显示
     */
    updateLocation(location) {
        const locationName = document.getElementById('location-name');
        const locationSubtitle = document.getElementById('location-subtitle');
        
        if (locationName) {
            locationName.textContent = this.getLocationDisplayName(location);
        }
        
        if (locationSubtitle) {
            locationSubtitle.textContent = location;
        }
    }
    
    /**
     * 更新场景描述
     */
    updateSceneDescription(description) {
        const sceneDescription = document.getElementById('scene-description');
        if (sceneDescription) {
            this.typewriterText(sceneDescription, description);
        }
    }
    
    /**
     * 更新玩家属性
     */
    updatePlayerAttributes(attributes) {
        const container = document.getElementById('player-attributes');
        if (!container) return;
        
        container.innerHTML = '';
        
        const attributeNames = {
            'observation': '观察力',
            'communication': '沟通力',
            'action': '行动力',
            'empathy': '共情值'
        };
        
        for (const [key, value] of Object.entries(attributes)) {
            const attrDiv = document.createElement('div');
            attrDiv.className = 'attribute-item';
            
            const name = attributeNames[key] || key;
            attrDiv.innerHTML = `
                <div class="attribute-info">
                    <span class="attribute-name">${name}</span>
                    <span class="attribute-value">${value}</span>
                </div>
                <div class="attribute-bar">
                    <div class="attribute-fill" style="width: ${Math.min(value * 10, 100)}%"></div>
                </div>
            `;
            
            container.appendChild(attrDiv);
        }
    }
    
    /**
     * 更新物品栏
     */
    updateInventory(inventory) {
        const container = document.querySelector('.inventory-items');
        if (!container) return;
        
        container.innerHTML = '';
        
        if (inventory.length === 0) {
            container.innerHTML = '<div class="empty-inventory">暂无物品</div>';
            return;
        }
        
        inventory.forEach(item => {
            const itemDiv = document.createElement('div');
            itemDiv.className = 'inventory-item';
            itemDiv.textContent = this.getItemDisplayName(item);
            itemDiv.title = `点击查看 ${item}`;
            
            itemDiv.addEventListener('click', () => {
                this.examineItem(item);
            });
            
            container.appendChild(itemDiv);
        });
    }
    
    /**
     * 更新可用操作
     */
    updateAvailableActions(actions) {
        const container = document.getElementById('available-actions');
        if (!container) return;
        
        container.innerHTML = '';
        
        actions.forEach(action => {
            const button = document.createElement('button');
            button.className = 'action-button';
            button.textContent = this.getActionDisplayName(action);
            
            button.addEventListener('click', () => {
                this.executeAction(action);
            });
            
            container.appendChild(button);
        });
    }
    
    /**
     * 执行操作
     */
    executeAction(action) {
        console.log('[UIManager] 执行操作:', action);
        
        if (!this.gameClient) {
            this.showNotification('游戏客户端未连接', 'error');
            return;
        }
        
        // 解析操作
        if (action.startsWith('examine_')) {
            const target = action.replace('examine_', '');
            this.gameClient.sendCommand('examine', { target: target });
        } else if (action.startsWith('talk_to_')) {
            const target = action.replace('talk_to_', '');
            this.gameClient.sendCommand('talk', { target: target });
        } else if (action.startsWith('move_')) {
            const direction = action.replace('move_', '');
            this.gameClient.sendCommand('move', { direction: direction });
        } else {
            // 通用操作
            this.gameClient.sendCommand(action, {});
        }
    }
    
    /**
     * 检查物品
     */
    examineItem(item) {
        if (this.gameClient) {
            this.gameClient.sendCommand('examine', { target: item });
        }
    }
    
    /**
     * 打字机效果显示文本
     */
    async typewriterText(element, text, callback = null) {
        if (!element) return;
        
        this.isTyping = true;
        element.innerHTML = '';
        
        const speed = Math.max(1, 11 - this.textSpeed) * 10; // 转换速度
        
        for (let i = 0; i < text.length; i++) {
            if (!this.isTyping) break; // 如果被中断
            
            element.innerHTML += text.charAt(i);
            await new Promise(resolve => setTimeout(resolve, speed));
        }
        
        this.isTyping = false;
        
        if (callback) {
            callback();
        }
    }
    
    /**
     * 设置文字速度
     */
    setTextSpeed(speed) {
        this.textSpeed = Math.max(1, Math.min(10, speed));
        console.log('[UIManager] 文字速度设置为:', this.textSpeed);
    }
    
    /**
     * 显示通知
     */
    showNotification(message, type = 'info', duration = null) {
        const container = document.getElementById('notifications');
        if (!container) {
            console.log(`[${type.toUpperCase()}] ${message}`);
            return;
        }
        
        const notification = document.createElement('div');
        notification.className = `notification ${type}`;
        notification.innerHTML = `
            <div class="notification-content">
                <span class="notification-message">${message}</span>
                <button class="notification-close">&times;</button>
            </div>
        `;
        
        // 关闭按钮
        const closeBtn = notification.querySelector('.notification-close');
        closeBtn.addEventListener('click', () => {
            notification.remove();
        });
        
        container.appendChild(notification);
        
        // 自动消失
        const autoDuration = duration || (type === 'error' ? 5000 : 3000);
        setTimeout(() => {
            if (notification.parentNode) {
                notification.remove();
            }
        }, autoDuration);
    }
    
    /**
     * 添加到故事日志
     */
    addToStoryLog(speaker, content) {
        const logContainer = document.getElementById('story-log');
        if (!logContainer) return;
        
        const logEntry = document.createElement('div');
        logEntry.className = 'story-log-entry';
        logEntry.innerHTML = `
            <div class="log-speaker">${speaker}</div>
            <div class="log-content">${content}</div>
            <div class="log-time">${new Date().toLocaleTimeString()}</div>
        `;
        
        logContainer.appendChild(logEntry);
        logContainer.scrollTop = logContainer.scrollHeight;
    }
    
    /**
     * 清除加载状态
     */
    clearLoadingStates() {
        const loadingElements = document.querySelectorAll('.loading-placeholder, .loading-text');
        loadingElements.forEach(el => {
            el.textContent = '等待游戏数据...';
        });
    }
    
    /**
     * 显示加载状态
     */
    showLoadingStates() {
        const loadingElements = document.querySelectorAll('.loading-placeholder');
        loadingElements.forEach(el => {
            el.textContent = '连接中...';
        });
    }
    
    /**
     * 获取位置显示名称
     */
    getLocationDisplayName(location) {
        const names = {
            'bookstore': '时光角落书店',
            'old_street': '古老街道',
            'harbor': '旧港码头'
        };
        return names[location] || location;
    }
    
    /**
     * 获取物品显示名称
     */
    getItemDisplayName(item) {
        const names = {
            'old_letter': '旧信件',
            'mysterious_key': '神秘钥匙',
            'old_diary': '古旧日记'
        };
        return names[item] || item;
    }
    
    /**
     * 获取操作显示名称
     */
    getActionDisplayName(action) {
        const names = {
            'examine_bookshelf': '检查书架',
            'talk_to_owner': '与店主交谈',
            'look_around': '环顾四周',
            'examine_street_lamp': '查看路灯',
            'enter_bookstore': '进入书店',
            'walk_to_harbor': '前往港口'
        };
        return names[action] || action;
    }
}

// 导出到全局
window.UIManager = UIManager;
console.log('[UIManager] UI管理器模块已加载');
