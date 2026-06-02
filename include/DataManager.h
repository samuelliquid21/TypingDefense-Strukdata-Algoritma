#pragma once
#include "../ext/nlohmann/json.hpp"
#include <string>

using json = nlohmann::json;

class DataManager {
public:
    static DataManager& getInstance();

    void load(const std::string& path = "./data/PlayerData.json");
    void save() const;

    json& getData();
    const json& getData() const;

    void setFilePath(const std::string& path);

private:
    DataManager() = default;
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    json m_data;
    std::string m_filePath = "./data/PlayerData.json";
};
