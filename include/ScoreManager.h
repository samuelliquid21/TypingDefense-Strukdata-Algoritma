#pragma once
#include "../ext/nlohmann/json.hpp"
#include <fstream>
#include <string>

using json = nlohmann::json;

namespace ScoreManager {

    inline json Load(const std::string& path) {
        std::ifstream file(path);
        json j;
        file >> j;
        return j;
    }

    inline void Save(const std::string& path, const json& j) {
        std::ofstream file(path);
        file << j.dump(4);
    }

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