#include "AssetManager.h"
#include "raylib.h"

AssetManager& AssetManager::getInstance() {
    static AssetManager instance;
    return instance;
}

// ===============================
// TEXTURES
// ===============================

Texture2D AssetManager::loadTexture(const std::string& key, const std::string& filepath) {
    auto it = m_textures.find(key);
    if (it != m_textures.end())
        return it->second;
    Texture2D tex = LoadTexture(filepath.c_str());
    m_textures[key] = tex;
    return tex;
}

Texture2D AssetManager::getTexture(const std::string& key) {
    auto it = m_textures.find(key);
    if (it != m_textures.end())
        return it->second;
    return Texture2D{0, 0, 0, 0};
}

bool AssetManager::hasTexture(const std::string& key) const {
    return m_textures.find(key) != m_textures.end();
}

// ===============================
// SPRITE SHEETS
// ===============================

void AssetManager::registerSpriteSheet(const std::string& key, const std::string& filepath,
                                        int columns, int rows) {
    Texture2D tex = loadTexture(key, filepath);
    float fw = (float)tex.width / columns;
    float fh = (float)tex.height / rows;
    m_sheets[key] = {columns, rows, fw, fh};
}

Rectangle AssetManager::getSpriteFrame(const std::string& key, int col, int row) const {
    auto it = m_sheets.find(key);
    if (it == m_sheets.end())
        return Rectangle{0, 0, 0, 0};
    return Rectangle{
        col * it->second.frameWidth,
        row * it->second.frameHeight,
        it->second.frameWidth,
        it->second.frameHeight
    };
}

// ===============================
// SOUNDS
// ===============================

Sound AssetManager::loadSound(const std::string& key, const std::string& filepath) {
    auto it = m_sounds.find(key);
    if (it != m_sounds.end())
        return it->second;
    Sound snd = LoadSound(filepath.c_str());
    m_sounds[key] = snd;
    return snd;
}

Sound AssetManager::getSound(const std::string& key) {
    auto it = m_sounds.find(key);
    if (it != m_sounds.end())
        return it->second;
    return Sound{0, 0, 0, 0};
}

bool AssetManager::hasSound(const std::string& key) const {
    return m_sounds.find(key) != m_sounds.end();
}

// ===============================
// MUSIC
// ===============================

Music AssetManager::loadMusic(const std::string& key, const std::string& filepath) {
    auto it = m_musics.find(key);
    if (it != m_musics.end())
        return it->second;
    Music mus = LoadMusicStream(filepath.c_str());
    m_musics[key] = mus;
    return mus;
}

Music AssetManager::getMusic(const std::string& key) {
    auto it = m_musics.find(key);
    if (it != m_musics.end())
        return it->second;
    return Music{0, 0, 0, 0};
}

bool AssetManager::hasMusic(const std::string& key) const {
    return m_musics.find(key) != m_musics.end();
}

// ===============================
// LIFECYCLE
// ===============================

void AssetManager::unloadAll() {
    unloadTextures();
    unloadSounds();
    unloadMusic();
}

void AssetManager::unloadTextures() {
    for (auto& [key, tex] : m_textures)
        UnloadTexture(tex);
    m_textures.clear();
    m_sheets.clear();
}

void AssetManager::unloadSounds() {
    for (auto& [key, snd] : m_sounds)
        UnloadSound(snd);
    m_sounds.clear();
}

void AssetManager::unloadMusic() {
    for (auto& [key, mus] : m_musics)
        UnloadMusicStream(mus);
    m_musics.clear();
}
