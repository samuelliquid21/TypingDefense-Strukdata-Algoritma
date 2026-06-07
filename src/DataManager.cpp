#include "DataManager.h"
#include <fstream>

// ==== SINGLETON ====

DataManager& DataManager::getInstance() {
    static DataManager instance;
    return instance;
}

// ==== LOAD & SAVE ====

void DataManager::load(const std::string& path) {
    if (!path.empty()) m_filePath = path;            // update path jika ada
    std::ifstream file(m_filePath);
    if (file.is_open()) {
        file >> m_data;
    }
}

void DataManager::save() const {
    std::ofstream file(m_filePath);
    file << m_data.dump(4);                          // simpan dengan indentasi 4 spasi
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

bool DataManager::FindPlayer(const std::string& username, PlayerProfile& outProfile) {
    if (!m_data.contains("data") || !m_data["data"].is_array()) {
        return false;
    }

    for (const auto& entry : m_data["data"]) {
        if (entry.contains("username") && entry["username"].get<std::string>() == username) {
            outProfile.username = username;
            outProfile.highest_score = entry.value("highest_score", 0);
            outProfile.research_point = entry.value("research_point", 0);
            outProfile.unlocked_words = entry.value("unlocked_words", std::vector<std::string>{});
            outProfile.unlocked_skills = entry.value("unlocked_skills", std::vector<std::string>{});
            return true;
        }
    }
    return false;
}

void DataManager::SavePlayer(const PlayerProfile& profile) {
    if (!m_data.contains("data") || !m_data["data"].is_array()) {
        m_data["data"] = json::array();
    }

    for (auto& entry : m_data["data"]) {
        if (entry.contains("username") && entry["username"].get<std::string>() == profile.username) {
            entry["highest_score"] = profile.highest_score;
            entry["research_point"] = profile.research_point;
            entry["unlocked_words"] = profile.unlocked_words;
            entry["unlocked_skills"] = profile.unlocked_skills;
            save();
            return;
        }
    }
}

void DataManager::CreatePlayer(const std::string& username) {
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
}
