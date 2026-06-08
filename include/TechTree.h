#pragma once

#include <raylib.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include "DataManager.h"

// Daftar skill yang tersedia di Tech Tree
enum SkillName {
    BARRIER,        // Shield 1 hantaman — skill dasar (root)
    AURA_FIELD,     // Shield aktif berdasarkan waktu
    SHOCKWAVE,      // Hancurkan asteroid sekitar
    CHRONO_STASIS,  // Perlambat gerakan asteroid
    INSTANT_CRIT,   // Ketik huruf pertama untuk hancurkan
    SCORE_BOOSTER   // Multiplier 16x score
};

// Status UI untuk setiap node skill
enum SkillState {
    LOCKED,      // Tidak bisa diakses — parent belum di-unlock
    AVAILABLE,   // Bisa dibeli/di-unlock — parent sudah di-unlock
    UNLOCKED     // Sudah aktif — sudah dibeli
};

// Data lengkap satu node skill
struct SkillData {
    SkillName name;          // Identitas skill
    int rpCost;              // Biaya Research Point untuk unlock
    std::string desc;        // Deskripsi singkat skill
    bool isUnlocked;         // Status unlock sebenarnya (dari save file)

    // UI Data untuk Raylib
    Vector2 position;        // Titik tengah node — untuk menggambar garis antar skill
    Rectangle bounds;        // Area hitbox — untuk mendeteksi klik/hover mouse
    SkillState uiState;      // Status tampilan (LOCKED/AVAILABLE/UNLOCKED) — dihitung otomatis
};

// Tech Tree system — mengelola skill, dependensi, dan state-nya
class TechTree {
private:
    std::unordered_map<SkillName, SkillData> skills;                        // Semua skill dan datanya
    std::unordered_map<SkillName, std::vector<SkillName>> adjList;          // Graf dependensi parent → child

    // Perbarui uiState semua skill berdasarkan isUnlocked + BFS dari BARRIER
    void updateSkillStates();

public:
    TechTree();

    // Tambah dependensi: parent harus di-unlock sebelum child bisa AVAILABLE
    void addDependency(SkillName parent, SkillName child);

    // Konversi string ↔ SkillName (untuk serialisasi JSON)
    static SkillName SkillNameFromString(const std::string& str);
    static std::string StringFromSkillName(SkillName name);

    // Load/save status skill dari/ke PlayerProfile (persistensi)
    void loadFromProfile(const PlayerProfile& profile);
    void saveToProfile(PlayerProfile& profile);

    // Unlock skill: kurangi RP, update state, return true jika berhasil
    bool unlockSkill(SkillName name, PlayerProfile& profile);

    // Proses input mouse: deteksi klik pada node skill yang AVAILABLE
    bool handleInput(Vector2 mousePos, bool isMouseClicked, PlayerProfile& profile);

    // Getter untuk dibaca TechTreeUI
    const std::unordered_map<SkillName, SkillData>& getSkills() const;
    const std::unordered_map<SkillName, std::vector<SkillName>>& getAdjList() const;
};
