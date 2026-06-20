#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>

class AssetManager {
public:
    static AssetManager& getInstance();

    Texture2D loadTexture(const std::string& key, const std::string& filepath);
    Texture2D getTexture(const std::string& key);
    bool hasTexture(const std::string& key) const;

    void registerSpriteSheet(const std::string& key, const std::string& filepath,
                             int columns, int rows);
    Rectangle getSpriteFrame(const std::string& key, int col, int row) const;
    Vector2 getSpriteFrameSize(const std::string& key) const;

    Sound loadSound(const std::string& key, const std::string& filepath);
    Sound getSound(const std::string& key);
    bool hasSound(const std::string& key) const;

    Music loadMusic(const std::string& key, const std::string& filepath);
    Music getMusic(const std::string& key);
    bool hasMusic(const std::string& key) const;

    void unloadAll();
    void unloadTextures();
    void unloadSounds();
    void unloadMusic();

private:
    AssetManager() = default;
    ~AssetManager() = default;
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    struct SheetInfo { int columns, rows; float frameWidth, frameHeight; };

    std::unordered_map<std::string, Texture2D> m_textures;
    std::unordered_map<std::string, Sound>     m_sounds;
    std::unordered_map<std::string, Music>     m_musics;
    std::unordered_map<std::string, SheetInfo> m_sheets;
};
