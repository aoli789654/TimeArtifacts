/**
 * 主题管理器
 * 负责管理游戏界面主题
 */

class ThemeManager {
    constructor() {
        this.currentTheme = 'default';
        this.availableThemes = ['default', 'vintage', 'dark'];
        
        console.log('[ThemeManager] 主题管理器已创建');
    }
    
    /**
     * 设置主题
     */
    setTheme(themeName) {
        if (!this.availableThemes.includes(themeName)) {
            console.warn('[ThemeManager] 未知主题:', themeName);
            return;
        }
        
        console.log('[ThemeManager] 切换主题:', themeName);
        
        // 更新body的data-theme属性
        document.body.setAttribute('data-theme', themeName);
        
        this.currentTheme = themeName;
        
        // 保存到本地存储
        localStorage.setItem('timeartifacts_theme', themeName);
        
        // 触发主题变更事件
        this.onThemeChanged(themeName);
    }
    
    /**
     * 获取当前主题
     */
    getCurrentTheme() {
        return this.currentTheme;
    }
    
    /**
     * 主题变更回调
     */
    onThemeChanged(themeName) {
        console.log('[ThemeManager] 主题已变更为:', themeName);
        
        // 可以在这里添加主题变更的特殊处理
        // 比如更新某些动态样式、重新初始化动画等
    }
}

// 导出到全局
window.ThemeManager = ThemeManager;
console.log('[ThemeManager] 主题管理器模块已加载');
