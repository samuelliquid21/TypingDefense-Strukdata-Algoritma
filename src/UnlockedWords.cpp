#include "UnlockedWords.h"
#include "word_module.h"
#include "GameConfig.h"
#include "raylib.h"
#include <algorithm>

namespace {
    // Helper: cari kata di pool Easy, Medium, Hard dan dapatkan level & definisinya
    void LookupWordInfo(const std::string& word, std::string& difficulty, std::string& definition) {
        auto it = std::find(easy.begin(), easy.end(), word);
        if (it != easy.end()) {
            difficulty = "Easy";
            definition = easy_definitions.at(word);
        } else {
            auto it2 = std::find(medium.begin(), medium.end(), word);
            if (it2 != medium.end()) {
                difficulty = "Medium";
                definition = medium_definitions.at(word);
            } else {
                difficulty = "Hard";
                auto it3 = hard_definitions.find(word);
                if (it3 != hard_definitions.end())
                    definition = it3->second;
            }
        }
    }

    // Helper: cari spasi terakhir yang muat dalam lebar maksimal
    size_t FindLastFittingSpace(const std::string& text, int fontSize, int maxWidth) {
        size_t lastSpace = std::string::npos;
        for (size_t i = 0; i < text.size(); ++i) {
            if (text[i] == ' ') {
                std::string testLine = text.substr(0, i);
                if (MeasureText(testLine.c_str(), fontSize) > maxWidth)
                    break;
                lastSpace = i;
            }
        }
        return lastSpace;
    }
}

// ===============================
// 🔓 KATA TERBUKA
// ===============================
WordNode::WordNode(const std::string& w, const std::string& d, const std::string& def)
    : word(w), difficulty(d), definition(def), prev(nullptr), next(nullptr) {}

UnlockedWords::UnlockedWords()
    : m_head(nullptr), m_current(nullptr), m_selectedIndex(0),
      m_nodeCount(0), m_scrollOffset(0), m_showDefinition(false), m_requestBack(false),
      m_isSorted(false), m_sortAscending(true) {}

UnlockedWords::~UnlockedWords() {
    ClearList();
}

// Hapus semua node Circular DLL dengan traversal do-while
// Berhenti saat kembali ke m_head (sifat circular)
void UnlockedWords::ClearList() {
    if (m_head == nullptr) return;
    WordNode* temp = m_head;
    do {
        WordNode* next = temp->next;
        delete temp;
        temp = next;
    } while (temp != m_head);
    m_head = nullptr;
    m_current = nullptr;
}

void UnlockedWords::Reset() {
    m_selectedIndex = 0;
    m_scrollOffset = 0;
    m_showDefinition = false;
    m_requestBack = false;
    m_searchQuery.clear();
    m_isSorted = false;
    m_sortAscending = true;
    RebuildFilter();
}

bool UnlockedWords::WantsToGoBack() {
    if (m_requestBack) {
        m_requestBack = false;
        Reset();
        return true;
    }
    return false;
}

// Bangun Circular Doubly Linked List dari daftar unlocked_words milik player
// Setiap kata dicari level dan definisinya dari word_module
void UnlockedWords::BuildFromPlayer(const PlayerProfile& profile) {
    ClearList();
    m_nodeCount = 0;

    for (const auto& word : profile.unlocked_words) {
        std::string difficulty;
        std::string definition;
        LookupWordInfo(word, difficulty, definition);
        WordNode* newNode = new WordNode(word, difficulty, definition);
        InsertNode(newNode);
    }

    Reset();
}

// Helper: sisipkan node baru ke circular DLL
void UnlockedWords::InsertNode(WordNode* newNode) {
    if (m_head == nullptr) {
        m_head = newNode;
        newNode->next = m_head;
        newNode->prev = m_head;
        m_current = m_head;
    } else {
        WordNode* tail = m_head->prev;
        tail->next = newNode;
        newNode->prev = tail;
        newNode->next = m_head;
        m_head->prev = newNode;
    }
    m_nodeCount++;
}

// Traverse Circular DLL dan kumpulkan node yang namanya mengandung substring m_searchQuery
void UnlockedWords::RebuildFilter() {
    m_filteredNodes.clear();
    CollectFilteredNodes();
    if (m_isSorted) {
        SortManual();
    }
    if (m_selectedIndex >= (int)m_filteredNodes.size())
        m_selectedIndex = m_filteredNodes.empty() ? 0 : (int)m_filteredNodes.size() - 1;
    m_scrollOffset = 0;
    m_current = m_filteredNodes.empty() ? nullptr : m_filteredNodes[m_selectedIndex];
}

