#include "Dictionary.h"
#include "word_module.h"
#include "GameConfig.h"
#include "raylib.h"
#include <algorithm>
#include <string>

Dictionary::Dictionary() {
    LoadWords();
    Reset();
}

void Dictionary::Reset() {
    m_selectedIndex = 0;
    m_scrollOffset = 0;
    m_showDefinition = false;
    m_requestBack = false;
    m_currentWord.clear();
    m_currentDefinition.clear();
    m_searchQuery.clear();
    m_filteredEntries = m_entries;
}

bool Dictionary::WantsToGoBack() {
    if (m_requestBack) {
        m_requestBack = false;
        Reset();
        return true;
    }
    return false;
}

void Dictionary::LoadWords() {
    m_entries.clear();

    for (auto it = easy.begin(); it != easy.end(); ++it)
        m_entries.push_back({*it, "Easy"});
    for (auto it = medium.begin(); it != medium.end(); ++it)
        m_entries.push_back({*it, "Medium"});
    for (auto it = hard.begin(); it != hard.end(); ++it)
        m_entries.push_back({*it, "Hard"});

    std::sort(m_entries.begin(), m_entries.end(),
        [](const DictionaryEntry& a, const DictionaryEntry& b) {
            return a.word < b.word;
        });

    m_wordStrings.clear();
    for (auto it = m_entries.begin(); it != m_entries.end(); ++it)
        m_wordStrings.push_back(it->word);

    std::vector<std::string> diffs;
    for (auto it = m_entries.begin(); it != m_entries.end(); ++it)
        diffs.push_back(it->difficulty);

    m_easyCount   = (int)std::count(diffs.begin(), diffs.end(), std::string("Easy"));
    m_mediumCount = (int)std::count(diffs.begin(), diffs.end(), std::string("Medium"));
    m_hardCount   = (int)std::count(diffs.begin(), diffs.end(), std::string("Hard"));

    m_filteredEntries = m_entries;
}

void Dictionary::ApplyFilter() {
    m_filteredEntries.clear();
    if (m_searchQuery.empty()) {
        m_filteredEntries = m_entries;
    } else {
        for (const auto& entry : m_entries) {
            if (entry.word.find(m_searchQuery) != std::string::npos) {
                m_filteredEntries.push_back(entry);
            }
        }
    }
    if (m_selectedIndex >= (int)m_filteredEntries.size())
        m_selectedIndex = m_filteredEntries.empty() ? 0 : (int)m_filteredEntries.size() - 1;
}

