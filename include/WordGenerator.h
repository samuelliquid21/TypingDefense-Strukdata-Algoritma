#pragma once
#include <string>

enum class Difficulty {
    EASY,
    MEDIUM,
    HARD
};

namespace WordSystem {

    /**
     * Mengambil 1 kata random berdasarkan difficulty.
     * 
     * @param diff Difficulty (EASY, MEDIUM, HARD)
     * @return Referensi ke kata (tidak dicopy)
     */
    const std::string& getRandomWord(Difficulty diff);

}