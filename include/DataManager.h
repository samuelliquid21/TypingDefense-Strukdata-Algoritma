#pragma once
#include "../ext/nlohmann/json.hpp"
#include <string>

using json = nlohmann::json;

// Singleton untuk mengelola PlayerData.json (baca/tulis)
class DataManager {
public:
    static DataManager& getInstance(); // ambil instance singleton

    void load(const std::string& path = "./data/PlayerData.json"); // load data dari file
    void save() const; // simpan data ke file

    json& getData(); // ambil referensi data (bisa diubah)
    const json& getData() const; // ambil referensi data (read-only)

    void setFilePath(const std::string& path); // ganti path file

private:
    DataManager() = default;
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    json m_data; // data json di memory
    std::string m_filePath = "./data/PlayerData.json"; // path file default
};
