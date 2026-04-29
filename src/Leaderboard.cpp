#include "Leaderboard.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <fstream>
#include "../ext/nlohmann/json.hpp"

using json = nlohmann::json;

namespace LeaderboardSystem {

    static std::vector<PlayerData> players;
    static std::vector<PlayerData> sortedPlayers;
    static int selectedIndex = 0;
    static const int MAX_DISPLAY = 10;
    static bool showingDetail = false;
    static int detailIndex = -1;
    static float fadeIn = 0.0f;
    static float titlePulse = 0.0f;
    static float podiumRise[3] = {0,0,0};
    static float rowSlideIn[10] = {0};
    static float scanLinePos = 0.0f;
    static bool isEntering = true;
    static bool isExiting = false;
    static float enterTimer = 0.0f;
    static float exitTimer = 0.0f;
    static const float ENTER_DURATION = 0.6f;
    static const float EXIT_DURATION = 0.5f;
    static float glitchIntensity = 1.0f;
    struct Star { Vector2 pos; float speed, size, alpha; };
    static std::vector<Star> stars;
    static Music bgMusic = {0};
    static Sound sfxClick = {0};
    static Sound sfxGlitchMasuk = {0};
    static Sound sfxGlitchKeluar = {0};
    static bool hasMusic = false, hasClick = false, hasGlitchMasuk = false, hasGlitchKeluar = false;
    static float musicVolume = 0.0f;
    static const float MAX_VOLUME = 0.5f;
    static bool glitchEnterPlayed = false, glitchExitPlayed = false;
    static const int SW = 1080, SH = 720;

    void SortPlayers() {
        sortedPlayers = players;
        std::sort(sortedPlayers.begin(), sortedPlayers.end(), [](const PlayerData& a, const PlayerData& b) { return a.score > b.score; });
        for (size_t i = 0; i < sortedPlayers.size(); i++) sortedPlayers[i].rank = (int)i + 1;
    }

    void InitStars() {
        stars.clear();
        for (int i = 0; i < 90; i++) {
            Star s; s.pos = {(float)GetRandomValue(0,SW),(float)GetRandomValue(0,SH)};
            s.speed = (float)GetRandomValue(5,25)/10.0f; s.size = (float)GetRandomValue(1,3);
            s.alpha = (float)GetRandomValue(20,60)/100.0f; stars.push_back(s);
        }
    }

    void FullReset() {
        selectedIndex = 0; showingDetail = false; detailIndex = -1; fadeIn = 0.0f; scanLinePos = 0.0f;
        for (int i = 0; i < 3; i++) podiumRise[i] = 0.0f;
        for (int i = 0; i < 10; i++) rowSlideIn[i] = 0.0f;
        isEntering = true; isExiting = false; enterTimer = 0.0f; exitTimer = 0.0f;
        glitchIntensity = 1.0f; glitchEnterPlayed = false; glitchExitPlayed = false; musicVolume = 0.0f;
        InitStars();
    }

    void UpdateStars() { for (auto& s : stars) { s.pos.y += s.speed; if (s.pos.y > SH+5) { s.pos.y = -5; s.pos.x = (float)GetRandomValue(0,SW); } } }

    void DrawStars() {
        float da = fadeIn; if (isEntering) da = 1.0f - glitchIntensity; if (isExiting) da = 1.0f - glitchIntensity; if (da < 0) da = 0;
        for (auto& s : stars) { float a = s.alpha * da; if (a > 0.01f) DrawCircleV(s.pos, s.size, {255,255,255,(unsigned char)(a*200)}); }
    }

    void LoadAudio() {
        if (hasMusic) { StopMusicStream(bgMusic); UnloadMusicStream(bgMusic); hasMusic = false; }
        if (hasClick) { UnloadSound(sfxClick); hasClick = false; }
        if (hasGlitchMasuk) { UnloadSound(sfxGlitchMasuk); hasGlitchMasuk = false; }
        if (hasGlitchKeluar) { UnloadSound(sfxGlitchKeluar); hasGlitchKeluar = false; }
        if (FileExists("assets/sound/bgm.mp3")) { bgMusic = LoadMusicStream("assets/sound/bgm.mp3"); SetMusicVolume(bgMusic,0); PlayMusicStream(bgMusic); hasMusic = true; }
        if (FileExists("assets/sound/click.mp3")) { sfxClick = LoadSound("assets/sound/click.mp3"); hasClick = true; }
        if (FileExists("assets/sound/glitchmasuk.mp3")) { sfxGlitchMasuk = LoadSound("assets/sound/glitchmasuk.mp3"); hasGlitchMasuk = true; }
        if (FileExists("assets/sound/glitchkeluar.mp3")) { sfxGlitchKeluar = LoadSound("assets/sound/glitchkeluar.mp3"); hasGlitchKeluar = true; }
        glitchEnterPlayed = false; glitchExitPlayed = false; isEntering = true;
    }

