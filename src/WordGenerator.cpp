#include "WordGenerator.h"
#include "word_module.h"
#include <random>

// ===============================
// 🔤 GENERATOR KATA RANDOM
// ===============================

namespace {
    // Random number generator dengan Mersenne Twister (seed dari hardware)
    std::mt19937 rng(std::random_device{}());

    // Pilih satu kata random dari vector kata yang diberikan
    const std::string& pick(const std::vector<std::string>& words) {
        // Distribusi uniform dari index 0 hingga size-1
        std::uniform_int_distribution<> dist(0, static_cast<int>(words.size()) - 1);
        return words[dist(rng)];
    }
}

const std::string& WordSystem::getRandomWord(Difficulty diff) {
    // Pilih kata dari pool yang sesuai dengan tingkat kesulitan
    switch (diff) {
        case Difficulty::EASY:
            return pick(easy);
        case Difficulty::MEDIUM:
            return pick(medium);
        case Difficulty::HARD:
            return pick(hard);
    }

    // Fallback: kata pertama dari pool easy (seharusnya tidak pernah tercapai)
    return easy[0];
}
