#pragma once
#include "raylib.h"
#include <string>

// Node untuk Circular Doubly Linked List menu
struct MenuNode {
    std::string data;      
    MenuNode* prev;        
    MenuNode* next;        
    MenuNode(const std::string& value) : data(value), prev(nullptr), next(nullptr) {}
};

// Class untuk menu utama dengan Circular Doubly Linked List
// Navigasi: UP = prev, DOWN = next, circular (head->...->tail->head)
class MainMenu {
public:
    MainMenu();  // Constructor: inisialisasi list dengan options default
    ~MainMenu(); // Destructor: cleanup memory untuk linked list

    void Update();  // Handle input (UP/DOWN/ENTER)
    void Draw();    // Render menu dengan traversal linked list

    int GetSelectedIndex() const;  // Index option yang terpilih
    bool IsOptionChosen() const;   // Apakah option sudah dipilih?
    void ResetChoice();            // Reset pilihan untuk frame berikutnya

private:
    std::string title;      // Judul menu
    MenuNode* head;         // Pointer ke head node (first option)
    MenuNode* current;      // Pointer ke node yang sedang terpilih

    int selectedIndex;      // Index option yang terpilih (0-based)
    int optionCount;        // Jumlah total options
    bool optionChosen;      // Flag: user sudah pilih option?

    int titleFontSize;      // Size font untuk title
    int menuFontSize;       // Size font untuk menu options

    void AddOption(const std::string& option);  // Tambah node baru ke list
    void ClearOptions();                        // Hapus semua nodes (cleanup memory)
};