    void LoadFromJSON(const std::string& fp) {
        std::ifstream f(fp); if (!f.is_open()) return;
        std::string s, l;
        while (std::getline(f, l)) { if (l.find("<<<<<<<")==std::string::npos && l.find("=======")==std::string::npos && l.find(">>>>>>>")==std::string::npos) s += l + "\n"; }
        f.close();
        json j; try { j = json::parse(s); } catch (...) { return; }
        players.clear();
        if (j.contains("data")) for (auto& it : j["data"]) {
            PlayerData p; p.name = it.value("username","?"); p.score = it.value("score",0); p.accuracy = it.value("accuracy",0.0f); players.push_back(p);
        }
        SortPlayers();
    }

    void DrawBg() {
        DrawRectangle(0,0,SW,SH,{0,0,0,170});
        for (int x=0;x<SW;x+=40) for (int y=0;y<SH;y+=40) DrawRectangle(x,y,1,1,{0,180,200,18});
        DrawStars();
        DrawRectangle(0,(int)scanLinePos,SW,2,{0,255,255,5});
        if ((isEntering||isExiting) && glitchIntensity>0.01f) { for (int i=0;i<10;i++) { int y=GetRandomValue(0,SH),h=GetRandomValue(2,15); DrawRectangle(0,y,SW,h,(i%2==0)?(Color){0,255,200,(unsigned char)(glitchIntensity*180)}:(Color){255,50,120,(unsigned char)(glitchIntensity*140)}); } }
    }

    void DrawHeader() {
        float a = (isEntering||isExiting) ? 0 : fadeIn;
        DrawRectangle(SW/2-250,18,500,55,{0,0,0,170});
        DrawRectangleLinesEx({(float)(SW/2-250),18,500,55},1.5f,ColorAlpha({0,220,200,255},a));
        const char* t = "HALL OF CHAMPIONS"; int tw = MeasureText(t,36);
        DrawText(t,SW/2-tw/2,28,36,ColorAlpha({0,245,225,255},a));
    }

    void DrawPodium(float cx, float cy, float w, float h, int rank, const PlayerData* p) {
        float a = (isEntering||isExiting) ? 0 : fadeIn * podiumRise[rank-1];
        Color c; if (rank==1) c={255,195,35,255}; else if (rank==2) c={185,195,210,255}; else c={195,130,45,255};
        float ct = cy - h/2;
        DrawRectangle(cx-w/2,ct,w,h,ColorAlpha({6,18,28,255},a*0.95f));
        DrawRectangleLinesEx({cx-w/2,ct,w,h},1.5f,ColorAlpha(c,a));
        if (p) {
            if (rank==1) { DrawRectangle(cx-15,ct-20,30,5,ColorAlpha(GOLD,a)); DrawRectangle(cx-15,ct-28,6,12,ColorAlpha(GOLD,a)); DrawRectangle(cx-3,ct-32,6,16,ColorAlpha(GOLD,a)); DrawRectangle(cx+9,ct-28,6,12,ColorAlpha(GOLD,a)); }
            int nw = MeasureText(p->name.c_str(),20); DrawText(p->name.c_str(),cx-nw/2,ct+15,20,ColorAlpha(WHITE,a));
            const char* sc = TextFormat("%d",p->score); int sw = MeasureText(sc,26); DrawText(sc,cx-sw/2,ct+40,26,ColorAlpha({0,235,215,255},a));
        } else { DrawText("---",cx-18,ct+h/2-8,18,ColorAlpha({60,100,100,255},a*0.4f)); }
    }

    void DrawRow(const PlayerData& p, int idx, float y, bool sel) {
        float a = (isEntering||isExiting) ? 0 : rowSlideIn[idx] * fadeIn;
        DrawRectangle(70,y,SW-140,40,ColorAlpha(sel?(Color){18,48,58,200}:(Color){8,15,22,200},a));
        if (sel) { DrawRectangle(70,y,4,40,ColorAlpha({0,245,225,255},a)); DrawText(">",82,y+12,16,ColorAlpha({0,245,225,255},a)); }
        DrawText(TextFormat("#%02d",p.rank),100,y+11,17,ColorAlpha({180,200,210,255},a));
        DrawText(p.name.c_str(),165,y+11,17,ColorAlpha(WHITE,a));
        DrawText(TextFormat("%.1f%%",p.accuracy),510,y+12,15,ColorAlpha({160,200,200,255},a));
        const char* sc = TextFormat("%d",p.score); DrawText(sc,SW-100-MeasureText(sc,17),y+11,17,ColorAlpha({0,235,215,255},a));
    }

    void DrawFooter() {
        float a = (isEntering||isExiting) ? 0 : fadeIn;
        DrawText("The Typing Guardians",60,SH-30,13,ColorAlpha({120,200,200,255},a*0.55f));
        DrawText("[ W/S ] NAVIGATE   [ ENTER ] DETAIL   [ ESC ] EXIT",SW-470,SH-30,13,ColorAlpha({120,200,200,255},a*0.55f));
    }

    void Init() { FullReset(); LoadAudio(); LoadFromJSON("data/data.json"); }
    void AddPlayerData(const PlayerData& d) { players.push_back(d); SortPlayers(); }
    void ClearData() { players.clear(); sortedPlayers.clear(); }
    int GetPlayerCount() { return (int)players.size(); }

