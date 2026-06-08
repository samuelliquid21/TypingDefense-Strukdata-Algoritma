#pragma once
#include "../ext/nlohmann/json.hpp"
#include <fstream>
#include <string>

using json = nlohmann::json;

// LEGACY — seharusnya digantikan oleh DataManager
// Namespace fungsional untuk membaca/menulis data skor ke file JSON (data.json)
namespace ScoreManager {

    // Baca file JSON dari path dan parse ke objek json
    inline json Load(const std::string& path) {
        std::ifstream file(path);
        json j;
        file >> j;
        return j;
    }

    // Tulis objek json ke file dengan indentasi 4 spasi
    inline void Save(const std::string& path, const json& j) {
        std::ofstream file(path);
        file << j.dump(4);
    }

    // Masukkan atau update skor pemain:
    // - Jika username sudah ada, update skor hanya jika skor baru lebih besar
    // - Jika username belum ada, tambahkan entry baru
    inline void InsertOrUpdate(json& j, const std::string& username, int score) {
        bool found = false;

        for (auto& p : j["data"]) {
            if (p["username"] == username) {
                if (score > p["score"]) {
                    p["score"] = score;
                }
                found = true;
                break;
            }
        }

        if (!found) {
            j["data"].push_back({
                {"username", username},
                {"score", score}
            });
        }
    }

    // Dapatkan skor tertinggi dari seluruh data
    inline int GetHighest(const json& j) {
        int maxScore = 0;
        for (auto& p : j["data"]) {
            if (p["score"] > maxScore) {
                maxScore = p["score"];
            }
        }
        return maxScore;
    }
}
