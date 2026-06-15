#pragma once
#include "raylib.h"
#include "HashMap.h"
#include <string>

// =============================================
// AssetManager - Modul penyimpanan asset
// menggunakan HashMap custom (chaining + rehashing)
//
// Menyimpan:
//   - Texture2D  → key: nama string
//   - Sound      → key: nama string
//   - Music      → key: nama string
//
// Singleton pattern — akses via AssetManager::Get()
// =============================================

class AssetManager {
public:

    static AssetManager& Get() {
        static AssetManager instance;
        return instance;
    }

    // =============================================
    // TEXTURE
    // =============================================

    Texture2D& LoadTex(const std::string& key, const std::string& path) {
        if (!textures.contains(key))
            textures.put(key, LoadTexture(path.c_str()));
        return *textures.get(key);
    }

    Texture2D& GetTex(const std::string& key) {
        return *textures.get(key);
    }

    bool HasTex(const std::string& key) const {
        return textures.contains(key);
    }

    void UnloadTex(const std::string& key) {
        Texture2D* tex = textures.get(key);
        if (tex) {
            UnloadTexture(*tex);
            textures.remove(key);
        }
    }

    // =============================================
    // SOUND
    // =============================================

    Sound& LoadSnd(const std::string& key, const std::string& path) {
        if (!sounds.contains(key))
            sounds.put(key, LoadSound(path.c_str()));
        return *sounds.get(key);
    }

    Sound& GetSnd(const std::string& key) {
        return *sounds.get(key);
    }

    bool HasSnd(const std::string& key) const {
        return sounds.contains(key);
    }

    void PlaySnd(const std::string& key) {
        Sound* snd = sounds.get(key);
        if (snd) PlaySound(*snd);
    }

    void StopSnd(const std::string& key) {
        Sound* snd = sounds.get(key);
        if (snd) StopSound(*snd);
    }

    void UnloadSnd(const std::string& key) {
        Sound* snd = sounds.get(key);
        if (snd) {
            UnloadSound(*snd);
            sounds.remove(key);
        }
    }

    // =============================================
    // MUSIC
    // =============================================

    Music& LoadMus(const std::string& key, const std::string& path) {
        if (!music.contains(key))
            music.put(key, LoadMusicStream(path.c_str()));
        return *music.get(key);
    }

    Music& GetMus(const std::string& key) {
        return *music.get(key);
    }

    bool HasMus(const std::string& key) const {
        return music.contains(key);
    }

    // =============================================
    // UNLOAD SEMUA ASSET
    // =============================================

    void UnloadAll() {
        for (const auto& k : textures.keys()) {
            Texture2D* tex = textures.get(k);
            if (tex) UnloadTexture(*tex);
        }
        for (const auto& k : sounds.keys()) {
            Sound* snd = sounds.get(k);
            if (snd) UnloadSound(*snd);
        }
        for (const auto& k : music.keys()) {
            Music* mus = music.get(k);
            if (mus) UnloadMusicStream(*mus);
        }
        textures.clear();
        sounds.clear();
        music.clear();
    }

    void PrintAll() const {
        TraceLog(LOG_INFO, "=== AssetManager Contents ===");
        for (const auto& k : textures.keys())
            TraceLog(LOG_INFO, "  TEX : %s", k.c_str());
        for (const auto& k : sounds.keys())
            TraceLog(LOG_INFO, "  SND : %s", k.c_str());
        for (const auto& k : music.keys())
            TraceLog(LOG_INFO, "  MUS : %s", k.c_str());
    }

private:
    AssetManager() = default;
    ~AssetManager() { UnloadAll(); }

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    HashMap<std::string, Texture2D> textures;
    HashMap<std::string, Sound>     sounds;
    HashMap<std::string, Music>     music;
};