// Helper: traverse Circular DLL dan kumpulkan node yang cocok dengan m_searchQuery
void UnlockedWords::CollectFilteredNodes() {
    if (m_head == nullptr) return;
    WordNode* trav = m_head;
    do {
        if (m_searchQuery.empty() ||
            trav->word.find(m_searchQuery) != std::string::npos) {
            m_filteredNodes.push_back(trav);
        }
        trav = trav->next;
    } while (trav != m_head);
}

// Selection Sort manual pada m_filteredNodes (tidak pakai std::sort)
void UnlockedWords::SortManual() {
    int n = (int)m_filteredNodes.size();
    for (int i = 0; i < n - 1; i++) {
        int best = i;
        for (int j = i + 1; j < n; j++) {
            if (m_sortAscending) {
                if (m_filteredNodes[j]->word < m_filteredNodes[best]->word)
                    best = j;
            } else {
                if (m_filteredNodes[j]->word > m_filteredNodes[best]->word)
                    best = j;
            }
        }
        if (best != i) {
            WordNode* temp = m_filteredNodes[i];
            m_filteredNodes[i] = m_filteredNodes[best];
            m_filteredNodes[best] = temp;
        }
    }
}

void UnlockedWords::Update() {
    if (m_showDefinition) {
        HandleDefinitionInput();
        return;
    }
    if (m_nodeCount == 0) {
        if (IsKeyPressed(KEY_ESCAPE))
            m_requestBack = true;
        return;
    }
    HandleSearchInput();
    if (m_filteredNodes.empty()) {
        if (IsKeyPressed(KEY_ESCAPE))
            m_requestBack = true;
        return;
    }
    HandleSortInput();
    int visibleRows = (Config::screenHeight - 120 - 60) / 30;
    int fc = (int)m_filteredNodes.size();
    HandleDownInput(visibleRows, fc);
    HandleUpInput(visibleRows, fc);
    if (IsKeyPressed(KEY_ENTER)) {
        m_showDefinition = true;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        m_requestBack = true;
    }
}

// Helper: proses input saat popup definisi aktif (hanya ENTER)
void UnlockedWords::HandleDefinitionInput() {
    if (IsKeyPressed(KEY_ENTER)) {
        m_showDefinition = false;
    }
}

// Helper: proses input pencarian (karakter dan backspace)
void UnlockedWords::HandleSearchInput() {
    int c = GetCharPressed();
    while (c > 0) {
        if (c >= 32 && c <= 126) {
            m_searchQuery.push_back((char)c);
            RebuildFilter();
            m_selectedIndex = 0;
        }
        c = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (!m_searchQuery.empty()) {
            m_searchQuery.pop_back();
            RebuildFilter();
            m_selectedIndex = 0;
        }
    }
}

// Helper: proses input sort (S = A-Z, D = Z-A, hanya saat search kosong)
void UnlockedWords::HandleSortInput() {
    if (m_searchQuery.empty()) {
        if (IsKeyPressed(KEY_S)) {
            m_isSorted = true;
            m_sortAscending = true;
            SortManual();
            m_selectedIndex = 0;
            m_current = m_filteredNodes.empty() ? nullptr : m_filteredNodes[0];
            m_scrollOffset = 0;
        }
        if (IsKeyPressed(KEY_D)) {
            m_isSorted = true;
            m_sortAscending = false;
            SortManual();
            m_selectedIndex = 0;
            m_current = m_filteredNodes.empty() ? nullptr : m_filteredNodes[0];
            m_scrollOffset = 0;
        }
    }
}

// Helper: navigasi bawah dengan circular wrap
void UnlockedWords::HandleDownInput(int visibleRows, int fc) {
    if (IsKeyPressed(KEY_DOWN)) {
        m_selectedIndex = (m_selectedIndex + 1) % fc;
        m_current = m_filteredNodes[m_selectedIndex];
        if (m_selectedIndex == 0)
            m_scrollOffset = 0;
        else if (m_selectedIndex >= m_scrollOffset + visibleRows)
            m_scrollOffset = m_selectedIndex - visibleRows + 1;
    }
}

