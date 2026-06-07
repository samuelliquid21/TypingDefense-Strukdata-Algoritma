#pragma once
#include <string>
#include "DataManager.h"

struct WordNode {
    std::string word;
    std::string difficulty;
    std::string definition;
    WordNode* prev;
    WordNode* next;
    WordNode(const std::string& w, const std::string& d, const std::string& def);
};

class UnlockedWords {
public:
    UnlockedWords();
    ~UnlockedWords();
    void Update();
    void Draw();
    void Reset();
    void BuildFromPlayer(const PlayerProfile& profile);
    bool WantsToGoBack();

private:
    WordNode* m_head;
    WordNode* m_current;
    int m_selectedIndex;
    int m_nodeCount;
    int m_scrollOffset;
    bool m_showDefinition;
    bool m_requestBack;

    void ClearList();
    void DrawWordList();
    void DrawDefinitionPopup();
};
