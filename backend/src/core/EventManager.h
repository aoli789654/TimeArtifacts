/**
 * EventManager.h
 * 
 * 事件管理器 - 观察者模式的核心实现
 * 
 * 【文件作用】：
 * 1. 管理事件的订阅和取消订阅
 * 2. 负责事件的分发和路由
 * 3. 支持事件优先级和异步处理
 * 4. 提供事件过滤和调试功能
 * 
 * 【设计模式】：观察者模式 + 中介者模式
 * 【核心优势】：系统解耦、易于扩展、性能可控
 * 
 * 【使用示例】：
 * ```cpp
 * // 订阅属性变化事件
 * eventManager->subscribe("AttributeChanged", [this](const Event& e) {
 *     const auto& attrEvent = static_cast<const AttributeChangedEvent&>(e);
 *     updateUI(attrEvent.attributeName, attrEvent.newValue);
 * });
 * 
 * // 发布事件
 * auto event = std::make_unique<AttributeChangedEvent>("observation", 1, 2, "examination");
 * eventManager->publish(std::move(event));
 * ```
 */

#pragma once

#include "Events.h"
#include <functional>
#include <vector>
#include <map>
#include <queue>
#include <memory>
#include <mutex>
#include <string>

/**
 * 事件回调函数类型
 * 【说明】：所有订阅者都必须提供这种签名的回调函数
 * 【参数】：const Event& - 事件引用，使用多态访问具体事件信息
 */
using EventCallback = std::function<void(const Event&)>;

/**
 * 订阅者信息结构
 * 【作用】：存储订阅者的详细信息，便于管理和调试
 */
struct Subscriber {
    EventCallback callback;    // 回调函数
    std::string subscriberId;  // 订阅者唯一ID
    int priority;             // 订阅优先级（数字越小优先级越高）
    bool active;              // 是否激活
    
    Subscriber(EventCallback cb, const std::string& id, int prio = 5)
        : callback(cb), subscriberId(id), priority(prio), active(true) {}
};

/**
 * 事件队列项
 * 【作用】：用于异步事件处理的队列结构
 */
struct QueuedEvent {
    std::unique_ptr<Event> event;
    bool immediate;           // 是否立即处理
    
    QueuedEvent(std::unique_ptr<Event> e, bool imm = false)
        : event(std::move(e)), immediate(imm) {}
};

/**
 * 事件管理器类
 * 
 * 【核心功能】：
 * 1. 事件订阅和取消订阅
 * 2. 事件发布和分发
 * 3. 事件队列管理
 * 4. 订阅者优先级管理
 * 5. 事件过滤和统计
 */
class EventManager {
private:
    // 订阅者映射：事件类型 -> 订阅者列表
    std::map<std::string, std::vector<std::shared_ptr<Subscriber>>> subscribers;
    
    // 事件队列（用于异步处理）
    std::queue<QueuedEvent> eventQueue;
    
    // 线程安全
    mutable std::mutex subscriberMutex;
    mutable std::mutex queueMutex;
    
    // 事件统计
    std::map<std::string, int> eventCounts;
    
    // 事件过滤器（用于调试和性能优化）
    std::vector<std::string> eventFilters;
    
    // 管理器状态
    bool processingEvents;
    bool debugMode;
    int maxQueueSize;

public:
    /**
     * 构造函数
     * 【参数】：maxQueue - 最大队列大小，防止内存溢出
     */
    explicit EventManager(int maxQueue = 1000);
    
    /**
     * 析构函数
     * 【作用】：清理所有订阅者和队列中的事件
     */
    ~EventManager();
    
    // =================================================================
    // 订阅管理
    // =================================================================
    
    /**
     * 订阅事件
     * 【作用】：注册对特定类型事件的监听
     * 【参数】：
     *   - eventType: 事件类型名称（如"AttributeChanged"）
     *   - callback: 事件处理回调函数
     *   - subscriberId: 订阅者唯一标识
     *   - priority: 处理优先级（0-10，数字越小优先级越高）
     * 【返回】：订阅ID，用于后续取消订阅
     * 
     * 【使用示例】：
     * ```cpp
     * auto id = eventManager->subscribe("ItemAcquired", 
     *     [this](const Event& e) {
     *         const auto& itemEvent = static_cast<const ItemAcquiredEvent&>(e);
     *         playItemSound(itemEvent.itemName);
     *     }, "AudioSystem", 3);
     * ```
     */
    std::string subscribe(const std::string& eventType, 
                         EventCallback callback,
                         const std::string& subscriberId = "",
                         int priority = 5);
    
