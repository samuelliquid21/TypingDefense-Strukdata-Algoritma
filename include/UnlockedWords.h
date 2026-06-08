#pragma once
#include <string>
#include "DataManager.h"

// Node untuk Circular Doubly Linked List yang menyimpan kata yang sudah di-unlock
struct WordNode {
    std::string word;       // Kata dalam bahasa Inggris
    std::string difficulty; // Label kesulitan: "Easy", "Medium", "Hard"
    std::string definition; // Definisi kata dalam bahasa Indonesia
    WordNode* prev;         // Pointer ke node sebelumnya (circular)
    WordNode* next;         // Pointer ke node berikutnya (circular)
    WordNode(const std::string& w, const std::string& d, const std::string& def);
};

// Kelas yang menampilkan daftar kata yang telah di-unlock player
// Dibangun dari PlayerProfile.unlocked_words menjadi Circular Doubly Linked List
class UnlockedWords {
public:
    UnlockedWords();
    ~UnlockedWords();
    // Update logika input: pencarian, navigasi, popup definisi
    void Update();
    // Render seluruh tampilan Word Bank
    void Draw();
    // Reset state ke awal
    void Reset();
    // Bangun Circular DLL dari daftar unlocked_words milik player
    void BuildFromPlayer(const PlayerProfile& profile);
    // Cek apakah user minta kembali; return true sekali lalu reset
    bool WantsToGoBack();

private:
    WordNode* m_head;         // Head Circular DLL (pointer ke node pertama)
    WordNode* m_current;      // Node yang sedang dipilih/ditunjuk
    int m_selectedIndex;      // Indeks di m_filteredNodes
    int m_nodeCount;          // Jumlah total node dalam list
    int m_scrollOffset;       // Offset scroll untuk virtual scrolling
    bool m_showDefinition;    // Sedang menampilkan popup definisi?
    bool m_requestBack;       // Flag tombol ESC
    std::string m_searchQuery;  // Teks pencarian user
    std::vector<WordNode*> m_filteredNodes; // Pointer ke node yang cocok dengan pencarian (memudahkan navigasi & rendering)

    // Hapus semua node Circular DLL (dealokasi memori)
    void ClearList();
    // Traverse Circular DLL dan kumpulkan node yang cocok dengan m_searchQuery ke m_filteredNodes
    void RebuildFilter();
    // Render daftar kata + search bar
    void DrawWordList();
    // Render popup definisi (sama seperti Dictionary)
    void DrawDefinitionPopup();
};