// Helper: navigasi atas dengan circular wrap
void UnlockedWords::HandleUpInput(int visibleRows, int fc) {
    if (IsKeyPressed(KEY_UP)) {
        m_selectedIndex = (m_selectedIndex - 1 + fc) % fc;
        m_current = m_filteredNodes[m_selectedIndex];
        if (m_selectedIndex >= fc - 1) {
            int maxOff = fc - visibleRows;
            if (maxOff < 0) maxOff = 0;
            m_scrollOffset = maxOff;
        } else if (m_selectedIndex < m_scrollOffset)
            m_scrollOffset = m_selectedIndex;
    }
}

void UnlockedWords::Draw() {
    DrawWordList();
    if (m_showDefinition)
        DrawDefinitionPopup();
}

// Render daftar kata dari m_filteredNodes dengan virtual scrolling
void UnlockedWords::DrawWordList() {
    DrawWordListHeader();
    DrawSearchBar();
    if (m_nodeCount == 0) {
        DrawEmptyMessage();
        return;
    }
    DrawResultInfo();
    DrawSortHint();
    if (m_filteredNodes.empty()) {
        DrawNotFoundMessage();
        return;
    }
    DrawListItems();
}

// Helper: render header (judul, total info, ESC instruction)
void UnlockedWords::DrawWordListHeader() {
    DrawText("WORD BANK", (Config::screenWidth - MeasureText("WORD BANK", 30)) / 2, 20, 30, WHITE);
    std::string info = TextFormat("Total: %d kata", m_nodeCount);
    DrawText(info.c_str(), (Config::screenWidth - MeasureText(info.c_str(), 14)) / 2, 55, 14, Color{200, 200, 200, 180});
    const char* instr = "ESC: Kembali";
    DrawText(instr, Config::screenWidth - MeasureText(instr, 14) - 20, 25, 14, Color{200, 200, 200, 180});
}

// Helper: render search bar dengan kursor berkedip
void UnlockedWords::DrawSearchBar() {
    std::string searchLabel = "Cari: " + m_searchQuery;
    bool cursorOn = ((int)(GetTime() * 2) % 2 == 0);
    searchLabel += cursorOn ? "|" : " ";
    DrawText(searchLabel.c_str(), 120, 75, 18, Color{0, 255, 200, 220});
}

// Helper: render pesan jika belum ada kata yang di-unlock
void UnlockedWords::DrawEmptyMessage() {
    const char* emptyMsg = "Belum ada kata yang di-unlock";
    const char* subMsg = "Main dulu yuk!";
    DrawText(emptyMsg, (Config::screenWidth - MeasureText(emptyMsg, 20)) / 2,
             Config::screenHeight / 2 - 20, 20, Color{200, 200, 200, 180});
    DrawText(subMsg, (Config::screenWidth - MeasureText(subMsg, 16)) / 2,
             Config::screenHeight / 2 + 10, 16, Color{200, 200, 200, 120});
}

// Helper: render info jumlah hasil filter
void UnlockedWords::DrawResultInfo() {
    std::string resultInfo;
    if (m_searchQuery.empty()) {
        resultInfo = TextFormat("Menampilkan semua (%zu kata)", m_filteredNodes.size());
    } else {
        resultInfo = TextFormat("Ditemukan: %zu kata", m_filteredNodes.size());
    }
    DrawText(resultInfo.c_str(), 120, 95, 14, Color{200, 200, 200, 180});
}

// Helper: render petunjuk sort (hanya saat search kosong)
void UnlockedWords::DrawSortHint() {
    if (m_searchQuery.empty()) {
        const char* sortHint = m_isSorted
            ? (m_sortAscending ? "S:A-Z | D:Z-A (sorted A-Z)" : "S:A-Z | D:Z-A (sorted Z-A)")
            : "S:Sort A-Z | D:Sort Z-A";
        DrawText(sortHint, Config::screenWidth - 120 - MeasureText(sortHint, 14), 75, 14, Color{0, 255, 200, 200});
    }
}

// Helper: render pesan jika pencarian tidak menemukan apa pun
void UnlockedWords::DrawNotFoundMessage() {
    const char* notFound = "Kata tidak ditemukan";
    DrawText(notFound, (Config::screenWidth - MeasureText(notFound, 18)) / 2,
             Config::screenHeight / 2, 18, Color{200, 200, 200, 150});
}

