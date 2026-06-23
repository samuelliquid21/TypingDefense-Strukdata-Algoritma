#pragma once
#include "raylib.h"
#include <string>

// Node untuk Circular Doubly Linked List menu
struct MenuNode {
    std::string data;      // Teks option menu
    MenuNode* prev;        // Pointer ke node sebelumnya (navigasi UP)
    MenuNode* next;        // Pointer ke node berikutnya (navigasi DOWN)
    MenuNode(const std::string& value) : data(value), prev(nullptr), next(nullptr) {}
};

// Class untuk menu utama dengan Circular Doubly Linked List
// Navigasi: UP = prev, DOWN = next, circular (head->...->tail->head)
// Circular DDL dipilih agar navigasi berputar seamless tanpa boundary check
class MainMenu {
public:
    MainMenu();  // Constructor: inisialisasi list dengan options default
    ~MainMenu(); // Destructor: cleanup memory untuk linked list

    void Update();  // Handle input (UP/DOWN/ENTER) dengan navigasi circular
    void Draw();    // Render menu dengan traversal Circular Doubly Linked List

    int GetSelectedIndex() const;  // Index option yang terpilih (0-based)
    bool IsOptionChosen() const;   // Apakah option sudah dipilih via ENTER?
    void ResetChoice();            // Reset flag optionChosen untuk frame berikutnya

private:
    std::string title;      // Judul menu (Cosmic Keypad)
    MenuNode* head;         // Pointer ke head node (first option dalam list)
    MenuNode* current;      // Pointer ke node yang sedang terpilih (untuk navigasi)

    int selectedIndex;      // Index option yang terpilih (0-based)
    int optionCount;        // Jumlah total options dalam linked list
    bool optionChosen;      // Flag: user sudah tekan ENTER untuk pilih option?

    int titleFontSize;      // Size font untuk title
    int menuFontSize;       // Size font untuk menu options

    void AddOption(const std::string& option);  // Tambah node baru ke Circular Doubly Linked List
    void ClearOptions();                        // Hapus semua nodes dan cleanup memory
    void initOptionDefaults();                  // Tambah semua option default (Play, Quit, dll)
    void drawTitle();                           // Render judul "Cosmic Keypad"
    void drawMenuItems();                       // Render daftar menu dengan navigasi circular
};
