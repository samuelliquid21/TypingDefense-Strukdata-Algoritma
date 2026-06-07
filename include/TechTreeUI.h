#pragma once
#include "raylib.h"
#include "TechTree.h"

class TechTreeUI {
public:
    TechTreeUI(TechTree& tree);

    void Update();
    void Draw(const PlayerProfile& profile);

private:
    TechTree& m_tree;
    SkillName m_hoveredSkill;
    SkillName m_tooltipSkill;
    bool m_isTooltipActive;

    void DrawEdges();
    void DrawNode(const SkillData& skill, bool isHovered);
    void DrawTooltip(const SkillData& skill);
    void DrawResearchPoints(int rp);
    void DrawTitle();
    void DrawHelpText();

    static Color GetStateColor(SkillState state);
};
