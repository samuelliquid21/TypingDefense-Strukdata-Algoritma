#pragma once
#include <string>

// enum untuk menentukan pilihan kesulitan kata
enum class Difficulty {
    EASY,
    MEDIUM,
    HARD
};

namespace WordSystem {
    // fungsi untuk mengambil kata random sesuai dengan tingak kesulitan
    const std::string& getRandomWord(Difficulty diff);
}