#pragma once
#include "raylib.h"
#include "TechTree.h"

// UI untuk render Tech Tree — menampilkan node, edge, tooltip, dan informasi RP
class TechTreeUI {
public:
    TechTreeUI(TechTree& tree);

    void Update();                           // Update hover state & tooltip toggle
    void Draw(const PlayerProfile& profile); // Render seluruh Tech Tree UI

private:
    TechTree& m_tree;           // Referensi ke TechTree (data skill)
    SkillName m_hoveredSkill;   // Skill yang sedang di-hover mouse
    SkillName m_tooltipSkill;   // Skill yang tooltip-nya sedang aktif
    bool m_isTooltipActive;     // Apakah tooltip sedang ditampilkan

    void DrawEdges();                                        // Gambar garis dependensi antar node
    void DrawNode(const SkillData& skill, bool isHovered);   // Gambar satu node skill
    void DrawTooltip(const SkillData& skill);                // Gambar panel informasi skill
    void DrawResearchPoints(int rp);                         // Tampilkan jumlah RP di pojok kanan
    void DrawTitle();                                        // Gambar judul "SKILL LAB"
    void DrawHelpText();                                     // Tampilkan petunjuk kontrol di bawah

    static Color GetStateColor(SkillState state);            // Dapatkan warna berdasarkan state

    // Helper untuk Update: handle hover dan klik kanan
    bool HandleHover(Vector2 mouse);
    // Helper untuk DrawTooltip: word-wrap deskripsi
    void WordWrapDescription(const std::string& text, int maxTextW, int fontSize, std::vector<std::string>& outLines);
    // Helper untuk DrawTooltip: hitung posisi panel
    void ComputeTooltipPosition(const SkillData& skill, int panelW, int panelH, float& panelX, float& panelY);
    // Helper untuk DrawTooltip: render background panel
    void DrawTooltipBox(float panelX, float panelY, float panelW, float panelH);
    // Helper untuk DrawTooltip: render header (nama skill)
    void DrawTooltipHeader(const SkillData& skill, float panelX, float panelY);
    // Helper untuk DrawTooltip: render body (desc, cost, status, hint)
    void DrawTooltipBody(const SkillData& skill, const std::vector<std::string>& descLines, float panelX, float panelY);
};
