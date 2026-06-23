#pragma once
#include <string>
#include <vector>
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
    // Selection Sort manual pada m_filteredNodes (A-Z atau Z-A tergantung m_sortAscending)
    void SortManual();
    // Render daftar kata + search bar
    void DrawWordList();
    // Render popup definisi (sama seperti Dictionary)
    void DrawDefinitionPopup();

    // Helper untuk BuildFromPlayer: sisipkan node ke circular DLL
    void InsertNode(WordNode* newNode);
    // Helper untuk RebuildFilter: kumpulkan node yang cocok
    void CollectFilteredNodes();
    // Helper untuk Update: proses input saat popup definisi aktif
    void HandleDefinitionInput();
    // Helper untuk Update: proses input pencarian
    void HandleSearchInput();
    // Helper untuk Update: proses input sort (S/D)
    void HandleSortInput();
    // Helper untuk Update: navigasi bawah
    void HandleDownInput(int visibleRows, int fc);
    // Helper untuk Update: navigasi atas
    void HandleUpInput(int visibleRows, int fc);
    // Helper untuk DrawWordList: render bagian header
    void DrawWordListHeader();
    // Helper untuk DrawWordList: render search bar
    void DrawSearchBar();
    // Helper untuk DrawWordList: render pesan kosong
    void DrawEmptyMessage();
    // Helper untuk DrawWordList: render info hasil
    void DrawResultInfo();
    // Helper untuk DrawWordList: render petunjuk sort
    void DrawSortHint();
    // Helper untuk DrawWordList: render pesan tidak ditemukan
    void DrawNotFoundMessage();
    // Helper untuk DrawWordList: render item-item yang terlihat
    void DrawListItems();
    // Helper untuk DrawWordList: render satu item
    void DrawListItem(int idx, int y, int rowHeight, WordNode* node);
    // Helper untuk DrawDefinitionPopup: word-wrap definisi
    void WordWrapDefinition();
    // Helper untuk DrawDefinitionPopup: hitung dimensi popup
    void ComputePopupDimensions(int& boxW, int& boxH, int& boxX, int& boxY, int& defHeight);
    // Helper untuk DrawDefinitionPopup: render konten popup
    void DrawPopupContent(int boxX, int boxY, int boxW, int boxH);

    std::vector<std::string> m_definitionLines; // Baris-baris definisi setelah word-wrap

    bool m_isSorted;         // Apakah sort sedang aktif?
    bool m_sortAscending;    // true = A-Z, false = Z-A
};
