#pragma once
#include "json.hpp"
#include <string>
#include <vector>

using json = nlohmann::json;

// Struct untuk menyimpan data profil satu player
struct PlayerProfile {
    std::string username;                      // Nama unik player
    int highest_score = 0;                     // Skor tertinggi yang pernah dicapai
    int research_point = 0;                    // Poin riset (currency untuk unlock)
    float survival_time    = 0.0f;
    std::vector<std::string> unlocked_words;   // Daftar kata-kata yang sudah di-unlock
    std::vector<std::string> unlocked_skills;  // Daftar skill yang sudah di-unlock
};

// Singleton untuk mengelola data player di PlayerData.json (baca/tulis dari file)
class DataManager {
public:
    static DataManager& getInstance(); // Ambil instance singleton (thread-safe via static local)

    void load(const std::string& path = "./data/PlayerData.json"); // Load data dari file JSON ke memory
    void save() const; // Simpan data dari memory ke file JSON (indentasi 4 spasi)

    json& getData(); // Ambil referensi data JSON (bisa dimodifikasi langsung)
    const json& getData() const; // Ambil referensi data JSON (read-only)

    void setFilePath(const std::string& path); // Ganti path file data

    bool FindPlayer(const std::string& username, PlayerProfile& outProfile); // Cari player berdasarkan username
    void SavePlayer(const PlayerProfile& profile); // Update data player di JSON + simpan ke file
    void CreatePlayer(const std::string& username); // Buat entry player baru dengan default values

private:
    DataManager() = default;                             // Constructor private (singleton)
    DataManager(const DataManager&) = delete;            // Non-copyable
    DataManager& operator=(const DataManager&) = delete; // Non-assignable

    json m_data;              // Data JSON yang disimpan di memory
    std::string m_filePath = "./data/PlayerData.json"; // Path file JSON default
};
