#pragma once
#include <string>
#include <vector>

// Struktur untuk satu entri di kamus: menampung kata beserta label kesulitannya
struct DictionaryEntry {
    std::string word;       // Kata (dalam bahasa Inggris)
    std::string difficulty; // Label kesulitan: "Easy", "Medium", atau "Hard"
};

// Kelas yang menampilkan dan mengelola kamus kata (menu Dictionary)
// Menyediakan pencarian (substring), navigasi daftar, dan popup definisi
class Dictionary {
public:
    Dictionary();
    // Update logika input: ketik cari, panah navigasi, ENTER lihat definisi, ESC kembali
    void Update();
    // Render seluruh tampilan kamus (daftar kata + popup definisi jika aktif)
    void Draw();
    // Reset state kamus ke awal (indeks, scroll, pencarian, definisi)
    void Reset();
    // Mengecek apakah pengguna meminta kembali ke menu sebelumnya; return true sekali lalu reset
    bool WantsToGoBack();

private:
    std::vector<DictionaryEntry> m_entries;       // Seluruh entri kamus (sudah di-sort)
    std::vector<std::string> m_wordStrings;       // Hanya kumpulan kata (sebagai index untuk std::find)
    std::vector<DictionaryEntry> m_filteredEntries; // Hasil filter berdasarkan m_searchQuery
    std::vector<std::string> m_definitionLines;     // Baris-baris definisi setelah word-wrap

    int m_selectedIndex;     // Indeks yang dipilih di m_filteredEntries
    int m_scrollOffset;      // Offset scroll untuk virtual scrolling
    bool m_showDefinition;   // Sedang menampilkan popup definisi?
    bool m_requestBack;      // Flag tombol ESC ditekan, ditahan sampai WantsToGoBack() dipanggil
    std::string m_currentWord;        // Kata yang sedang ditampilkan definisinya
    std::string m_currentDefinition;  // Definisi mentah (sebelum word-wrap)
    std::string m_searchQuery;        // Teks pencarian yang diketik user

    int m_easyCount;   // Jumlah kata Easy
    int m_mediumCount; // Jumlah kata Medium
    int m_hardCount;   // Jumlah kata Hard

    // Menggabungkan easy/medium/hard dari word_module.h, lalu sortir alfabetis
    void LoadWords();
    // Memfilter m_entries berdasarkan substring m_searchQuery, hasil ke m_filteredEntries
    void ApplyFilter();
    // Memecah definisi panjang menjadi baris-baris agar muat di kotak popup
    void WordWrapDefinition();
    // Render daftar kata + search bar
    void DrawWordList();
    // Render popup definisi di atas layar (latar belakang digelapkan)
    void DrawDefinitionPopup();

    // Helper untuk LoadWords: isi m_entries dari pool kata
    void PopulateEntriesFromPools();
    // Helper untuk LoadWords: hitung jumlah tiap kesulitan
    void ComputeDifficultyCounts();
    // Helper untuk Update: proses input saat popup definisi aktif
    void HandleDefinitionInput();
    // Helper untuk Update: proses input pencarian
    void HandleSearchInput();
    // Helper untuk Update: navigasi bawah
    void HandleDownInput(int visibleRows);
    // Helper untuk Update: navigasi atas
    void HandleUpInput(int visibleRows);
    // Helper untuk Update: proses ENTER — lookup definisi
    void HandleEnterInput();
    // Helper untuk HandleEnterInput: ambil definisi dari map sesuai level
    void LookupDefinition(const std::string& diff);
    // Helper untuk DrawWordList: render item-item yang terlihat
    void DrawListItems();
    // Helper untuk DrawWordList: render satu item
    void DrawListItem(int idx, int y, int rowHeight, const DictionaryEntry& entry);
    // Helper untuk DrawDefinitionPopup: hitung dimensi popup
    void ComputePopupDimensions(int& boxW, int& boxH, int& boxX, int& boxY, int& defHeight);
    // Helper untuk DrawDefinitionPopup: render konten popup
    void DrawPopupContent(int boxX, int boxY, int boxW, int boxH);
};
