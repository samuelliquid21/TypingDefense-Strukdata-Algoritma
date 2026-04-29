#include "WordGenerator.h"
#include <vector>
#include <random>

namespace {
    // Word lists
    const std::vector<std::string> easy = {
        "cat", "dog", "sun", "tree", "book",
        "run", "fish", "bird", "moon", "star",
        "ship", "mars", "earth", "venus", "rock"
    };

    const std::vector<std::string> medium = {
        "planet", "rocket", "system", "typing",
        "comet", "orbit", "alien", "space",
        "laser", "pilot", "galaxy", "meteor",
        "probe", "saturn"
    };

    const std::vector<std::string> hard = {
        "asteroid", "galaxy", "algorithm", "development",
        "supernova", "astronaut", "telescope", "constellation",
        "spaceship", "gravity", "universe", "starlight",
        "satellite", "blackhole"
    };

    // Random engine (dibuat sekali)
    std::mt19937 rng(std::random_device{}());

    const std::string& pick(const std::vector<std::string>& words) {
        std::uniform_int_distribution<> dist(0, words.size() - 1);
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

    // fallback (harusnya tidak terjadi)
    return easy[0];
}