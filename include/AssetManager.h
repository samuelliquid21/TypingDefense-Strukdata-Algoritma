#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>

// Singleton manager untuk semua asset game (texture, sound, music).
// Menyimpan cache asset agar tidak perlu load ulang, dan menyediakan
// akses terpusat ke sprite sheet, sound effect, dan musik.
class AssetManager {
public:
    static AssetManager& getInstance();

    // === Texture API ===
    Texture2D loadTexture(const std::string& key, const std::string& filepath); // Load texture dari file + cache
    Texture2D getTexture(const std::string& key);    // Ambil texture dari cache
    bool hasTexture(const std::string& key) const;   // Cek apakah texture sudah diload

    // === Sprite Sheet API ===
    void registerSpriteSheet(const std::string& key, const std::string& filepath,
                             int columns, int rows); // Daftarkan sprite sheet grid
    Rectangle getSpriteFrame(const std::string& key, int col, int row) const; // Ambil frame tertentu dari sheet
    Vector2 getSpriteFrameSize(const std::string& key) const; // Ukuran tiap frame dalam sprite sheet

    // === Sound API ===
    Sound loadSound(const std::string& key, const std::string& filepath);
    Sound getSound(const std::string& key);
    bool hasSound(const std::string& key) const;

    // === Music API ===
    Music loadMusic(const std::string& key, const std::string& filepath);
    Music getMusic(const std::string& key);
    bool hasMusic(const std::string& key) const;

    // === Cleanup ===
    void unloadAll();
    void unloadTextures();
    void unloadSounds();
    void unloadMusic();

private:
    AssetManager() = default;
    ~AssetManager() = default;
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    // Informasi layout sprite sheet
    struct SheetInfo { int columns, rows; float frameWidth, frameHeight; };

    std::unordered_map<std::string, Texture2D> m_textures; // Cache texture
    std::unordered_map<std::string, Sound>     m_sounds;    // Cache sound
    std::unordered_map<std::string, Music>     m_musics;    // Cache music
    std::unordered_map<std::string, SheetInfo> m_sheets;    // Cache sprite sheet info
};