    void Update(bool& back) {
        float dt = GetFrameTime();
        if (hasMusic && bgMusic.stream.buffer) {
            UpdateMusicStream(bgMusic);
            if (!isExiting && musicVolume<MAX_VOLUME) { musicVolume+=dt*0.5f; if (musicVolume>MAX_VOLUME) musicVolume=MAX_VOLUME; SetMusicVolume(bgMusic,musicVolume); }
            if (isExiting) { musicVolume-=dt*3.0f; if (musicVolume<0) musicVolume=0; SetMusicVolume(bgMusic,musicVolume); }
        }
        scanLinePos+=dt*150; if (scanLinePos>SH) scanLinePos=-5;
        UpdateStars();
        if (isEntering) {
            enterTimer+=dt; glitchIntensity=1-(enterTimer/ENTER_DURATION); if (glitchIntensity<0) glitchIntensity=0;
            if (!glitchEnterPlayed && enterTimer>0.05f) { if (hasGlitchMasuk) PlaySound(sfxGlitchMasuk); glitchEnterPlayed=true; }
            if (enterTimer>=ENTER_DURATION) { isEntering=false; glitchIntensity=0; }
        }
        if (isExiting) {
            exitTimer+=dt; glitchIntensity=exitTimer/EXIT_DURATION; if (glitchIntensity>1) glitchIntensity=1;
            if (!glitchExitPlayed && exitTimer>0.05f) { if (hasGlitchKeluar) PlaySound(sfxGlitchKeluar); glitchExitPlayed=true; }
            if (exitTimer>=EXIT_DURATION) { isExiting=false; back=true; } return;
        }
        if (!isEntering && fadeIn<1) { fadeIn+=dt*1.8f; if (fadeIn>1) fadeIn=1; }
        if (fadeIn>0.2f) { podiumRise[0]=std::min(podiumRise[0]+dt*2.0f,1.0f); podiumRise[1]=std::min(podiumRise[1]+dt*2.3f,1.0f); podiumRise[2]=std::min(podiumRise[2]+dt*2.3f,1.0f); }
        if (fadeIn>0.5f) { for (int i=0;i<10;i++) if (i<(int)sortedPlayers.size()-3) rowSlideIn[i]=std::min(rowSlideIn[i]+dt*1.8f,1.0f); }
        if (showingDetail) { if (IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_ESCAPE)) { showingDetail=false; if (hasClick) PlaySound(sfxClick); } return; }
        int total=(int)sortedPlayers.size(), maxIdx=total>0?std::min(total,MAX_DISPLAY)-1:0;
        if ((IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP))&&selectedIndex>0) { selectedIndex--; if (hasClick) PlaySound(sfxClick); }
        if ((IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN))&&selectedIndex<maxIdx) { selectedIndex++; if (hasClick) PlaySound(sfxClick); }
        if (IsKeyPressed(KEY_ENTER)&&selectedIndex<total) { showingDetail=true; detailIndex=selectedIndex; if (hasClick) PlaySound(sfxClick); }
        if (IsKeyPressed(KEY_ESCAPE)||IsKeyPressed(KEY_BACKSPACE)) { isExiting=true; isEntering=false; exitTimer=0; glitchExitPlayed=false; }
    }

    void Draw() {
        DrawBg(); DrawHeader();
        float py=200;
        struct { float cx,w,h; int r,i; } pd[3] = {{290,215,145,2,1},{540,245,165,1,0},{790,215,145,3,2}};
        PlayerData* t3[3] = {0}; for (int i=0;i<3&&i<(int)sortedPlayers.size();i++) t3[i]=&sortedPlayers[i];
        for (int i=0;i<3;i++) DrawPodium(pd[i].cx,py,pd[i].w,pd[i].h,pd[i].r,t3[pd[i].i]);
        float sy=370, a=(isEntering||isExiting)?0:fadeIn;
        DrawText("RANK",95,sy+10,12,ColorAlpha({200,170,40,255},a));
        DrawText("PLAYER",165,sy+10,12,ColorAlpha({200,170,40,255},a));
        DrawText("ACC",505,sy+10,12,ColorAlpha({200,170,40,255},a));
        DrawText("SCORE",SW-170,sy+10,12,ColorAlpha({200,170,40,255},a));
        for (int i=3;i<MAX_DISPLAY;i++) { if (i<(int)sortedPlayers.size()) DrawRow(sortedPlayers[i],i-3,sy+26+(i-3)*44,i==selectedIndex); }
        DrawFooter();
    }

    void Unload() {
        players.clear(); sortedPlayers.clear(); stars.clear();
        if (hasMusic) { StopMusicStream(bgMusic); UnloadMusicStream(bgMusic); }
        if (hasClick) UnloadSound(sfxClick);
        if (hasGlitchMasuk) UnloadSound(sfxGlitchMasuk);
        if (hasGlitchKeluar) UnloadSound(sfxGlitchKeluar);
    }

}