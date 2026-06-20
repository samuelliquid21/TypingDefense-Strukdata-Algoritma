#pragma once
#include "raylib.h"
#include "DataManager.h"
#include <string>

class LoginScreen {
public:
    LoginScreen();
    void Update();
    void Draw();
    void Reset();
    bool ShouldLogin() const;        // return true saat Enter ditekan + valid
    bool ShouldGoToRegister() const; // return true saat klik "Daftar"
    bool ShouldGoBack() const;       // return true saat klik "Kembali"
    const PlayerProfile& GetProfile() const; // return profile yang berhasil login

private:
    char inputBuffer[32];
    int inputLen;
    std::string statusMessage;
    PlayerProfile loggedInProfile;
    bool shouldLogin;
    bool goToRegister;
    bool goBack;
};