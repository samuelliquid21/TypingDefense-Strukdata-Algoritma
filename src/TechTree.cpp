#include "TechTree.h"
#include <stdexcept>
#include "raylib.h"

// Konstruktor: inisialisasi semua skill dengan posisi, biaya, dan dependensi
TechTree::TechTree() {
    float boxW = 220.0f;
    float boxH = 75.0f;

    // Definisikan 5 skill dengan layout diamond
    skills = {
        {SkillName::AURA_FIELD,    {SkillName::AURA_FIELD, 100, "Shield aktif rentang waktu (10s)", false,
                                    {540.0f, 100.0f}, {540.0f - boxW/2, 100.0f - boxH/2, boxW, boxH}, LOCKED}},

        {SkillName::BARRIER,       {SkillName::BARRIER, 30, "Shield 1 hantaman", false,
                                    {540.0f, 240.0f}, {540.0f - boxW/2, 240.0f - boxH/2, boxW, boxH}, LOCKED}},

        {SkillName::SCORE_BOOSTER, {SkillName::SCORE_BOOSTER, 250, "Multiplier 16x score (10s)", false,
                                    {360.0f, 380.0f}, {360.0f - boxW/2, 380.0f - boxH/2, boxW, boxH}, LOCKED}},

        {SkillName::SHOCKWAVE,     {SkillName::SHOCKWAVE, 300, "Hancurkan asteroid sekitar", false,
                                    {720.0f, 380.0f}, {720.0f - boxW/2, 380.0f - boxH/2, boxW, boxH}, LOCKED}},

        {SkillName::INSTANT_CRIT,  {SkillName::INSTANT_CRIT, 500, "Huruf pertama hancurkan asteroid (10s)", false,
                                    {720.0f, 520.0f}, {720.0f - boxW/2, 520.0f - boxH/2, boxW, boxH}, LOCKED}}
    };

    // Struktur dependensi: AURA_FIELD di atas BARRIER, BARRIER root → 3 child
    addDependency(SkillName::BARRIER, SkillName::AURA_FIELD);
    addDependency(SkillName::BARRIER, SkillName::SCORE_BOOSTER);
    addDependency(SkillName::BARRIER, SkillName::SHOCKWAVE);
    addDependency(SkillName::SHOCKWAVE, SkillName::INSTANT_CRIT);

    // Hitung state awal: BARRIER jadi AVAILABLE, sisanya LOCKED
    updateSkillStates();
}

// Tambah edge parent → child di adjacency list
void TechTree::addDependency(SkillName parent, SkillName child) {
    adjList[parent].push_back(child);
}

// Konversi string (dari JSON) ke enum SkillName
SkillName TechTree::SkillNameFromString(const std::string& str) {
    if (str == "aura_field")    return AURA_FIELD;
    if (str == "barrier")       return BARRIER;
    if (str == "shockwave")     return SHOCKWAVE;
    if (str == "instant_crit")  return INSTANT_CRIT;
    if (str == "score_booster") return SCORE_BOOSTER;
    throw std::invalid_argument("TechTree: unknown skill name '" + str + "'");
}

// Konversi enum SkillName ke string (untuk serialisasi JSON)
std::string TechTree::StringFromSkillName(SkillName name) {
    switch (name) {
        case AURA_FIELD:    return "aura_field";
        case BARRIER:       return "barrier";
        case SHOCKWAVE:     return "shockwave";
        case INSTANT_CRIT:  return "instant_crit";
        case SCORE_BOOSTER: return "score_booster";
    }
    return "";
}

// Load status unlock dari PlayerProfile, reset semua lalu set sesuai data save
void TechTree::loadFromProfile(const PlayerProfile& profile) {
    // Reset semua skill ke LOCKED
    for (auto& [key, skill] : skills) {
        skill.isUnlocked = false;
    }

    // Set isUnlocked berdasarkan daftar skill yang tersimpan di profil
    for (const std::string& skillName : profile.unlocked_skills) {
        try {
            SkillName name = SkillNameFromString(skillName);
            skills[name].isUnlocked = true;
        } catch (const std::invalid_argument& e) {
            TraceLog(LOG_WARNING, "TechTree: skip unknown skill '%s'", skillName.c_str());
        }
    }

    // Perbarui uiState (AVAILABLE/LOCKED) berdasarkan isUnlocked
    updateSkillStates();
}

