#pragma once

// enum untuk state game

enum class GameState {
    LOADING,        // Loading screen + opening text
    OPENING,        // Cinematic opening conversation
    MENU,
    GAMEPLAY,
    PAUSE,
    GAME_OVER,
    LEADERBOARD,
    CREDIT,
    SKIN_SELECT,
    GACHA
};
