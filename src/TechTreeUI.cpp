#include "TechTreeUI.h"
#include "GameConfig.h"
#include "SkinManager.h"
#include <string>

namespace {
    // Helper: konversi enum SkillName ke teks untuk ditampilkan
    std::string SkillNameToDisplayText(SkillName name) {
        switch (name) {
            case AURA_FIELD:    return "Aura Field";
            case BARRIER:       return "Barrier";
            case SHOCKWAVE:     return "Shockwave";
            case INSTANT_CRIT:  return "Instant Crit";
            case SCORE_BOOSTER: return "Score Booster";
        }
        return "";
    }
}

// ===============================
// 🖌️ UI TECH TREE
// ===============================

// Konstruktor: inisialisasi ref tree, default hover & tooltip ke BARRIER (tidak aktif)
TechTreeUI::TechTreeUI(TechTree& tree)
    : m_tree(tree), m_hoveredSkill(BARRIER), m_tooltipSkill(BARRIER), m_isTooltipActive(false) {}

// Update: deteksi hover mouse dan toggle tooltip dengan klik kanan
void TechTreeUI::Update() {
    Vector2 mouse = GetMousePosition();
    if (!HandleHover(mouse)) {
        m_hoveredSkill = static_cast<SkillName>(-1);
        if (m_isTooltipActive) {
            m_isTooltipActive = false;
        }
    }
}

// Helper: handle hover mouse dan toggle tooltip dengan klik kanan
bool TechTreeUI::HandleHover(Vector2 mouse) {
    for (const auto& [key, skill] : m_tree.getSkills()) {
        if (CheckCollisionPointRec(mouse, skill.bounds)) {
            m_hoveredSkill = key;
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                if (m_isTooltipActive && m_tooltipSkill == key) {
                    m_isTooltipActive = false;
                } else {
                    m_tooltipSkill = key;
                    m_isTooltipActive = true;
                }
            }
            return true;
        }
    }
    return false;
}

/* Draw: render seluruh Tech Tree UI.
 * Pipeline: background → judul → RP → edges → node (hanya yang tidak LOCKED)
 *           → tooltip (jika aktif) → help text */
void TechTreeUI::Draw(const PlayerProfile& profile) {
    ClearBackground(Color{10, 10, 25, 255});

    DrawTitle();                                 // Judul "SKILL LAB" di tengah atas
    DrawResearchPoints(SkinManager::getInstance().getRP());  // Jumlah RP di pojok kanan atas
    DrawEdges();                                 // Garis dependensi antar node

    // Gambar node yang tidak LOCKED
    for (const auto& [key, skill] : m_tree.getSkills()) {
        if (skill.uiState == LOCKED) continue;   // Skip node yang masih terkunci
        bool isHovered = (key == m_hoveredSkill);
        DrawNode(skill, isHovered);
    }

    // Tooltip: hanya tampil jika skill valid dan tidak LOCKED
    if (m_isTooltipActive) {
        const auto& skills = m_tree.getSkills();
        if (skills.count(m_tooltipSkill) && skills.at(m_tooltipSkill).uiState != LOCKED) {
            DrawTooltip(skills.at(m_tooltipSkill));
        }
    }

    DrawHelpText(); // Petunjuk kontrol di bagian bawah layar
}

// Gambar garis dari parent ke child untuk node yang tidak LOCKED
void TechTreeUI::DrawEdges() {
    for (const auto& [parent, children] : m_tree.getAdjList()) {
        // Skip jika parent tidak ada atau masih LOCKED
        if (!m_tree.getSkills().count(parent)) continue;
        if (m_tree.getSkills().at(parent).uiState == LOCKED) continue;

        Vector2 start = m_tree.getSkills().at(parent).position;

        for (SkillName child : children) {
            // Skip child yang tidak ada atau masih LOCKED
            if (!m_tree.getSkills().count(child)) continue;
            if (m_tree.getSkills().at(child).uiState == LOCKED) continue;
            Vector2 end = m_tree.getSkills().at(child).position;
            DrawLineEx(start, end, 2.0f, Color{255, 255, 255, 60});
        }
    }
}

// Gambar satu node skill: background, border, nama, dan biaya RP
void TechTreeUI::DrawNode(const SkillData& skill, bool isHovered) {
    Color fillColor = GetStateColor(skill.uiState);
    Color borderColor = isHovered ? WHITE : Color{255, 255, 255, 60};
    DrawRectangleRec(skill.bounds, fillColor);
    DrawRectangleLinesEx(skill.bounds, isHovered ? 3.0f : 1.0f, borderColor);
    float centerX = skill.bounds.x + skill.bounds.width / 2;
    float baseY = skill.bounds.y;
    std::string nameText = SkillNameToDisplayText(skill.name);
    int nameW = MeasureText(nameText.c_str(), 20);
    DrawText(nameText.c_str(), centerX - nameW / 2, baseY + 18, 20, WHITE);
    std::string costText = TextFormat("RP: %d", skill.rpCost);
    int costW = MeasureText(costText.c_str(), 14);
    DrawText(costText.c_str(), centerX - costW / 2, baseY + 44, 14, LIGHTGRAY);
}

