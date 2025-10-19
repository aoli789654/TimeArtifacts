@echo off
:: Set UTF-8 encoding to prevent Chinese character garbling
chcp 65001 >nul

:: Set console title
title Time Artifacts - Frontend Server

:: Clear screen
cls

echo ========================================
echo    Time Artifacts - Frontend Server
echo ========================================
echo.

:: Check if Python is installed
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Python not found
    echo Please install Python first
    pause
    exit /b 1
)

:: Navigate to frontend directory
cd /d "%~dp0frontend"
if %errorlevel% neq 0 (
    echo [ERROR] Frontend directory not found
    echo Expected path: %~dp0frontend
    pause
    exit /b 1
)

echo [INFO] Starting HTTP server on port 3000...
echo [INFO] Frontend will be available at: http://localhost:3000
echo [INFO] Press Ctrl+C to stop the server
echo.

:: Start Python HTTP server
python -m http.server 3000

:: If server stops, show message
echo.
echo [INFO] Frontend server stopped
pause
