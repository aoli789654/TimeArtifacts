/**
 * EventManager.cpp
 * 
 * 事件管理器实现
 * 
 * 【实现重点】：
 * 1. 线程安全的订阅者管理
 * 2. 高效的事件分发机制
 * 3. 优先级队列和排序算法
 * 4. 异常安全的事件处理
 * 5. 详细的调试和统计功能
 */

#include "EventManager.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

EventManager::EventManager(int maxQueue)
    : processingEvents(false)
    , debugMode(false)
    , maxQueueSize(maxQueue) {
    
    std::cout << "[EventManager] 事件管理器已创建，最大队列大小: " << maxQueueSize << std::endl;
}

EventManager::~EventManager() {
    std::cout << "[EventManager] 正在销毁事件管理器..." << std::endl;
    
    // 清理队列
    clearEventQueue();
    
    // 清理订阅者
    {
        std::lock_guard<std::mutex> lock(subscriberMutex);
        int totalSubscribers = 0;
        for (const auto& pair : subscribers) {
            totalSubscribers += pair.second.size();
        }
        std::cout << "[EventManager] 清理 " << totalSubscribers << " 个订阅者" << std::endl;
        subscribers.clear();
    }
    
    // 输出统计信息
    if (!eventCounts.empty()) {
        std::cout << "[EventManager] 事件处理统计：" << std::endl;
        for (const auto& pair : eventCounts) {
            std::cout << "  " << pair.first << ": " << pair.second << " 次" << std::endl;
        }
    }
    
    std::cout << "[EventManager] 事件管理器已销毁" << std::endl;
}

// =================================================================
// 订阅管理
// =================================================================

std::string EventManager::subscribe(const std::string& eventType, 
                                   EventCallback callback,
                                   const std::string& subscriberId,
                                   int priority) {
    if (!callback) {
        std::cerr << "[EventManager] 错误: 尝试订阅空回调函数" << std::endl;
        return "";
    }
    
    // 生成订阅者ID
    std::string finalId = subscriberId.empty() ? generateSubscriberId(eventType) : subscriberId;
    
    // 创建订阅者
    auto subscriber = std::make_shared<Subscriber>(callback, finalId, priority);
    
    {
        std::lock_guard<std::mutex> lock(subscriberMutex);
        
        // 检查是否已存在相同ID的订阅者
        auto& eventSubscribers = subscribers[eventType];
        auto it = std::find_if(eventSubscribers.begin(), eventSubscribers.end(),
            [&finalId](const std::shared_ptr<Subscriber>& sub) {
                return sub->subscriberId == finalId;
            });
        
        if (it != eventSubscribers.end()) {
            std::cout << "[EventManager] 警告: 订阅者 " << finalId 
                      << " 已存在，将替换现有订阅" << std::endl;
            *it = subscriber;
        } else {
            eventSubscribers.push_back(subscriber);
        }
        
        // 按优先级排序
        sortSubscribersByPriority(eventSubscribers);
    }
    
    if (debugMode) {
        std::cout << "[EventManager] 新增订阅: " << finalId 
                  << " → " << eventType << " (优先级: " << priority << ")" << std::endl;
    }
    
    return finalId;
}

bool EventManager::unsubscribe(const std::string& eventType, const std::string& subscriberId) {
    std::lock_guard<std::mutex> lock(subscriberMutex);
    
    auto it = subscribers.find(eventType);
    if (it == subscribers.end()) {
        return false;
    }
    
    auto& eventSubscribers = it->second;
    auto subIt = std::remove_if(eventSubscribers.begin(), eventSubscribers.end(),
        [&subscriberId](const std::shared_ptr<Subscriber>& sub) {
            return sub->subscriberId == subscriberId;
        });
    
    bool removed = (subIt != eventSubscribers.end());
    eventSubscribers.erase(subIt, eventSubscribers.end());
    
    // 如果没有订阅者了，移除整个事件类型
    if (eventSubscribers.empty()) {
        subscribers.erase(it);
    }
    
    if (debugMode && removed) {
        std::cout << "[EventManager] 取消订阅: " << subscriberId 
                  << " ← " << eventType << std::endl;
    }
    
    return removed;
}