// Simpan daftar skill yang sudah di-unlock ke PlayerProfile
void TechTree::saveToProfile(PlayerProfile& profile) {
    profile.unlocked_skills.clear();
    for (auto& [key, skill] : skills) {
        if (skill.isUnlocked) {
            profile.unlocked_skills.push_back(StringFromSkillName(skill.name));
        }
    }
}

// Unlock skill: kurangi RP, update state,simpan ke profil
bool TechTree::unlockSkill(SkillName name, PlayerProfile& profile) {
    // Cek: RP cukup dan skill benar-benar AVAILABLE (parent sudah di-unlock)
    if (profile.research_point >= skills[name].rpCost && skills[name].uiState == AVAILABLE) {
        profile.research_point -= skills[name].rpCost;          // Kurangi RP
        skills[name].isUnlocked = true;                          // Tandai sebagai ter-unlock
        profile.unlocked_skills.push_back(StringFromSkillName(name)); // Simpan ke profil
        updateSkillStates(); // Perbarui state child menjadi AVAILABLE
        return true;
    }
    return false;
}

// Handle input mouse: deteksi klik kiri pada node AVAILABLE
bool TechTree::handleInput(Vector2 mousePos, bool isMouseClicked, PlayerProfile& profile) {
    if (!isMouseClicked) return false;

    // Iterasi semua skill, cek collision mouse dengan bounds
    for (auto& [key, skill] : skills) {
        if (CheckCollisionPointRec(mousePos, skill.bounds)) {
            // Hanya proses jika skill berstatus AVAILABLE
            if (skill.uiState == AVAILABLE) {
                return unlockSkill(skill.name, profile);
            }
            break; // Hanya proses node pertama yang terkena
        }
    }
    return false;
}

/* Perbarui uiState semua skill menggunakan algoritma BFS dari root BARRIER.
 *
 * Logika:
 * 1. Set semua skill: UNLOCKED jika isUnlocked, LOCKED jika tidak.
 * 2. BFS dari BARRIER. Untuk setiap node yang sudah di-unlock, child-nya
 *    di-set AVAILABLE (karena parent sudah terbuka).
 * 3. Jika child juga sudah di-unlock, lanjutkan BFS ke child tersebut.
 * 4. BARRIER selalu AVAILABLE sebagai root (tidak perlu di-unlock). */
void TechTree::updateSkillStates() {
    // Step 1: Set state dasar berdasarkan isUnlocked
    for (auto& [key, skill] : skills) {
        if (skill.isUnlocked) {
            skill.uiState = UNLOCKED;
        } else {
            skill.uiState = LOCKED;
        }
    }

    // Step 2: BFS dari BARRIER untuk menentukan AVAILABLE nodes
    std::queue<SkillName> q;
    std::unordered_map<SkillName, bool> visited;

    q.push(SkillName::BARRIER);
    visited[SkillName::BARRIER] = true;

    while (!q.empty()) {
        SkillName current = q.front();
        q.pop();

        // Hanya proses child jika current sudah di-unlock
        if (skills[current].isUnlocked) {
            for (SkillName child : adjList[current]) {
                if (!visited[child]) {
                    visited[child] = true;
                    if (!skills[child].isUnlocked) {
                        // Child belum di-unlock → jadi AVAILABLE (bisa dibeli)
                        skills[child].uiState = AVAILABLE;
                    } else {
                        // Child sudah di-unlock → lanjut BFS ke child
                        q.push(child);
                    }
                }
            }
        }
    }

    // Step 3: BARRIER selalu AVAILABLE (root skill, gratis)
    if (skills[SkillName::BARRIER].uiState == LOCKED) {
        skills[SkillName::BARRIER].uiState = AVAILABLE;
    }
}

// Getter: kembalikan reference ke map skills (read-only)
const std::unordered_map<SkillName, SkillData>& TechTree::getSkills() const { return skills; }
// Getter: kembalikan reference ke adjacency list (read-only)
const std::unordered_map<SkillName, std::vector<SkillName>>& TechTree::getAdjList() const { return adjList; }
