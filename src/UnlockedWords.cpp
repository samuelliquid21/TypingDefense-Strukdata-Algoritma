#include "UnlockedWords.h"
#include "word_module.h"
#include "GameConfig.h"
#include "raylib.h"
#include <algorithm>

WordNode::WordNode(const std::string& w, const std::string& d, const std::string& def)
    : word(w), difficulty(d), definition(def), prev(nullptr), next(nullptr) {}

UnlockedWords::UnlockedWords()
    : m_head(nullptr), m_current(nullptr), m_selectedIndex(0),
      m_nodeCount(0), m_scrollOffset(0), m_showDefinition(false), m_requestBack(false) {}

UnlockedWords::~UnlockedWords() {
    ClearList();
}

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
    if (m_head) m_current = m_head;
}

bool UnlockedWords::WantsToGoBack() {
    if (m_requestBack) {
        m_requestBack = false;
        Reset();
        return true;
    }
    return false;
}

void UnlockedWords::BuildFromPlayer(const PlayerProfile& profile) {
    ClearList();
    m_nodeCount = 0;

    for (const auto& word : profile.unlocked_words) {
        std::string difficulty;
        std::string definition;

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

        WordNode* newNode = new WordNode(word, difficulty, definition);

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

    m_selectedIndex = 0;
    m_scrollOffset = 0;
    m_showDefinition = false;
    m_requestBack = false;
}

void UnlockedWords::Update() {
    if (m_showDefinition) {
        if (IsKeyPressed(KEY_ENTER)) {
            m_showDefinition = false;
        }
        return;
    }

    if (m_nodeCount == 0) {
        if (IsKeyPressed(KEY_ESCAPE))
            m_requestBack = true;
        return;
    }

    int visibleRows = (Config::screenHeight - 120 - 60) / 30;

    if (IsKeyPressed(KEY_DOWN)) {
        m_current = m_current->next;
        m_selectedIndex = (m_selectedIndex + 1) % m_nodeCount;
        if (m_selectedIndex == 0)
            m_scrollOffset = 0;
        else if (m_selectedIndex >= m_scrollOffset + visibleRows)
            m_scrollOffset = m_selectedIndex - visibleRows + 1;
    }

    if (IsKeyPressed(KEY_UP)) {
        m_current = m_current->prev;
        m_selectedIndex = (m_selectedIndex - 1 + m_nodeCount) % m_nodeCount;
        if (m_selectedIndex >= m_nodeCount - 1) {
            int maxOff = m_nodeCount - visibleRows;
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

void UnlockedWords::DrawWordList() {
    int rowHeight = 30;
    int startY = 100;

    DrawText("KATA TERUNLOCK", (Config::screenWidth - MeasureText("KATA TERUNLOCK", 30)) / 2, 20, 30, WHITE);

    std::string info = TextFormat("Total: %d kata", m_nodeCount);
    DrawText(info.c_str(), (Config::screenWidth - MeasureText(info.c_str(), 14)) / 2, 55, 14, Color{200, 200, 200, 180});

    const char* instr = "ESC: Kembali";
    DrawText(instr, Config::screenWidth - MeasureText(instr, 14) - 20, 25, 14, Color{200, 200, 200, 180});

    if (m_nodeCount == 0) {
        const char* emptyMsg = "Belum ada kata yang di-unlock";
        const char* subMsg = "Main dulu yuk!";
        DrawText(emptyMsg, (Config::screenWidth - MeasureText(emptyMsg, 20)) / 2,
                 Config::screenHeight / 2 - 20, 20, Color{200, 200, 200, 180});
        DrawText(subMsg, (Config::screenWidth - MeasureText(subMsg, 16)) / 2,
                 Config::screenHeight / 2 + 10, 16, Color{200, 200, 200, 120});
        return;
    }

    int visibleRows = (Config::screenHeight - 120 - 60) / rowHeight;
    int endIdx = m_scrollOffset + visibleRows;
    if (endIdx > m_nodeCount)
        endIdx = m_nodeCount;

    int y = startY + 30;
    int idx = 0;
    WordNode* trav = m_head;
    for (int i = 0; i < m_scrollOffset; ++i)
        trav = trav->next;

    for (idx = m_scrollOffset; idx < endIdx; ++idx) {
        if (idx == m_selectedIndex) {
            DrawRectangle(100, y, Config::screenWidth - 200, rowHeight, Color{255, 255, 255, 30});
        }

        Color diffColor;
        if (trav->difficulty == "Easy")   diffColor = Color{100, 255, 100, 255};
        else if (trav->difficulty == "Medium") diffColor = Color{255, 255, 100, 255};
        else                             diffColor = Color{255, 100, 100, 255};

        DrawText(trav->word.c_str(), 120, y + (rowHeight - 20) / 2, 20,
                 idx == m_selectedIndex ? YELLOW : WHITE);

        std::string tag = "[" + trav->difficulty + "]";
        DrawText(tag.c_str(),
                 Config::screenWidth - 120 - MeasureText(tag.c_str(), 16),
                 y + (rowHeight - 16) / 2, 16, diffColor);

        y += rowHeight;
        trav = trav->next;
    }
}

void UnlockedWords::DrawDefinitionPopup() {
    DrawRectangle(0, 0, Config::screenWidth, Config::screenHeight, Color{0, 0, 0, 180});

    int lineHeight = 22;
    int maxWidth = 540;
    int fontSize = 16;

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