    /**
     * 取消订阅
     * 【作用】：移除特定订阅者对事件的监听
     * 【参数】：
     *   - eventType: 事件类型
     *   - subscriberId: 订阅者ID
     * 【返回】：是否成功取消
     */
    bool unsubscribe(const std::string& eventType, const std::string& subscriberId);
    
    /**
     * 取消订阅者的所有订阅
     * 【作用】：一次性清理某个系统的所有事件订阅
     * 【使用场景】：系统关闭时的资源清理
     */
    void unsubscribeAll(const std::string& subscriberId);
    
    /**
     * 暂停/恢复订阅者
     * 【作用】：临时禁用某个订阅者，不删除订阅关系
     */
    void setSubscriberActive(const std::string& subscriberId, bool active);
    
    // =================================================================
    // 事件发布
    // =================================================================
    
    /**
     * 立即发布事件（同步处理）
     * 【作用】：立即分发事件给所有订阅者
     * 【参数】：event - 要发布的事件（使用移动语义）
     * 【特点】：阻塞调用，事件处理完成后才返回
     * 【使用场景】：关键事件，需要确保立即处理
     */
    void publishImmediate(std::unique_ptr<Event> event);
    
    /**
     * 异步发布事件（加入队列）
     * 【作用】：将事件加入队列，在下次processEvents时处理
     * 【特点】：非阻塞调用，性能更好
     * 【使用场景】：一般事件，可以延迟处理
     */
    void publish(std::unique_ptr<Event> event);
    
    /**
     * 发布多个事件
     * 【作用】：批量发布事件，提高性能
     */
    void publishBatch(std::vector<std::unique_ptr<Event>> events);
    
    // =================================================================
    // 事件处理
    // =================================================================
    
    /**
     * 处理事件队列
     * 【作用】：处理队列中的所有异步事件
     * 【参数】：maxEvents - 最大处理事件数（0表示处理所有）
     * 【调用频率】：通常在主循环中每帧调用一次
     * 【返回】：实际处理的事件数量
     */
    int processEvents(int maxEvents = 0);
    
    /**
     * 清空事件队列
     * 【作用】：丢弃所有未处理的事件
     * 【使用场景】：场景切换、游戏重置等
     */
    void clearEventQueue();
    
    // =================================================================
    // 查询和统计
    // =================================================================
    
    /**
     * 获取订阅者数量
     * 【参数】：eventType - 特定事件类型，空字符串表示所有事件
     */
    int getSubscriberCount(const std::string& eventType = "") const;
    
    /**
     * 获取队列大小
     */
    int getQueueSize() const;
    
    /**
     * 获取事件统计
     * 【返回】：事件类型到处理次数的映射
     */
    std::map<std::string, int> getEventStatistics() const;
    
    /**
     * 重置统计数据
     */
    void resetStatistics();
    
    /**
     * 检查是否有订阅者
     */
    bool hasSubscribers(const std::string& eventType) const;
    
    // =================================================================
    // 调试和配置
    // =================================================================
    
    /**
     * 设置调试模式
     * 【作用】：启用详细的事件处理日志
     */
    void setDebugMode(bool enabled);
    
    /**
     * 添加事件过滤器
     * 【作用】：只处理指定类型的事件，用于调试
     */
    void addEventFilter(const std::string& eventType);
    
    /**
     * 移除事件过滤器
     */
    void removeEventFilter(const std::string& eventType);
    
    /**
     * 清空所有过滤器
     */
    void clearEventFilters();
    
    /**
     * 设置最大队列大小
     */
    void setMaxQueueSize(int maxSize);

private:
    /**
     * 分发事件给订阅者
     * 【内部方法】：核心事件分发逻辑
     */
    void dispatchEvent(const Event& event);
    
    /**
     * 对订阅者按优先级排序
     */
    void sortSubscribersByPriority(std::vector<std::shared_ptr<Subscriber>>& subs);
    
    /**
     * 生成唯一的订阅者ID
     */
    std::string generateSubscriberId(const std::string& base = "");
    
    /**
     * 检查事件是否通过过滤器
     */
    bool passesFilter(const std::string& eventType) const;
    
    /**
     * 记录事件处理日志
     */
    void logEvent(const Event& event, const std::string& action) const;
};