// Helper: render daftar item dengan virtual scrolling
void UnlockedWords::DrawListItems() {
    int rowHeight = 30;
    int startY = 100;
    int visibleRows = (Config::screenHeight - 120 - 60) / rowHeight;
    int endIdx = m_scrollOffset + visibleRows;
    if (endIdx > (int)m_filteredNodes.size())
        endIdx = (int)m_filteredNodes.size();
    int y = startY + 30;
    for (int idx = m_scrollOffset; idx < endIdx; ++idx) {
        DrawListItem(idx, y, rowHeight, m_filteredNodes[idx]);
        y += rowHeight;
    }
}

// Helper: render satu item dalam daftar
void UnlockedWords::DrawListItem(int idx, int y, int rowHeight, WordNode* node) {
    if (idx == m_selectedIndex) {
        DrawRectangle(100, y, Config::screenWidth - 200, rowHeight, Color{255, 255, 255, 30});
    }
    Color diffColor;
    if (node->difficulty == "Easy")   diffColor = Color{100, 255, 100, 255};
    else if (node->difficulty == "Medium") diffColor = Color{255, 255, 100, 255};
    else                             diffColor = Color{255, 100, 100, 255};
    DrawText(node->word.c_str(), 120, y + (rowHeight - 20) / 2, 20,
             idx == m_selectedIndex ? YELLOW : WHITE);
    std::string tag = "[" + node->difficulty + "]";
    DrawText(tag.c_str(),
             Config::screenWidth - 120 - MeasureText(tag.c_str(), 16),
             y + (rowHeight - 16) / 2, 16, diffColor);
}

// Render popup definisi di UnlockedWords (mirip dengan Dictionary::DrawDefinitionPopup)
// Mengakses definisi dari m_current->definition
void UnlockedWords::DrawDefinitionPopup() {
    DrawRectangle(0, 0, Config::screenWidth, Config::screenHeight, Color{0, 0, 0, 180});
    WordWrapDefinition();
    int boxW, boxH, boxX, boxY, defHeight;
    ComputePopupDimensions(boxW, boxH, boxX, boxY, defHeight);
    DrawPopupContent(boxX, boxY, boxW, boxH);
}

// Helper: word-wrap definisi m_current->definition ke m_definitionLines
void UnlockedWords::WordWrapDefinition() {
    m_definitionLines.clear();
    int maxWidth = 540;
    int fontSize = 16;
    std::string remaining = m_current->definition;
    while (!remaining.empty()) {
        if (MeasureText(remaining.c_str(), fontSize) <= maxWidth) {
            m_definitionLines.push_back(remaining);
            break;
        }
        size_t lastSpace = FindLastFittingSpace(remaining, fontSize, maxWidth);
        if (lastSpace == std::string::npos) {
            m_definitionLines.push_back(remaining);
            break;
        }
        m_definitionLines.push_back(remaining.substr(0, lastSpace));
        remaining = remaining.substr(lastSpace + 1);
    }
}

// Helper: hitung dimensi kotak popup berdasarkan jumlah baris definisi
void UnlockedWords::ComputePopupDimensions(int& boxW, int& boxH, int& boxX, int& boxY, int& defHeight) {
    int lineHeight = 22;
    defHeight = (int)m_definitionLines.size() * lineHeight + 20;
    if (defHeight < 40) defHeight = 40;
    boxW = 600;
    boxH = 80 + defHeight;
    if (boxH > Config::screenHeight - 60)
        boxH = Config::screenHeight - 60;
    boxX = (Config::screenWidth - boxW) / 2;
    boxY = (Config::screenHeight - boxH) / 2;
}

// Helper: render konten popup (kata, definisi, hint)
void UnlockedWords::DrawPopupContent(int boxX, int boxY, int boxW, int boxH) {
    int lineHeight = 22;
    int fontSize = 16;
    DrawRectangle(boxX, boxY, boxW, boxH, Color{20, 20, 50, 255});
    DrawRectangleLines(boxX, boxY, boxW, boxH, Color{0, 255, 200, 200});
    DrawText(m_current->word.c_str(),
             (Config::screenWidth - MeasureText(m_current->word.c_str(), 28)) / 2,
             boxY + 20, 28, YELLOW);
    int defY = boxY + 60;
    for (auto it = m_definitionLines.begin(); it != m_definitionLines.end(); ++it) {
        DrawText(it->c_str(), boxX + 30, defY, fontSize, WHITE);
        defY += lineHeight;
    }
    const char* hint = "ENTER untuk kembali";
    DrawText(hint, (Config::screenWidth - MeasureText(hint, 14)) / 2,
             boxY + boxH - 25, 14, Color{200, 200, 200, 180});
}
