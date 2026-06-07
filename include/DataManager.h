#pragma once
#include "../ext/nlohmann/json.hpp"
#include <string>
#include <vector>

using json = nlohmann::json;

struct PlayerProfile {
    std::string username;
    int highest_score = 0;
    int research_point = 0;
    std::vector<std::string> unlocked_words;
    std::vector<std::string> unlocked_skills;
};

// Singleton untuk mengelola PlayerData.json (baca/tulis)
class DataManager {
public:
    static DataManager& getInstance(); // ambil instance singleton

    void load(const std::string& path = "./data/PlayerData.json"); // load data dari file
    void save() const; // simpan data ke file

    json& getData(); // ambil referensi data (bisa diubah)
    const json& getData() const; // ambil referensi data (read-only)

    void setFilePath(const std::string& path); // ganti path file

    bool FindPlayer(const std::string& username, PlayerProfile& outProfile); // cari player by username
    void SavePlayer(const PlayerProfile& profile); // update data player di JSON + save
    void CreatePlayer(const std::string& username); // buat entry player baru

private:
    DataManager() = default;
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    json m_data; // data json di memory
    std::string m_filePath = "./data/PlayerData.json"; // path file default
};