void EventManager::unsubscribeAll(const std::string& subscriberId) {
    std::lock_guard<std::mutex> lock(subscriberMutex);
    
    int removedCount = 0;
    for (auto it = subscribers.begin(); it != subscribers.end();) {
        auto& eventSubscribers = it->second;
        
        auto subIt = std::remove_if(eventSubscribers.begin(), eventSubscribers.end(),
            [&subscriberId](const std::shared_ptr<Subscriber>& sub) {
                return sub->subscriberId == subscriberId;
            });
        
        if (subIt != eventSubscribers.end()) {
            removedCount += std::distance(subIt, eventSubscribers.end());
            eventSubscribers.erase(subIt, eventSubscribers.end());
        }
        
        // 如果没有订阅者了，移除整个事件类型
        if (eventSubscribers.empty()) {
            it = subscribers.erase(it);
        } else {
            ++it;
        }
    }
    
    if (debugMode && removedCount > 0) {
        std::cout << "[EventManager] 取消所有订阅: " << subscriberId 
                  << " (共 " << removedCount << " 个)" << std::endl;
    }
}

void EventManager::setSubscriberActive(const std::string& subscriberId, bool active) {
    std::lock_guard<std::mutex> lock(subscriberMutex);
    
    for (auto& pair : subscribers) {
        for (auto& subscriber : pair.second) {
            if (subscriber->subscriberId == subscriberId) {
                subscriber->active = active;
                if (debugMode) {
                    std::cout << "[EventManager] 订阅者 " << subscriberId 
                              << (active ? " 已激活" : " 已暂停") << std::endl;
                }
            }
        }
    }
}

// =================================================================
// 事件发布
// =================================================================

void EventManager::publishImmediate(std::unique_ptr<Event> event) {
    if (!event) {
        std::cerr << "[EventManager] 错误: 尝试发布空事件" << std::endl;
        return;
    }
    
    if (debugMode) {
        logEvent(*event, "立即发布");
    }
    
    // 更新统计
    eventCounts[event->getType()]++;
    
    // 立即分发
    dispatchEvent(*event);
}

void EventManager::publish(std::unique_ptr<Event> event) {
    if (!event) {
        std::cerr << "[EventManager] 错误: 尝试发布空事件" << std::endl;
        return;
    }
    
    if (debugMode) {
        logEvent(*event, "异步发布");
    }
    
    // 检查队列大小
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (static_cast<int>(eventQueue.size()) >= maxQueueSize) {
            std::cerr << "[EventManager] 警告: 事件队列已满，丢弃事件: " 
                      << event->getType() << std::endl;
            return;
        }
        
        // 加入队列
        eventQueue.emplace(std::move(event), false);
    }
}

void EventManager::publishBatch(std::vector<std::unique_ptr<Event>> events) {
    if (events.empty()) {
        return;
    }
    
    if (debugMode) {
        std::cout << "[EventManager] 批量发布 " << events.size() << " 个事件" << std::endl;
    }
    
    std::lock_guard<std::mutex> lock(queueMutex);
    
    for (auto& event : events) {
        if (event && static_cast<int>(eventQueue.size()) < maxQueueSize) {
            eventQueue.emplace(std::move(event), false);
        }
    }
}

// =================================================================
// 事件处理
// =================================================================

int EventManager::processEvents(int maxEvents) {
    if (processingEvents) {
        std::cerr << "[EventManager] 警告: 已在处理事件，避免重入" << std::endl;
        return 0;
    }
    
    processingEvents = true;
    int processedCount = 0;
    
    try {
        while (processedCount != maxEvents) {
            std::unique_ptr<Event> event;
            
            // 从队列取出事件
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (eventQueue.empty()) {
                    break;
                }
                
                event = std::move(eventQueue.front().event);
                eventQueue.pop();
            }
            
            if (event) {
                // 更新统计
                eventCounts[event->getType()]++;
                
                if (debugMode) {
                    logEvent(*event, "队列处理");
                }
                
                // 分发事件
                dispatchEvent(*event);
                processedCount++;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[EventManager] 事件处理异常: " << e.what() << std::endl;
    }
    
    processingEvents = false;
    
    if (debugMode && processedCount > 0) {
        std::cout << "[EventManager] 处理了 " << processedCount << " 个事件" << std::endl;
    }
    
    return processedCount;
}

void EventManager::clearEventQueue() {
    std::lock_guard<std::mutex> lock(queueMutex);
    
    int queueSize = eventQueue.size();
    while (!eventQueue.empty()) {
        eventQueue.pop();
    }
    
    if (debugMode && queueSize > 0) {
        std::cout << "[EventManager] 清空事件队列，丢弃 " << queueSize << " 个事件" << std::endl;
    }
}

// =================================================================
// 查询和统计
// =================================================================

int EventManager::getSubscriberCount(const std::string& eventType) const {
    std::lock_guard<std::mutex> lock(subscriberMutex);
    
    if (eventType.empty()) {
        int total = 0;
        for (const auto& pair : subscribers) {
            total += pair.second.size();
        }
        return total;
    }
    
    auto it = subscribers.find(eventType);
    return (it != subscribers.end()) ? it->second.size() : 0;
}

int EventManager::getQueueSize() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return eventQueue.size();
}