// Gambar tooltip panel untuk satu skill — word-wrap otomatis, tinggi dinamis
void TechTreeUI::DrawTooltip(const SkillData& skill) {
    float panelW = 260;
    int maxTextW = (int)panelW - 20;
    int fontSize = 14;
    std::vector<std::string> descLines;
    WordWrapDescription(skill.desc, maxTextW, fontSize, descLines);
    float panelH = 20 + 22 + (descLines.size() * 18) + 22 + 22 + 18 + 10;
    float panelX, panelY;
    ComputeTooltipPosition(skill, (int)panelW, (int)panelH, panelX, panelY);
    DrawTooltipBox(panelX, panelY, panelW, panelH);
    DrawTooltipHeader(skill, panelX, panelY);
    DrawTooltipBody(skill, descLines, panelX, panelY);
}

// Helper: word-wrap deskripsi skill ke baris-baris yang muat
void TechTreeUI::WordWrapDescription(const std::string& text, int maxTextW, int fontSize, std::vector<std::string>& outLines) {
    std::string remaining = text;
    while (!remaining.empty()) {
        if (MeasureText(remaining.c_str(), fontSize) <= maxTextW) {
            outLines.push_back(remaining);
            break;
        }
        size_t lastSpace = std::string::npos;
        for (size_t i = 0; i < remaining.size(); ++i) {
            if (remaining[i] == ' ') {
                std::string testLine = remaining.substr(0, i);
                if (MeasureText(testLine.c_str(), fontSize) > maxTextW)
                    break;
                lastSpace = i;
            }
        }
        if (lastSpace == std::string::npos) {
            outLines.push_back(remaining);
            break;
        }
        outLines.push_back(remaining.substr(0, lastSpace));
        remaining = remaining.substr(lastSpace + 1);
    }
}

// Helper: hitung posisi panel tooltip (kiri/kanan node) dengan clamping
void TechTreeUI::ComputeTooltipPosition(const SkillData& skill, int panelW, int panelH, float& panelX, float& panelY) {
    if (skill.position.x > Config::screenWidth / 2) {
        panelX = skill.bounds.x - panelW - 10;
    } else {
        panelX = skill.bounds.x + skill.bounds.width + 10;
    }
    panelY = skill.bounds.y;
    if (panelX < 10) panelX = 10;
    if (panelX + panelW > Config::screenWidth - 10) panelX = Config::screenWidth - panelW - 10;
}

// Helper: render background tooltip
void TechTreeUI::DrawTooltipBox(float panelX, float panelY, float panelW, float panelH) {
    DrawRectangle(panelX, panelY, panelW, panelH, Color{0, 0, 0, 220});
    DrawRectangleLines(panelX, panelY, panelW, panelH, Color{0, 200, 255, 200});
}

// Helper: render header tooltip (nama skill)
void TechTreeUI::DrawTooltipHeader(const SkillData& skill, float panelX, float panelY) {
    std::string nameText = SkillNameToDisplayText(skill.name);
    DrawText(nameText.c_str(), panelX + 10, panelY + 8, 16, Color{0, 200, 255, 255});
}

// Helper: render body tooltip (desc, RP cost, status, hint)
void TechTreeUI::DrawTooltipBody(const SkillData& skill, const std::vector<std::string>& descLines, float panelX, float panelY) {
    int curY = panelY + 30;
    for (auto it = descLines.begin(); it != descLines.end(); ++it) {
        DrawText(it->c_str(), panelX + 10, curY, 14, WHITE);
        curY += 18;
    }
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

// Tampilkan jumlah Research Points di pojok kanan atas layar
void TechTreeUI::DrawResearchPoints(int rp) {
    std::string text = TextFormat("Research Points: %d", rp);
    int textW = MeasureText(text.c_str(), 20);
    DrawText(text.c_str(), Config::screenWidth - textW - 20, 20, 20, Color{0, 255, 200, 255});
}

// Tampilkan petunjuk kontrol di bagian bawah layar (rata tengah)
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

// Gambar judul "SKILL LAB" di tengah atas layar
void TechTreeUI::DrawTitle() {
    const char* title = "SKILL LAB";
    int titleW = MeasureText(title, 30);
    DrawText(title, (Config::screenWidth - titleW) / 2, 20, 30, WHITE);
}

// Dapatkan warna untuk node berdasarkan state-nya
Color TechTreeUI::GetStateColor(SkillState state) {
    switch (state) {
        case LOCKED:    return Color{50, 50, 60, 255};
        case AVAILABLE: return Color{0, 200, 255, 220};
        case UNLOCKED:  return Color{0, 230, 150, 220};
    }
    return DARKGRAY;
}
