#include "UnlockedWords.h"
#include "word_module.h"
#include "GameConfig.h"
#include "raylib.h"
#include <algorithm>

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

        // Cari kata di pool Easy, lalu Medium, lalu Hard
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

        // Buat node baru, lalu tautkan ke list circular
        WordNode* newNode = new WordNode(word, difficulty, definition);

        if (m_head == nullptr) {
            // Node pertama: circular ke dirinya sendiri
            m_head = newNode;
            newNode->next = m_head;
            newNode->prev = m_head;
            m_current = m_head;
        } else {
            // Sisipkan di akhir: tail->next = newNode, head->prev = newNode
            WordNode* tail = m_head->prev;
            tail->next = newNode;
            newNode->prev = tail;
            newNode->next = m_head;
            m_head->prev = newNode;
        }
        m_nodeCount++;
    }

    m_selectedIndex = 0;
    m_scrollOffset = 0;
    m_showDefinition = false;
    m_requestBack = false;
    m_searchQuery.clear();
    m_isSorted = false;
    m_sortAscending = true;
    RebuildFilter();
}

// Traverse Circular DLL dan kumpulkan node yang namanya mengandung substring m_searchQuery
void UnlockedWords::RebuildFilter() {
    m_filteredNodes.clear();
    if (m_head == nullptr) return;

    WordNode* trav = m_head;
    do {
        if (m_searchQuery.empty() ||
            trav->word.find(m_searchQuery) != std::string::npos) {
            m_filteredNodes.push_back(trav);
        }
        trav = trav->next;
    } while (trav != m_head);

    if (m_isSorted) {
        SortManual();
    }

    if (m_selectedIndex >= (int)m_filteredNodes.size())
        m_selectedIndex = m_filteredNodes.empty() ? 0 : (int)m_filteredNodes.size() - 1;
    m_scrollOffset = 0;
    m_current = m_filteredNodes.empty() ? nullptr : m_filteredNodes[m_selectedIndex];
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
    // Jika popup definisi aktif, hanya proses ENTER untuk menutup
    if (m_showDefinition) {
        if (IsKeyPressed(KEY_ENTER)) {
            m_showDefinition = false;
        }
        return;
    }

    // Jika tidak ada kata, hanya ESC yang berfungsi
    if (m_nodeCount == 0) {
        if (IsKeyPressed(KEY_ESCAPE))
            m_requestBack = true;
        return;
    }

    // === INPUT PENCARIAN ===
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

    // Jika hasil filter kosong, hanya ESC yang berfungsi
    if (m_filteredNodes.empty()) {
        if (IsKeyPressed(KEY_ESCAPE))
            m_requestBack = true;
        return;
    }

    // SORT: S = A-Z, D = Z-A (hanya saat search kosong agar tidak bentrok input)
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

    int visibleRows = (Config::screenHeight - 120 - 60) / 30;
    int fc = (int)m_filteredNodes.size();

    // NAVIGASI BAWAH: circular wrap menggunakan modulo
    if (IsKeyPressed(KEY_DOWN)) {
        m_selectedIndex = (m_selectedIndex + 1) % fc;
        m_current = m_filteredNodes[m_selectedIndex];
        if (m_selectedIndex == 0)
            m_scrollOffset = 0;
        else if (m_selectedIndex >= m_scrollOffset + visibleRows)
            m_scrollOffset = m_selectedIndex - visibleRows + 1;
    }

    // NAVIGASI ATAS: circular wrap menggunakan modulo
    if (IsKeyPressed(KEY_UP)) {
        m_selectedIndex = (m_selectedIndex - 1 + fc) % fc;
        m_current = m_filteredNodes[m_selectedIndex];
        if (m_selectedIndex >= fc - 1) {
            // Jika melingkar ke item terakhir, scroll ke paling bawah
            int maxOff = fc - visibleRows;
            if (maxOff < 0) maxOff = 0;
            m_scrollOffset = maxOff;
        } else if (m_selectedIndex < m_scrollOffset)
            m_scrollOffset = m_selectedIndex;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        m_showDefinition = true;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        m_requestBack = true;
    }
}

void UnlockedWords::Draw() {
    DrawWordList();
    if (m_showDefinition)
        DrawDefinitionPopup();
}

