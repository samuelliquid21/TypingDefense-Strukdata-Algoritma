#include "TechTreeUI.h"
#include "GameConfig.h"
#include <string>

TechTreeUI::TechTreeUI(TechTree& tree)
    : m_tree(tree), m_hoveredSkill(BARRIER), m_tooltipSkill(BARRIER), m_isTooltipActive(false) {}

void TechTreeUI::Update() {
    Vector2 mouse = GetMousePosition();

    bool foundHover = false;
    for (const auto& [key, skill] : m_tree.getSkills()) {
        if (CheckCollisionPointRec(mouse, skill.bounds)) {
            m_hoveredSkill = key;
            foundHover = true;

            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                if (m_isTooltipActive && m_tooltipSkill == key) {
                    m_isTooltipActive = false;
                } else {
                    m_tooltipSkill = key;
                    m_isTooltipActive = true;
                }
            }
            break;
        }
    }

    if (!foundHover) {
        m_hoveredSkill = static_cast<SkillName>(-1);
        if (m_isTooltipActive) {
            m_isTooltipActive = false;
        }
    }
}

void TechTreeUI::Draw(const PlayerProfile& profile) {
    ClearBackground(Color{10, 10, 25, 255});

    DrawTitle();
    DrawResearchPoints(profile.research_point);
    DrawEdges();

    for (const auto& [key, skill] : m_tree.getSkills()) {
        if (skill.uiState == LOCKED) continue;
        bool isHovered = (key == m_hoveredSkill);
        DrawNode(skill, isHovered);
    }

    if (m_isTooltipActive) {
        const auto& skills = m_tree.getSkills();
        if (skills.count(m_tooltipSkill) && skills.at(m_tooltipSkill).uiState != LOCKED) {
            DrawTooltip(skills.at(m_tooltipSkill));
        }
    }

    DrawHelpText();
}

void TechTreeUI::DrawEdges() {
    for (const auto& [parent, children] : m_tree.getAdjList()) {
        if (!m_tree.getSkills().count(parent)) continue;
        if (m_tree.getSkills().at(parent).uiState == LOCKED) continue;

        Vector2 start = m_tree.getSkills().at(parent).position;

        for (SkillName child : children) {
            if (!m_tree.getSkills().count(child)) continue;
            if (m_tree.getSkills().at(child).uiState == LOCKED) continue;
            Vector2 end = m_tree.getSkills().at(child).position;
            DrawLineEx(start, end, 2.0f, Color{255, 255, 255, 60});
        }
    }
}

void TechTreeUI::DrawNode(const SkillData& skill, bool isHovered) {
    Color fillColor = GetStateColor(skill.uiState);
    Color borderColor = isHovered ? WHITE : Color{255, 255, 255, 60};

    DrawRectangleRec(skill.bounds, fillColor);
    DrawRectangleLinesEx(skill.bounds, isHovered ? 3.0f : 1.0f, borderColor);

    float centerX = skill.bounds.x + skill.bounds.width / 2;
    float baseY = skill.bounds.y;

    std::string nameText;
    switch (skill.name) {
        case BARRIER:       nameText = "Barrier"; break;
        case AURA_FIELD:    nameText = "Aura Field"; break;
        case SHOCKWAVE:     nameText = "Shockwave"; break;
        case CHRONO_STASIS: nameText = "Chrono Stasis"; break;
        case INSTANT_CRIT:  nameText = "Instant Crit"; break;
        case SCORE_BOOSTER: nameText = "Score Booster"; break;
    }

    int nameW = MeasureText(nameText.c_str(), 20);
    DrawText(nameText.c_str(), centerX - nameW / 2, baseY + 18, 20, WHITE);

    std::string costText = TextFormat("RP: %d", skill.rpCost);
    int costW = MeasureText(costText.c_str(), 14);
    DrawText(costText.c_str(), centerX - costW / 2, baseY + 44, 14, LIGHTGRAY);
}

