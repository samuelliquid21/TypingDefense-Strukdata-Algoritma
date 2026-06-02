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