// Render daftar kata dari m_filteredNodes dengan virtual scrolling
void UnlockedWords::DrawWordList() {
    int rowHeight = 30;
    int startY = 100;

    DrawText("WORD BANK", (Config::screenWidth - MeasureText("WORD BANK", 30)) / 2, 20, 30, WHITE);

    std::string info = TextFormat("Total: %d kata", m_nodeCount);
    DrawText(info.c_str(), (Config::screenWidth - MeasureText(info.c_str(), 14)) / 2, 55, 14, Color{200, 200, 200, 180});

    const char* instr = "ESC: Kembali";
    DrawText(instr, Config::screenWidth - MeasureText(instr, 14) - 20, 25, 14, Color{200, 200, 200, 180});

    // Search bar dengan kursor berkedip
    std::string searchLabel = "Cari: " + m_searchQuery;
    bool cursorOn = ((int)(GetTime() * 2) % 2 == 0);
    searchLabel += cursorOn ? "|" : " ";
    DrawText(searchLabel.c_str(), 120, 75, 18, Color{0, 255, 200, 220});

    // Pesan jika belum ada kata yang di-unlock
    if (m_nodeCount == 0) {
        const char* emptyMsg = "Belum ada kata yang di-unlock";
        const char* subMsg = "Main dulu yuk!";
        DrawText(emptyMsg, (Config::screenWidth - MeasureText(emptyMsg, 20)) / 2,
                 Config::screenHeight / 2 - 20, 20, Color{200, 200, 200, 180});
        DrawText(subMsg, (Config::screenWidth - MeasureText(subMsg, 16)) / 2,
                 Config::screenHeight / 2 + 10, 16, Color{200, 200, 200, 120});
        return;
    }

    // Info jumlah hasil
    std::string resultInfo;
    if (m_searchQuery.empty()) {
        resultInfo = TextFormat("Menampilkan semua (%zu kata)", m_filteredNodes.size());
    } else {
        resultInfo = TextFormat("Ditemukan: %zu kata", m_filteredNodes.size());
    }
    DrawText(resultInfo.c_str(), 120, 95, 14, Color{200, 200, 200, 180});

    // Petunjuk sort (hanya saat search kosong — tombol S/D bebas)
    if (m_searchQuery.empty()) {
        const char* sortHint = m_isSorted
            ? (m_sortAscending ? "S:A-Z | D:Z-A (sorted A-Z)" : "S:A-Z | D:Z-A (sorted Z-A)")
            : "S:Sort A-Z | D:Sort Z-A";
        DrawText(sortHint, Config::screenWidth - 120 - MeasureText(sortHint, 14), 75, 14, Color{0, 255, 200, 200});
    }

    // Pesan jika pencarian tidak menemukan apa pun
    if (m_filteredNodes.empty()) {
        const char* notFound = "Kata tidak ditemukan";
        DrawText(notFound, (Config::screenWidth - MeasureText(notFound, 18)) / 2,
                 Config::screenHeight / 2, 18, Color{200, 200, 200, 150});
        return;
    }

    // Virtual scrolling: render hanya item yang terlihat
    int visibleRows = (Config::screenHeight - 120 - 60) / rowHeight;
    int endIdx = m_scrollOffset + visibleRows;
    if (endIdx > (int)m_filteredNodes.size())
        endIdx = (int)m_filteredNodes.size();

    int y = startY + 30;
    for (int idx = m_scrollOffset; idx < endIdx; ++idx) {
        WordNode* node = m_filteredNodes[idx];

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

        y += rowHeight;
    }
}

// Render popup definisi di UnlockedWords (mirip dengan Dictionary::DrawDefinitionPopup)
// Mengakses definisi dari m_current->definition
void UnlockedWords::DrawDefinitionPopup() {
    DrawRectangle(0, 0, Config::screenWidth, Config::screenHeight, Color{0, 0, 0, 180});

    int lineHeight = 22;
    int maxWidth = 540;
    int fontSize = 16;

    // Word-wrap definisi secara lokal
    std::vector<std::string> lines;
    std::string remaining = m_current->definition;
    while (!remaining.empty()) {
        if (MeasureText(remaining.c_str(), fontSize) <= maxWidth) {
            lines.push_back(remaining);
            break;
        }
        size_t lastSpace = std::string::npos;
        for (size_t i = 0; i < remaining.size(); ++i) {
            if (remaining[i] == ' ') {
                std::string testLine = remaining.substr(0, i);
                if (MeasureText(testLine.c_str(), fontSize) > maxWidth)
                    break;
                lastSpace = i;
            }
        }
        if (lastSpace == std::string::npos) {
            lines.push_back(remaining);
            break;
        }
        lines.push_back(remaining.substr(0, lastSpace));
        remaining = remaining.substr(lastSpace + 1);
    }

    // Hitung dimensi kotak popup dinamis berdasarkan jumlah baris definisi
    int defHeight = (int)lines.size() * lineHeight + 20;
    if (defHeight < 40) defHeight = 40;
    int boxW = 600;
    int boxH = 80 + defHeight;
    if (boxH > Config::screenHeight - 60)
        boxH = Config::screenHeight - 60;
    int boxX = (Config::screenWidth - boxW) / 2;
    int boxY = (Config::screenHeight - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Color{20, 20, 50, 255});
    DrawRectangleLines(boxX, boxY, boxW, boxH, Color{0, 255, 200, 200});

    DrawText(m_current->word.c_str(),
             (Config::screenWidth - MeasureText(m_current->word.c_str(), 28)) / 2,
             boxY + 20, 28, YELLOW);

    int defY = boxY + 60;
    for (auto it = lines.begin(); it != lines.end(); ++it) {
        DrawText(it->c_str(), boxX + 30, defY, fontSize, WHITE);
        defY += lineHeight;
    }

    const char* hint = "ENTER untuk kembali";
    DrawText(hint, (Config::screenWidth - MeasureText(hint, 14)) / 2,
             boxY + boxH - 25, 14, Color{200, 200, 200, 180});
}
