/**
 * StateManager.cpp
 * 
 * 状态管理器实现
 * 
 * 【实现重点】：
 * 1. 延迟状态切换机制 - 避免在update过程中直接切换状态
 * 2. 状态栈管理 - 支持状态覆盖和恢复
 * 3. 安全的资源管理 - 确保状态切换时的内存安全
 * 4. 详细的日志记录 - 便于调试状态切换问题
 */

#include "StateManager.h"
#include <iostream>

StateManager::StateManager() 
    : currentState(nullptr)
    , nextState(nullptr)
    , shouldChangeState(false)
    , shouldPushState(false)
    , shouldPopState(false)
    , lastStateName("None") {
    
    std::cout << "[StateManager] 状态管理器已创建" << std::endl;
}

StateManager::~StateManager() {
    std::cout << "[StateManager] 正在销毁状态管理器..." << std::endl;
    
    // 清理所有状态
    cleanupCurrentState();
    
    // 清空状态栈
    while (!stateStack.empty()) {
        auto state = std::move(stateStack.top());
        stateStack.pop();
        if (state) {
            std::cout << "[StateManager] 清理栈中状态: " << state->getName() << std::endl;
            state->exit();
        }
    }
    
    std::cout << "[StateManager] 状态管理器已销毁" << std::endl;
}

void StateManager::setInitialState(std::unique_ptr<GameState> initialState) {
    if (currentState) {
        std::cout << "[StateManager] 警告: 尝试设置初始状态，但当前已有状态: " 
                  << currentState->getName() << std::endl;
        return;
    }
    
    if (!initialState) {
        std::cout << "[StateManager] 错误: 尝试设置空的初始状态" << std::endl;
        return;
    }
    
    std::cout << "[StateManager] 设置初始状态: " << initialState->getName() << std::endl;
    
    currentState = std::move(initialState);
    lastStateName = currentState->getName();
    
    // 立即进入初始状态
    currentState->enter();
    
    std::cout << "[StateManager] 初始状态已激活" << std::endl;
}

void StateManager::changeState(std::unique_ptr<GameState> newState) {
    if (!newState) {
        std::cout << "[StateManager] 错误: 尝试切换到空状态" << std::endl;
        return;
    }
    
    std::cout << "[StateManager] 请求状态切换: ";
    if (currentState) {
        std::cout << currentState->getName() << " → ";
    } else {
        std::cout << "None → ";
    }
    std::cout << newState->getName() << std::endl;
    
    // 检查当前状态是否允许切换
    if (currentState && !currentState->canTransition()) {
        std::cout << "[StateManager] 状态切换被拒绝: 当前状态不允许切换" << std::endl;
        return;
    }
    
    // 设置延迟切换
    nextState = std::move(newState);
    shouldChangeState = true;
}

void StateManager::pushState(std::unique_ptr<GameState> overlayState) {
    if (!overlayState) {
        std::cout << "[StateManager] 错误: 尝试压入空状态" << std::endl;
        return;
    }
    
    std::cout << "[StateManager] 请求压入覆盖状态: " << overlayState->getName() << std::endl;
    
    if (currentState) {
        std::cout << "[StateManager] 当前状态 " << currentState->getName() 
                  << " 将被覆盖" << std::endl;
    }
    
    // 设置延迟压入
    nextState = std::move(overlayState);
    shouldPushState = true;
}

void StateManager::popState() {
    if (stateStack.empty()) {
        std::cout << "[StateManager] 警告: 尝试弹出状态，但状态栈为空" << std::endl;
        return;
    }
    
    std::cout << "[StateManager] 请求弹出当前状态";
    if (currentState) {
        std::cout << ": " << currentState->getName();
    }
    std::cout << std::endl;
    
    // 设置延迟弹出
    shouldPopState = true;
}

