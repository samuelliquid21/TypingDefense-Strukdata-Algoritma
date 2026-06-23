#pragma once

// Enum yang merepresentasikan seluruh state dalam game state machine.
// Setiap state memiliki Update*() dan Draw*() sendiri di Game.cpp.
enum class GameState {
    MENU,                // Tampilan menu utama
    GAMEPLAY,            // Permainan sedang berjalan
    PAUSE,               // Game dijeda (ESC) — countdown sebelum kembali
    GAME_OVER,           // Player terkena asteroid
    LEADERBOARD,         // Papan peringkat skor
    CREDIT,              // Layar kredit dengan efek glitch transisi
    LOGIN_AND_REGISTER,  // Form login/register pemain
    LOGOUT,              // Keluar dari sesi pemain
    UNLOCK_SKILL,        // Tech tree untuk membuka skill
    WORD_DICTIONARY,     // Daftar kata yang tersedia
    UNLOCKED_WORDS,       // Kata-kata yang sudah dibuka pemain
    REGISTER,            // Layar pendaftaran akun baru
    SKIN_SELECT,         // Pilih skin / beli skin
    GACHA                // Gacha crate
};
