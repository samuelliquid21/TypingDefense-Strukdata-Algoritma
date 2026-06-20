#include "DataManager.h"
#include <fstream>
#include "raylib.h"

// ==== SINGLETON ====

DataManager& DataManager::getInstance() {
    static DataManager instance; // Inisialisasi sekali, thread-safe di C++11+
    return instance;
}

// ==== LOAD & SAVE ====

void DataManager::load(const std::string& path) {
    if (!path.empty()) m_filePath = path;
    try {
        std::ifstream file(m_filePath);
        if (file.is_open()) {
            file >> m_data;
        }
    } catch (const nlohmann::detail::parse_error& e) {
        TraceLog(LOG_WARNING, "DataManager: parse error, reset data. %s", e.what());
        m_data = json::object();
    }
}

void DataManager::save() const {
    try {
        std::ofstream file(m_filePath);
        if (!file.is_open()) {
            TraceLog(LOG_WARNING, "DataManager: cannot write %s", m_filePath.c_str());
            return;
        }
        file << m_data.dump(4);
    } catch (const std::exception& e) {
        TraceLog(LOG_ERROR, "DataManager: save failed. %s", e.what());
    }
}

// ==== GETTER ====

json& DataManager::getData() {
    return m_data;
}

const json& DataManager::getData() const {
    return m_data;
}

// ==== SETTER ====

void DataManager::setFilePath(const std::string& path) {
    m_filePath = path;
}

// ==== PLAYER MANAGEMENT ====

// Cari player berdasarkan username dengan iterasi array "data"
bool DataManager::FindPlayer(const std::string& username, PlayerProfile& outProfile) {
    // Pastikan struktur JSON valid: ada key "data" dan berupa array
    if (!m_data.contains("data") || !m_data["data"].is_array()) {
        return false;
    }

    // Iterasi setiap entry dalam array untuk mencari username yang cocok
    for (const auto& entry : m_data["data"]) {
        try {
            if (entry.contains("username") && entry["username"].get<std::string>() == username) {
                // Salin data dari JSON ke struct PlayerProfile
                outProfile.username = username;
                outProfile.highest_score = entry.value("highest_score", 0);
                outProfile.research_point = entry.value("research_point", 0);
                outProfile.unlocked_words = entry.value("unlocked_words", std::vector<std::string>{});
                outProfile.unlocked_skills = entry.value("unlocked_skills", std::vector<std::string>{});
                outProfile.survival_time    = entry.value("survival_time",    0.0f);
                return true;
            }
        } catch (const nlohmann::detail::type_error& e) {
            TraceLog(LOG_WARNING, "DataManager: skip corrupt entry. %s", e.what());
            continue;
        }
    }
    return false; // Tidak ditemukan
}

// Update data player yang sudah ada di memory JSON, lalu simpan ke file
void DataManager::SavePlayer(const PlayerProfile& profile) {
    try {
        if (!m_data.contains("data") || !m_data["data"].is_array()) {
            m_data["data"] = json::array();
        }

        for (auto& entry : m_data["data"]) {
            if (entry.contains("username") && entry["username"].get<std::string>() == profile.username) {
                entry["highest_score"] = profile.highest_score;
                entry["research_point"] = profile.research_point;
                entry["unlocked_words"] = profile.unlocked_words;
                entry["unlocked_skills"] = profile.unlocked_skills;
                entry["survival_time"]    = profile.survival_time;
                save();
                return;
            }
        }
    } catch (const std::exception& e) {
        TraceLog(LOG_ERROR, "DataManager::SavePlayer failed. %s", e.what());
    }
}

// Buat entry player baru dengan default values dan simpan ke file
void DataManager::CreatePlayer(const std::string& username) {
    try {
        if (!m_data.contains("data") || !m_data["data"].is_array()) {
            m_data["data"] = json::array();
        }

        json newEntry;
        newEntry["username"] = username;
        newEntry["highest_score"] = 0;
        newEntry["research_point"] = 0;
        newEntry["unlocked_words"] = json::array();
        newEntry["unlocked_skills"] = json::array();

        m_data["data"].push_back(newEntry);
        save();
    } catch (const std::exception& e) {
        TraceLog(LOG_ERROR, "DataManager::CreatePlayer failed. %s", e.what());
    }
}