void StateManager::update(float deltaTime) {
    // 处理延迟的状态操作
    if (shouldChangeState) {
        performStateChange();
        shouldChangeState = false;
    }
    
    if (shouldPushState) {
        performStatePush();
        shouldPushState = false;
    }
    
    if (shouldPopState) {
        performStatePop();
        shouldPopState = false;
    }
    
    // 更新当前状态
    if (currentState) {
        try {
            currentState->update(deltaTime);
            
            // 检查状态是否有自动切换需求
            auto nextAutoState = currentState->getNextState();
            if (nextAutoState) {
                std::cout << "[StateManager] 状态 " << currentState->getName() 
                          << " 请求自动切换到 " << nextAutoState->getName() << std::endl;
                changeState(std::move(nextAutoState));
            }
            
        } catch (const std::exception& e) {
            std::cerr << "[StateManager] 状态更新异常: " << e.what() << std::endl;
        }
    }
}

void StateManager::render() {
    if (currentState) {
        try {
            currentState->render();
        } catch (const std::exception& e) {
            std::cerr << "[StateManager] 状态渲染异常: " << e.what() << std::endl;
        }
    }
}

void StateManager::handleInput(const std::string& input) {
    if (currentState) {
        try {
            currentState->handleInput(input);
        } catch (const std::exception& e) {
            std::cerr << "[StateManager] 状态输入处理异常: " << e.what() << std::endl;
        }
    } else {
        std::cout << "[StateManager] 警告: 收到输入但没有当前状态: " << input << std::endl;
    }
}

std::string StateManager::getCurrentStateName() const {
    if (currentState) {
        return currentState->getName();
    }
    return "None";
}

bool StateManager::hasCurrentState() const {
    return currentState != nullptr;
}

size_t StateManager::getStateStackDepth() const {
    return stateStack.size();
}

void StateManager::performStateChange() {
    if (!nextState) {
        std::cout << "[StateManager] 错误: 执行状态切换但没有目标状态" << std::endl;
        return;
    }
    
    std::cout << "[StateManager] 执行状态切换..." << std::endl;
    
    // 退出当前状态
    cleanupCurrentState();
    
    // 切换到新状态
    currentState = std::move(nextState);
    lastStateName = currentState->getName();
    
    // 进入新状态
    std::cout << "[StateManager] 进入新状态: " << currentState->getName() << std::endl;
    currentState->enter();
    
    std::cout << "[StateManager] 状态切换完成" << std::endl;
}

void StateManager::performStatePush() {
    if (!nextState) {
        std::cout << "[StateManager] 错误: 执行状态压入但没有目标状态" << std::endl;
        return;
    }
    
    std::cout << "[StateManager] 执行状态压入..." << std::endl;
    
    // 将当前状态压入栈中
    if (currentState) {
        std::cout << "[StateManager] 将状态压入栈: " << currentState->getName() << std::endl;
        stateStack.push(std::move(currentState));
    }
    
    // 设置新的当前状态
    currentState = std::move(nextState);
    lastStateName = currentState->getName();
    
    // 进入新状态
    std::cout << "[StateManager] 进入覆盖状态: " << currentState->getName() << std::endl;
    currentState->enter();
    
    std::cout << "[StateManager] 状态压入完成，栈深度: " << stateStack.size() << std::endl;
}

void StateManager::performStatePop() {
    if (stateStack.empty()) {
        std::cout << "[StateManager] 错误: 执行状态弹出但栈为空" << std::endl;
        return;
    }
    
    std::cout << "[StateManager] 执行状态弹出..." << std::endl;
    
    // 退出当前状态
    cleanupCurrentState();
    
    // 从栈中恢复状态
    currentState = std::move(stateStack.top());
    stateStack.pop();
    lastStateName = currentState->getName();
    
    std::cout << "[StateManager] 恢复状态: " << currentState->getName() 
              << "，栈深度: " << stateStack.size() << std::endl;
    
    // 注意：不需要调用enter()，因为之前的状态只是被覆盖，没有退出
    
    std::cout << "[StateManager] 状态弹出完成" << std::endl;
}

void StateManager::cleanupCurrentState() {
    if (currentState) {
        std::cout << "[StateManager] 清理状态: " << currentState->getName() << std::endl;
        
        try {
            currentState->exit();
        } catch (const std::exception& e) {
            std::cerr << "[StateManager] 状态退出异常: " << e.what() << std::endl;
        }
        
        currentState.reset();
    }
}
