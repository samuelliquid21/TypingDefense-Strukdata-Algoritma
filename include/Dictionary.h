#pragma once
#include <string>
#include <vector>

struct DictionaryEntry {
    std::string word;
    std::string difficulty;
};

class Dictionary {
public:
    Dictionary();
    void Update();
    void Draw();
    void Reset();
    bool WantsToGoBack();

private:
    std::vector<DictionaryEntry> m_entries;
    std::vector<std::string> m_wordStrings;
    std::vector<DictionaryEntry> m_filteredEntries;
    std::vector<std::string> m_definitionLines;

    int m_selectedIndex;
    int m_scrollOffset;
    bool m_showDefinition;
    bool m_requestBack;
    std::string m_currentWord;
    std::string m_currentDefinition;
    std::string m_searchQuery;

    int m_easyCount;
    int m_mediumCount;
    int m_hardCount;

    void LoadWords();
    void ApplyFilter();
    void WordWrapDefinition();
    void DrawWordList();
    void DrawDefinitionPopup();
};
