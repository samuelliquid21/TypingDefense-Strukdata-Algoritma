#pragma once
#include <string>

// Tingkat kesulitan kata berdasarkan panjang huruf
enum class Difficulty {
    EASY,   // 1-5 huruf
    MEDIUM, // 6-10 huruf
    HARD    // 11+ huruf
};

// Namespace untuk sistem pemilihan kata random
namespace WordSystem {
    // Ambil satu kata secara random sesuai tingkat kesulitan yang diminta
    const std::string& getRandomWord(Difficulty diff);
}