std::map<std::string, int> EventManager::getEventStatistics() const {
    return eventCounts;
}

void EventManager::resetStatistics() {
    eventCounts.clear();
    if (debugMode) {
        std::cout << "[EventManager] 统计数据已重置" << std::endl;
    }
}

bool EventManager::hasSubscribers(const std::string& eventType) const {
    std::lock_guard<std::mutex> lock(subscriberMutex);
    auto it = subscribers.find(eventType);
    return it != subscribers.end() && !it->second.empty();
}

// =================================================================
// 调试和配置
// =================================================================

void EventManager::setDebugMode(bool enabled) {
    debugMode = enabled;
    std::cout << "[EventManager] 调试模式: " << (enabled ? "开启" : "关闭") << std::endl;
}

void EventManager::addEventFilter(const std::string& eventType) {
    eventFilters.push_back(eventType);
    if (debugMode) {
        std::cout << "[EventManager] 添加事件过滤器: " << eventType << std::endl;
    }
}

void EventManager::removeEventFilter(const std::string& eventType) {
    auto it = std::find(eventFilters.begin(), eventFilters.end(), eventType);
    if (it != eventFilters.end()) {
        eventFilters.erase(it);
        if (debugMode) {
            std::cout << "[EventManager] 移除事件过滤器: " << eventType << std::endl;
        }
    }
}

void EventManager::clearEventFilters() {
    eventFilters.clear();
    if (debugMode) {
        std::cout << "[EventManager] 清空所有事件过滤器" << std::endl;
    }
}

void EventManager::setMaxQueueSize(int maxSize) {
    maxQueueSize = maxSize;
    std::cout << "[EventManager] 设置最大队列大小: " << maxSize << std::endl;
}

// =================================================================
// 私有方法
// =================================================================

void EventManager::dispatchEvent(const Event& event) {
    const std::string& eventType = event.getType();
    
    // 检查过滤器
    if (!passesFilter(eventType)) {
        return;
    }
    
    std::vector<std::shared_ptr<Subscriber>> eventSubscribers;
    
    // 获取订阅者副本（避免长时间持锁）
    {
        std::lock_guard<std::mutex> lock(subscriberMutex);
        auto it = subscribers.find(eventType);
        if (it != subscribers.end()) {
            eventSubscribers = it->second;
        }
    }
    
    if (eventSubscribers.empty()) {
        if (debugMode) {
            std::cout << "[EventManager] 事件 " << eventType << " 没有订阅者" << std::endl;
        }
        return;
    }
    
    // 分发给所有活跃的订阅者
    for (const auto& subscriber : eventSubscribers) {
        if (!subscriber->active) {
            continue;
        }
        
        try {
            subscriber->callback(event);
            
            if (debugMode) {
                std::cout << "[EventManager] 事件分发: " << eventType 
                          << " → " << subscriber->subscriberId << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[EventManager] 订阅者 " << subscriber->subscriberId 
                      << " 处理事件 " << eventType << " 时发生异常: " << e.what() << std::endl;
        }
    }
}

void EventManager::sortSubscribersByPriority(std::vector<std::shared_ptr<Subscriber>>& subs) {
    std::sort(subs.begin(), subs.end(),
        [](const std::shared_ptr<Subscriber>& a, const std::shared_ptr<Subscriber>& b) {
            return a->priority < b->priority; // 数字越小优先级越高
        });
}

std::string EventManager::generateSubscriberId(const std::string& base) {
    static int counter = 0;
    std::ostringstream oss;
    oss << (base.empty() ? "Subscriber" : base) << "_" << (++counter);
    return oss.str();
}

bool EventManager::passesFilter(const std::string& eventType) const {
    if (eventFilters.empty()) {
        return true; // 无过滤器，通过所有事件
    }
    
    return std::find(eventFilters.begin(), eventFilters.end(), eventType) != eventFilters.end();
}

void EventManager::logEvent(const Event& event, const std::string& action) const {
    auto now = std::chrono::steady_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    std::cout << "[EventManager] [" << timestamp << "] " << action 
              << " - " << event.getType() 
              << " (优先级: " << event.getPriority() << ")" << std::endl;
}