void Dictionary::Update() {
    int visibleRows = (Config::screenHeight - 100 - 60) / 30;

    if (m_showDefinition) {
        if (IsKeyPressed(KEY_ENTER)) {
            m_showDefinition = false;
        }
        return;
    }

    int c = GetCharPressed();
    while (c > 0) {
        if (c >= 32 && c <= 126) {
            m_searchQuery.push_back((char)c);
            ApplyFilter();
            m_selectedIndex = 0;
            m_scrollOffset = 0;
        }
        c = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (!m_searchQuery.empty()) {
            m_searchQuery.pop_back();
            ApplyFilter();
            m_selectedIndex = 0;
            m_scrollOffset = 0;
        }
    }

    if (IsKeyPressed(KEY_DOWN)) {
        if (m_filteredEntries.empty()) return;
        m_selectedIndex++;
        if (m_selectedIndex >= (int)m_filteredEntries.size())
            m_selectedIndex = 0;
        if (m_selectedIndex >= m_scrollOffset + visibleRows)
            m_scrollOffset = m_selectedIndex - visibleRows + 1;
        else if (m_selectedIndex == 0)
            m_scrollOffset = 0;
    }

    if (IsKeyPressed(KEY_UP)) {
        if (m_filteredEntries.empty()) return;
        m_selectedIndex--;
        if (m_selectedIndex < 0)
            m_selectedIndex = (int)m_filteredEntries.size() - 1;
        if (m_selectedIndex < m_scrollOffset)
            m_scrollOffset = m_selectedIndex;
        else if (m_selectedIndex >= (int)m_filteredEntries.size() - 1) {
            int maxOff = (int)m_filteredEntries.size() - visibleRows;
            if (maxOff < 0) maxOff = 0;
            m_scrollOffset = maxOff;
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (m_filteredEntries.empty()) return;
        m_currentWord = m_filteredEntries[m_selectedIndex].word;

        auto it = std::find(m_wordStrings.begin(), m_wordStrings.end(), m_currentWord);
        if (it != m_wordStrings.end()) {
            int idx = (int)(it - m_wordStrings.begin());
            const std::string& diff = m_entries[idx].difficulty;

            if (diff == "Easy") {
                auto defIt = easy_definitions.find(m_currentWord);
                if (defIt != easy_definitions.end())
                    m_currentDefinition = defIt->second;
            } else if (diff == "Medium") {
                auto defIt = medium_definitions.find(m_currentWord);
                if (defIt != medium_definitions.end())
                    m_currentDefinition = defIt->second;
            } else if (diff == "Hard") {
                auto defIt = hard_definitions.find(m_currentWord);
                if (defIt != hard_definitions.end())
                    m_currentDefinition = defIt->second;
            }
        }
        WordWrapDefinition();
        m_showDefinition = true;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        m_requestBack = true;
    }
}

void Dictionary::Draw() {
    DrawWordList();
    if (m_showDefinition)
        DrawDefinitionPopup();
}

void Dictionary::DrawWordList() {
    int rowHeight = 30;
    int startY = 100;

    DrawText("KAMUS BAHASA", (Config::screenWidth - MeasureText("KAMUS BAHASA", 30)) / 2, 20, 30, WHITE);

    std::string info = TextFormat("Total: %zu  |  Easy: %d  |  Medium: %d  |  Hard: %d",
        m_entries.size(), m_easyCount, m_mediumCount, m_hardCount);
    DrawText(info.c_str(), (Config::screenWidth - MeasureText(info.c_str(), 14)) / 2, 55, 14, Color{200, 200, 200, 180});

    const char* instr = "ESC: Kembali";
    DrawText(instr, Config::screenWidth - MeasureText(instr, 14) - 20, 25, 14, Color{200, 200, 200, 180});

    std::string searchLabel = "Cari: " + m_searchQuery;
    bool cursorOn = ((int)(GetTime() * 2) % 2 == 0);
    if (cursorOn)
        searchLabel += "|";
    else
        searchLabel += " ";
    DrawText(searchLabel.c_str(), 120, 75, 18, Color{0, 255, 200, 220});

    std::string resultInfo;
    if (m_searchQuery.empty()) {
        resultInfo = TextFormat("Menampilkan semua (%zu kata)", m_filteredEntries.size());
    } else {
        resultInfo = TextFormat("Ditemukan: %zu kata", m_filteredEntries.size());
    }
    DrawText(resultInfo.c_str(), 120, 95, 14, Color{200, 200, 200, 180});

    int visibleRows = (Config::screenHeight - 120 - 60) / rowHeight;
    int endIdx = m_scrollOffset + visibleRows;
    if (endIdx > (int)m_filteredEntries.size())
        endIdx = (int)m_filteredEntries.size();

    int y = startY + 30;
    int idx = m_scrollOffset;
    for (auto it = m_filteredEntries.begin() + m_scrollOffset;
         it != m_filteredEntries.begin() + endIdx && it != m_filteredEntries.end();
         ++it, ++idx)
    {
        if (idx == m_selectedIndex) {
            DrawRectangle(100, y, Config::screenWidth - 200, rowHeight, Color{255, 255, 255, 30});
        }

        Color diffColor;
        if (it->difficulty == "Easy")   diffColor = Color{100, 255, 100, 255};
        else if (it->difficulty == "Medium") diffColor = Color{255, 255, 100, 255};
        else                             diffColor = Color{255, 100, 100, 255};

        DrawText(it->word.c_str(), 120, y + (rowHeight - 20) / 2, 20,
                 idx == m_selectedIndex ? YELLOW : WHITE);

        std::string tag = "[" + it->difficulty + "]";
        DrawText(tag.c_str(),
                 Config::screenWidth - 120 - MeasureText(tag.c_str(), 16),
                 y + (rowHeight - 16) / 2, 16, diffColor);

        y += rowHeight;
    }
}

void Dictionary::WordWrapDefinition() {
    m_definitionLines.clear();
    int maxWidth = 540;
    int fontSize = 16;

    std::string remaining = m_currentDefinition;
    while (!remaining.empty()) {
        if (MeasureText(remaining.c_str(), fontSize) <= maxWidth) {
            m_definitionLines.push_back(remaining);
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
            m_definitionLines.push_back(remaining);
            break;
        }

        m_definitionLines.push_back(remaining.substr(0, lastSpace));
        remaining = remaining.substr(lastSpace + 1);
    }
}

void Dictionary::DrawDefinitionPopup() {
    DrawRectangle(0, 0, Config::screenWidth, Config::screenHeight, Color{0, 0, 0, 180});

    int lineHeight = 22;
    int defHeight = (int)m_definitionLines.size() * lineHeight + 20;
    if (defHeight < 40) defHeight = 40;
    int boxW = 600;
    int boxH = 80 + defHeight;
    if (boxH > Config::screenHeight - 60)
        boxH = Config::screenHeight - 60;
    int boxX = (Config::screenWidth - boxW) / 2;
    int boxY = (Config::screenHeight - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Color{20, 20, 50, 255});
    DrawRectangleLines(boxX, boxY, boxW, boxH, Color{0, 255, 200, 200});

    DrawText(m_currentWord.c_str(),
             (Config::screenWidth - MeasureText(m_currentWord.c_str(), 28)) / 2,
             boxY + 20, 28, YELLOW);

    int defY = boxY + 60;
    for (auto it = m_definitionLines.begin(); it != m_definitionLines.end(); ++it) {
        DrawText(it->c_str(), boxX + 30, defY, 16, WHITE);
        defY += lineHeight;
    }

    const char* hint = "ENTER untuk kembali";
    DrawText(hint, (Config::screenWidth - MeasureText(hint, 14)) / 2,
             boxY + boxH - 25, 14, Color{200, 200, 200, 180});
}