void TechTreeUI::DrawTooltip(const SkillData& skill) {
    float panelW = 260;
    float panelH = 130;
    float panelX, panelY;

    if (skill.position.x > Config::screenWidth / 2) {
        panelX = skill.bounds.x - panelW - 10;
    } else {
        panelX = skill.bounds.x + skill.bounds.width + 10;
    }
    panelY = skill.bounds.y;

    if (panelX < 10) panelX = 10;
    if (panelX + panelW > Config::screenWidth - 10) panelX = Config::screenWidth - panelW - 10;

    DrawRectangle(panelX, panelY, panelW, panelH, Color{0, 0, 0, 220});
    DrawRectangleLines(panelX, panelY, panelW, panelH, Color{0, 200, 255, 200});

    std::string nameText;
    switch (skill.name) {
        case BARRIER:       nameText = "Barrier"; break;
        case AURA_FIELD:    nameText = "Aura Field"; break;
        case SHOCKWAVE:     nameText = "Shockwave"; break;
        case CHRONO_STASIS: nameText = "Chrono Stasis"; break;
        case INSTANT_CRIT:  nameText = "Instant Crit"; break;
        case SCORE_BOOSTER: nameText = "Score Booster"; break;
    }

    int curY = panelY + 8;
    DrawText(nameText.c_str(), panelX + 10, curY, 16, Color{0, 200, 255, 255});
    curY += 22;

    DrawText(skill.desc.c_str(), panelX + 10, curY, 14, WHITE);
    curY += 22;

    std::string costText = TextFormat("RP Cost: %d", skill.rpCost);
    DrawText(costText.c_str(), panelX + 10, curY, 14, YELLOW);
    curY += 22;

    const char* stateText = "";
    Color stateColor = WHITE;
    switch (skill.uiState) {
        case LOCKED:    stateText = "Status: LOCKED";    stateColor = GRAY; break;
        case AVAILABLE: stateText = "Status: AVAILABLE"; stateColor = YELLOW; break;
        case UNLOCKED:  stateText = "Status: UNLOCKED";  stateColor = GREEN; break;
    }
    DrawText(stateText, panelX + 10, curY, 14, stateColor);
    curY += 22;

    DrawText("Klik kiri untuk unlock", panelX + 10, curY, 12, Color{0, 200, 255, 200});
}

void TechTreeUI::DrawResearchPoints(int rp) {
    std::string text = TextFormat("Research Points: %d", rp);
    int textW = MeasureText(text.c_str(), 20);
    DrawText(text.c_str(), Config::screenWidth - textW - 20, 20, 20, Color{0, 255, 200, 255});
}

void TechTreeUI::DrawHelpText() {
    int y = Config::screenHeight - 60;
    int r = 12;
    const char* line1 = "Klik kiri pada skill AVAILABLE untuk unlock";
    const char* line2 = "Klik kanan untuk melihat informasi skill";
    const char* line3 = "ESC untuk kembali";

    DrawText(line1, (Config::screenWidth - MeasureText(line1, r)) / 2, y, r, Color{200, 200, 200, 180});
    DrawText(line2, (Config::screenWidth - MeasureText(line2, r)) / 2, y + 16, r, Color{200, 200, 200, 180});
    DrawText(line3, (Config::screenWidth - MeasureText(line3, r)) / 2, y + 32, r, Color{200, 200, 200, 180});
}

void TechTreeUI::DrawTitle() {
    const char* title = "SKILL UNLOCK";
    int titleW = MeasureText(title, 30);
    DrawText(title, (Config::screenWidth - titleW) / 2, 20, 30, WHITE);
}

Color TechTreeUI::GetStateColor(SkillState state) {
    switch (state) {
        case LOCKED:    return Color{50, 50, 60, 255};
        case AVAILABLE: return Color{0, 80, 180, 200};
        case UNLOCKED:  return Color{0, 150, 60, 200};
    }
    return DARKGRAY;
}
