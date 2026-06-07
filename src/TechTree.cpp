#include "TechTree.h"

TechTree::TechTree() {
    float boxW = 220.0f;
    float boxH = 75.0f;

    skills = {
        {SkillName::BARRIER,       {SkillName::BARRIER, 30, "Shield 1 hantaman", false,
                                   {540.0f, 100.0f}, {540.0f - boxW/2, 100.0f - boxH/2, boxW, boxH}, LOCKED}},

        {SkillName::AURA_FIELD,    {SkillName::AURA_FIELD, 100, "Shield aktif rentang waktu", false,
                                   {340.0f, 260.0f}, {340.0f - boxW/2, 260.0f - boxH/2, boxW, boxH}, LOCKED}},

        {SkillName::SHOCKWAVE,     {SkillName::SHOCKWAVE, 300, "Hancurkan asteroid sekitar", false,
                                   {740.0f, 260.0f}, {740.0f - boxW/2, 260.0f - boxH/2, boxW, boxH}, LOCKED}},

        {SkillName::CHRONO_STASIS, {SkillName::CHRONO_STASIS, 450, "Perlambat gerakan asteroid", false,
                                   {340.0f, 420.0f}, {340.0f - boxW/2, 420.0f - boxH/2, boxW, boxH}, LOCKED}},

        {SkillName::INSTANT_CRIT,  {SkillName::INSTANT_CRIT, 500, "Ketik huruf pertama untuk hancurkan", false,
                                   {620.0f, 420.0f}, {620.0f - boxW/2, 420.0f - boxH/2, boxW, boxH}, LOCKED}},

        {SkillName::SCORE_BOOSTER, {SkillName::SCORE_BOOSTER, 250, "Multiplier 16x score", false,
                                   {860.0f, 420.0f}, {860.0f - boxW/2, 420.0f - boxH/2, boxW, boxH}, LOCKED}}
    };

    addDependency(SkillName::BARRIER, SkillName::AURA_FIELD);
    addDependency(SkillName::BARRIER, SkillName::SHOCKWAVE);
    addDependency(SkillName::AURA_FIELD, SkillName::CHRONO_STASIS);
    addDependency(SkillName::SHOCKWAVE, SkillName::INSTANT_CRIT);
    addDependency(SkillName::SHOCKWAVE, SkillName::SCORE_BOOSTER);

    updateSkillStates();
}

void TechTree::addDependency(SkillName parent, SkillName child) {
    adjList[parent].push_back(child);
}

SkillName TechTree::SkillNameFromString(const std::string& str) {
    if (str == "barrier")       return BARRIER;
    if (str == "aura_field")    return AURA_FIELD;
    if (str == "shockwave")     return SHOCKWAVE;
    if (str == "chrono_stasis") return CHRONO_STASIS;
    if (str == "instant_crit")  return INSTANT_CRIT;
    if (str == "score_booster") return SCORE_BOOSTER;
    return BARRIER;
}

std::string TechTree::StringFromSkillName(SkillName name) {
    switch (name) {
        case BARRIER:       return "barrier";
        case AURA_FIELD:    return "aura_field";
        case SHOCKWAVE:     return "shockwave";
        case CHRONO_STASIS: return "chrono_stasis";
        case INSTANT_CRIT:  return "instant_crit";
        case SCORE_BOOSTER: return "score_booster";
    }
    return "";
}

void TechTree::loadFromProfile(const PlayerProfile& profile) {
    for (auto& [key, skill] : skills) {
        skill.isUnlocked = false;
    }

    for (const std::string& skillName : profile.unlocked_skills) {
        SkillName name = SkillNameFromString(skillName);
        skills[name].isUnlocked = true;
    }

    updateSkillStates();
}

void TechTree::saveToProfile(PlayerProfile& profile) {
    profile.unlocked_skills.clear();
    for (auto& [key, skill] : skills) {
        if (skill.isUnlocked) {
            profile.unlocked_skills.push_back(StringFromSkillName(skill.name));
        }
    }
}

bool TechTree::unlockSkill(SkillName name, PlayerProfile& profile) {
    if (profile.research_point >= skills[name].rpCost && skills[name].uiState == AVAILABLE) {
        profile.research_point -= skills[name].rpCost;
        skills[name].isUnlocked = true;
        profile.unlocked_skills.push_back(StringFromSkillName(name));
        updateSkillStates();
        return true;
    }
    return false;
}

bool TechTree::handleInput(Vector2 mousePos, bool isMouseClicked, PlayerProfile& profile) {
    if (!isMouseClicked) return false;

    for (auto& [key, skill] : skills) {
        if (CheckCollisionPointRec(mousePos, skill.bounds)) {
            if (skill.uiState == AVAILABLE) {
                return unlockSkill(skill.name, profile);
            }
            break;
        }
    }
    return false;
}

void TechTree::updateSkillStates() {
    for (auto& [key, skill] : skills) {
        if (skill.isUnlocked) {
            skill.uiState = UNLOCKED;
        } else {
            skill.uiState = LOCKED;
        }
    }

    std::queue<SkillName> q;
    std::unordered_map<SkillName, bool> visited;

    q.push(SkillName::BARRIER);
    visited[SkillName::BARRIER] = true;

    while (!q.empty()) {
        SkillName current = q.front();
        q.pop();

        if (skills[current].isUnlocked) {
            for (SkillName child : adjList[current]) {
                if (!visited[child]) {
                    visited[child] = true;
                    if (!skills[child].isUnlocked) {
                        skills[child].uiState = AVAILABLE;
                    } else {
                        q.push(child);
                    }
                }
            }
        }
    }

    if (skills[SkillName::BARRIER].uiState == LOCKED) {
        skills[SkillName::BARRIER].uiState = AVAILABLE;
    }
}

const std::unordered_map<SkillName, SkillData>& TechTree::getSkills() const { return skills; }
const std::unordered_map<SkillName, std::vector<SkillName>>& TechTree::getAdjList() const { return adjList; }
