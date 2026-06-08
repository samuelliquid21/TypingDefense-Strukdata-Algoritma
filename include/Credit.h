#pragma once
#include "raylib.h"
#include <vector>
#include <string>

// Struct untuk satu entri credit: role, nama, dan flag apakah header section
struct CreditEntry {
    std::string role;      // Peran/jabatan (contoh: "Gameplay Programmer")
    std::string name;      // Nama orang atau teks section (contoh: "=== ENGINEERING ===")
    bool isHeader;         // True jika ini header section (warna kuning, bold)
};

// Class untuk state Credit: menampilkan credit title dengan auto-scroll
class Credit {
public:
    Credit();                          // Constructor: init entries dan scroll position
    void Update(bool& backToMenu);     // Update scroll + handle input (back, manual scroll)
    void Draw();                       // Render semua credit entries dengan scrolling

private:
    std::vector<CreditEntry> entries;  // Daftar entri credit yang akan ditampilkan
    float scrollY;                     // Posisi scroll vertikal (mulai dari bawah layar)
    float scrollSpeed;                 // Kecepatan auto-scroll (pixel/detik)
    float totalHeight;                 // Total tinggi semua entri + padding untuk looping
};
