#include "DataManager.h"
#include <fstream>

// ==== SINGLETON ====

DataManager& DataManager::getInstance() {
    static DataManager instance; // Inisialisasi sekali, thread-safe di C++11+
    return instance;
}

// ==== LOAD & SAVE ====

void DataManager::load(const std::string& path) {
    if (!path.empty()) m_filePath = path;            // Update path jika parameter tidak kosong
    std::ifstream file(m_filePath);
    if (file.is_open()) {
        file >> m_data;  // Parse JSON dari file ke struktur nlohmann::json
    }
    // Jika file tidak ditemukan, m_data tetap kosong — akan terbuat saat CreatePlayer pertama
}

void DataManager::save() const {
    std::ofstream file(m_filePath);
    file << m_data.dump(4);                          // Simpan dengan indentasi 4 spasi agar readable
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
        if (entry.contains("username") && entry["username"].get<std::string>() == username) {
            // Salin data dari JSON ke struct PlayerProfile
            outProfile.username = username;
            outProfile.highest_score = entry.value("highest_score", 0);
            outProfile.research_point = entry.value("research_point", 0);
            outProfile.unlocked_words = entry.value("unlocked_words", std::vector<std::string>{});
            outProfile.unlocked_skills = entry.value("unlocked_skills", std::vector<std::string>{});
            return true;
        }
    }
    return false; // Tidak ditemukan
}

// Update data player yang sudah ada di memory JSON, lalu simpan ke file
void DataManager::SavePlayer(const PlayerProfile& profile) {
    // Pastikan key "data" ada sebagai array
    if (!m_data.contains("data") || !m_data["data"].is_array()) {
        m_data["data"] = json::array();
    }

    // Cari entry dengan username yang cocok dan update field-nya
    for (auto& entry : m_data["data"]) {
        if (entry.contains("username") && entry["username"].get<std::string>() == profile.username) {
            entry["highest_score"] = profile.highest_score;
            entry["research_point"] = profile.research_point;
            entry["unlocked_words"] = profile.unlocked_words;
            entry["unlocked_skills"] = profile.unlocked_skills;
            save();  // Langsung simpan ke file setelah update
            return;
        }
    }
    // Jika tidak ditemukan, tidak melakukan apa-apa (harus CreatePlayer dulu)
}

// Buat entry player baru dengan default values dan simpan ke file
void DataManager::CreatePlayer(const std::string& username) {
    // Pastikan key "data" ada sebagai array
    if (!m_data.contains("data") || !m_data["data"].is_array()) {
        m_data["data"] = json::array();
    }

    // Buat objek JSON baru dengan default values
    json newEntry;
    newEntry["username"] = username;
    newEntry["highest_score"] = 0;
    newEntry["research_point"] = 0;
    newEntry["unlocked_words"] = json::array();
    newEntry["unlocked_skills"] = json::array();

    m_data["data"].push_back(newEntry); // Tambah entry ke array
    save(); // Simpan perubahan ke file
}
