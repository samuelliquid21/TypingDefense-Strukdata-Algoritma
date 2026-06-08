#pragma once
#include "raylib.h"
#include "DataManager.h"
#include <string>

class RegisterScreen {
public:
    RegisterScreen();
    void Update();
    void Draw();
    void Reset();
    
    bool IsRegisterSuccess() const;
    bool ShouldGoToLogin() const;
    std::string GetRegisteredUsername() const;

private:
    char inputBuffer[32];
    int inputLen;
    std::string statusMessage;
    Color statusColor;
    
    bool isRegisterSuccess;
    bool goToLogin;
    std::string registeredUsername;
};