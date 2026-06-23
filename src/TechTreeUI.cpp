#include "TechTreeUI.h"
#include "GameConfig.h"
#include "SkinManager.h"
#include <string>

// ===============================
// 🖌️ UI TECH TREE
// ===============================

// Konstruktor: inisialisasi ref tree, default hover & tooltip ke BARRIER (tidak aktif)
TechTreeUI::TechTreeUI(TechTree& tree)
    : m_tree(tree), m_hoveredSkill(BARRIER), m_tooltipSkill(BARRIER), m_isTooltipActive(false) {}

// Update: deteksi hover mouse dan toggle tooltip dengan klik kanan
void TechTreeUI::Update() {
    Vector2 mouse = GetMousePosition();

    bool foundHover = false;
    // Cek apakah mouse berada di atas bounds salah satu node skill
    for (const auto& [key, skill] : m_tree.getSkills()) {
        if (CheckCollisionPointRec(mouse, skill.bounds)) {
            m_hoveredSkill = key;
            foundHover = true;

            // Klik kanan: toggle tooltip untuk skill yang di-hover
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                if (m_isTooltipActive && m_tooltipSkill == key) {
                    m_isTooltipActive = false;   // Tooltip sudah aktif untuk skill ini → matikan
                } else {
                    m_tooltipSkill = key;         // Aktifkan tooltip untuk skill ini
                    m_isTooltipActive = true;
                }
            }
            break;
        }
    }

    // Jika mouse tidak di atas node mana pun, reset hover & nonaktifkan tooltip
    if (!foundHover) {
        m_hoveredSkill = static_cast<SkillName>(-1);
        if (m_isTooltipActive) {
            m_isTooltipActive = false;
        }
    }
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
    Color fillColor = GetStateColor(skill.uiState);              // Warna berdasarkan state
    Color borderColor = isHovered ? WHITE : Color{255, 255, 255, 60};

    DrawRectangleRec(skill.bounds, fillColor);                   // Kotak background
    DrawRectangleLinesEx(skill.bounds, isHovered ? 3.0f : 1.0f, borderColor); // Border

    float centerX = skill.bounds.x + skill.bounds.width / 2;
    float baseY = skill.bounds.y;

    // Konversi enum ke teks untuk ditampilkan
    std::string nameText;
    switch (skill.name) {
        case AURA_FIELD:    nameText = "Aura Field"; break;
        case BARRIER:       nameText = "Barrier"; break;
        case SHOCKWAVE:     nameText = "Shockwave"; break;
        case INSTANT_CRIT:  nameText = "Instant Crit"; break;
        case SCORE_BOOSTER: nameText = "Score Booster"; break;
    }

    // Nama skill — rata tengah
    int nameW = MeasureText(nameText.c_str(), 20);
    DrawText(nameText.c_str(), centerX - nameW / 2, baseY + 18, 20, WHITE);

    // Biaya RP — rata tengah di bawah nama
    std::string costText = TextFormat("RP: %d", skill.rpCost);
    int costW = MeasureText(costText.c_str(), 14);
    DrawText(costText.c_str(), centerX - costW / 2, baseY + 44, 14, LIGHTGRAY);
}

// Gambar tooltip panel untuk satu skill — word-wrap otomatis, tinggi dinamis
void TechTreeUI::DrawTooltip(const SkillData& skill) {
    float panelW = 260;
    int maxTextW = (int)panelW - 20;  // Lebar maksimal teks dalam panel (dengan padding)
    int fontSize = 14;

    // Word-wrap: potong deskripsi menjadi baris-baris yang muat dalam panelW
    std::vector<std::string> descLines;
    std::string remaining = skill.desc;
    while (!remaining.empty()) {
        // Jika sisa teks muat dalam satu baris
        if (MeasureText(remaining.c_str(), fontSize) <= maxTextW) {
            descLines.push_back(remaining);
            break;
        }
        // Cari spasi terakhir yang masih muat dalam maxTextW
        size_t lastSpace = std::string::npos;
        for (size_t i = 0; i < remaining.size(); ++i) {
            if (remaining[i] == ' ') {
                std::string testLine = remaining.substr(0, i);
                if (MeasureText(testLine.c_str(), fontSize) > maxTextW)
                    break;
                lastSpace = i;
            }
        }
        // Jika tidak ada spasi yang muat, ambil sisa teks
        if (lastSpace == std::string::npos) {
            descLines.push_back(remaining);
            break;
        }
        descLines.push_back(remaining.substr(0, lastSpace));
        remaining = remaining.substr(lastSpace + 1);
    }

    // Tinggi panel = padding + nama + deskripsi + RP + status + petunjuk
    float panelH = 20 + 22 + (descLines.size() * 18) + 22 + 22 + 18 + 10;
    float panelX, panelY;

    // Posisi panel: kiri atau kanan node (hindari overflow layar)
    if (skill.position.x > Config::screenWidth / 2) {
        panelX = skill.bounds.x - panelW - 10;  // Panel di kiri node
    } else {
        panelX = skill.bounds.x + skill.bounds.width + 10;  // Panel di kanan node
    }
    panelY = skill.bounds.y;

    // Clamp posisi agar tidak keluar layar
    if (panelX < 10) panelX = 10;
    if (panelX + panelW > Config::screenWidth - 10) panelX = Config::screenWidth - panelW - 10;

    // Background panel semi-transparan + border cyan
    DrawRectangle(panelX, panelY, panelW, panelH, Color{0, 0, 0, 220});
    DrawRectangleLines(panelX, panelY, panelW, panelH, Color{0, 200, 255, 200});

    // Nama skill
    std::string nameText;
    switch (skill.name) {
        case AURA_FIELD:    nameText = "Aura Field"; break;
        case BARRIER:       nameText = "Barrier"; break;
        case SHOCKWAVE:     nameText = "Shockwave"; break;
        case INSTANT_CRIT:  nameText = "Instant Crit"; break;
        case SCORE_BOOSTER: nameText = "Score Booster"; break;
    }

    int curY = panelY + 8;
    DrawText(nameText.c_str(), panelX + 10, curY, 16, Color{0, 200, 255, 255});
    curY += 22;

    // Baris-baris deskripsi (word-wrapped)
    for (auto it = descLines.begin(); it != descLines.end(); ++it) {
        DrawText(it->c_str(), panelX + 10, curY, fontSize, WHITE);
        curY += 18;
    }

    // Biaya RP
    std::string costText = TextFormat("RP Cost: %d", skill.rpCost);
    DrawText(costText.c_str(), panelX + 10, curY, fontSize, YELLOW);
    curY += 22;

    // Status saat ini (dengan warna sesuai state)
    const char* stateText = "";
    Color stateColor = WHITE;
    switch (skill.uiState) {
        case LOCKED:    stateText = "Status: LOCKED";    stateColor = GRAY; break;
        case AVAILABLE: stateText = "Status: AVAILABLE"; stateColor = YELLOW; break;
        case UNLOCKED:  stateText = "Status: UNLOCKED";  stateColor = GREEN; break;
    }
    DrawText(stateText, panelX + 10, curY, fontSize, stateColor);
    curY += 22;

    // Petunjuk aksi
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
