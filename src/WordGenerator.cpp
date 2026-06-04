#include "WordGenerator.h"
#include "word_module.h"
#include <random>

namespace {
    std::mt19937 rng(std::random_device{}());

    const std::string& pick(const std::vector<std::string>& words) {
        std::uniform_int_distribution<> dist(0, static_cast<int>(words.size()) - 1);
        return words[dist(rng)];
    }
}

const std::string& WordSystem::getRandomWord(Difficulty diff) {
    switch (diff) {
        case Difficulty::EASY:
            return pick(easy);
        case Difficulty::MEDIUM:
            return pick(medium);
        case Difficulty::HARD:
            return pick(hard);
    }

    return easy[0];
}