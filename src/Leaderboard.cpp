#include "Leaderboard.h"
#include "AudioManager.h"
#include "raylib.h"
#include "GameConfig.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

namespace LeaderboardSystem {

    static AVLTree playerTree;                       // Storage utama pakai AVL
    static std::vector<PlayerData> sortedPlayers;    // Hasil traversal
    static int selectedIndex = 0;                    // Index player yang terpilih di list
    static int scrollOffset = 0;                     // Scroll offset untuk baris tabel
    static const int MAX_VISIBLE_ROWS = 7;           // Jumlah baris yang terlihat di tabel
    static bool showingDetail = false;               // Mode detail player (pop-up)
    static int detailIndex = -1;                     // Index player yang ditampilkan detailnya
    static float fadeIn = 0.0f;                      // Animasi fade-in keseluruhan
    static float titlePulse = 0.0f;                  // Pulse effect pada title
    static float podiumRise[3] = {0,0,0};            // Animasi naik podium (rank 1-3)
    static float rowSlideIn[MAX_VISIBLE_ROWS] = {0}; // Animasi slide-in setiap baris tabel
    static float scanLinePos = 0.0f;                 // Posisi scan line horizontal (efek retro)
    static bool isEntering = true;                   // Flag animasi masuk
    static bool isExiting = false;                   // Flag animasi keluar
    static float enterTimer = 0.0f;                  // Timer animasi masuk
    static float exitTimer = 0.0f;                   // Timer animasi keluar
    static const float ENTER_DURATION = 0.6f;        // Durasi animasi masuk (detik)
    static const float EXIT_DURATION = 0.5f;         // Durasi animasi keluar (detik)
    static float glitchIntensity = 1.0f;             // Intensitas efek glitch (1=max, 0=hilang)
    struct Star { Vector2 pos; float speed, size, alpha; };  // Bintang latar belakang
    static std::vector<Star> stars;                  // Kumpulan bintang untuk background
    static float musicVolume = 0.0f;                 // Volume musik (fade-in/out)
    static const float MAX_VOLUME = 0.5f;            // Volume maksimum
    static bool glitchEnterPlayed = false, glitchExitPlayed = false;

    // ---------- AVL helpers ----------
    int AVLTree::height(AVLNode* n) const {
        return n ? n->height : 0;
    }
    int AVLTree::balanceFactor(AVLNode* n) const {
        return n ? height(n->left) - height(n->right) : 0;
    }
    void AVLTree::updateHeight(AVLNode* n) {
        if (n) n->height = 1 + std::max(height(n->left), height(n->right));
    }
    AVLNode* AVLTree::rotateRight(AVLNode* y) {
        AVLNode* x  = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left  = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }
    AVLNode* AVLTree::rotateLeft(AVLNode* x) {
        AVLNode* y  = x->right;
        AVLNode* T2 = y->left;
        y->left  = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }
    AVLNode* AVLTree::balance(AVLNode* n) {
        updateHeight(n);
        int bf = balanceFactor(n);
        // Left heavy
        if (bf > 1) {
            if (balanceFactor(n->left) < 0) n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        // Right heavy
        if (bf < -1) {
            if (balanceFactor(n->right) > 0) n->right = rotateRight(n->right);
            return rotateLeft(n);
        }
        return n;
    }
    // Insert descending: score lebih besar → kiri
    AVLNode* AVLTree::insert(AVLNode* n, const PlayerData& d) {
        if (!n) return new AVLNode(d);
        if (d.score >= n->data.score) n->left  = insert(n->left,  d);
        else                          n->right = insert(n->right, d);
        return balance(n);
    }
    void AVLTree::insert(const PlayerData& d) {
        root = insert(root, d);
    }
    // In-order traversal → hasil sudah descending by score
    void AVLTree::inorderDesc(AVLNode* n, std::vector<PlayerData>& out) const {
        if (!n) return;
        inorderDesc(n->left,  out);
        out.push_back(n->data);
        inorderDesc(n->right, out);
    }
    void AVLTree::toSortedVector(std::vector<PlayerData>& out) const {
        out.clear();
        inorderDesc(root, out);
    }
    void AVLTree::clear(AVLNode* n) {
        if (!n) return;
        clear(n->left);
        clear(n->right);
        delete n;
    }
    void AVLTree::clear() {
        clear(root);
        root = nullptr;
    }

    // Sortir player by score descending, lalu assign rank
    void SortPlayers() {
        playerTree.toSortedVector(sortedPlayers);  // Traversal AVL → sudah terurut
        for (size_t i = 0; i < sortedPlayers.size(); i++) sortedPlayers[i].rank = (int)i + 1;
    }

    // Inisialisasi bintang latar dengan posisi dan kecepatan acak
    void InitStars() {
        stars.clear();
        for (int i = 0; i < 90; i++) {
            Star s; s.pos = {(float)GetRandomValue(0,Config::screenWidth),(float)GetRandomValue(0,Config::screenHeight)};
            s.speed = (float)GetRandomValue(5,25)/10.0f; s.size = (float)GetRandomValue(1,3);
            s.alpha = (float)GetRandomValue(20,60)/100.0f; stars.push_back(s);
        }
    }

    // Reset semua state ke nilai awal (untuk Init/re-init)
    void FullReset() {
        selectedIndex = 0; scrollOffset = 0; showingDetail = false; detailIndex = -1; fadeIn = 0.0f; scanLinePos = 0.0f;
        for (int i = 0; i < 3; i++) podiumRise[i] = 0.0f;
        for (int i = 0; i < MAX_VISIBLE_ROWS; i++) rowSlideIn[i] = 0.0f;
        isEntering = true; isExiting = false; enterTimer = 0.0f; exitTimer = 0.0f;
        glitchIntensity = 1.0f; glitchEnterPlayed = false; glitchExitPlayed = false; musicVolume = 0.0f;
        InitStars();
    }

    // Update posisi bintang: gerak ke bawah, loop kembali ke atas
    void UpdateStars() { for (auto& s : stars) { s.pos.y += s.speed; if (s.pos.y > Config::screenHeight+5) { s.pos.y = -5; s.pos.x = (float)GetRandomValue(0,Config::screenWidth); } } }

    // Gambar bintang dengan alpha yang disesuaikan state animasi
    void DrawStars() {
        float da = fadeIn; if (isEntering) da = 1.0f - glitchIntensity; if (isExiting) da = 1.0f - glitchIntensity; if (da < 0) da = 0;
        for (auto& s : stars) { float a = s.alpha * da; if (a > 0.01f) DrawCircleV(s.pos, s.size, {255,255,255,(unsigned char)(a*200)}); }
    }

    // Load audio: play bgm via AudioManager, reset flags
    void LoadAudio() {
        AudioManager::getInstance().playMusic("bgmLeaderboard");
        AudioManager::getInstance().setMusicVolume("bgmLeaderboard", 0.0f);

        glitchEnterPlayed = false; glitchExitPlayed = false; isEntering = true; musicVolume = 0.0f;
    }

    // Load data leaderboard dari file JSON dengan filter merge conflicts
    void LoadFromJSON(const std::string& fp) {
        std::ifstream f(fp); if (!f.is_open()) return;
        std::string s, l;
        // Baca file, skip line yang mengandung merge conflict markers (<<<<<<<, =======, >>>>>>>)
        while (std::getline(f, l)) { if (l.find("<<<<<<<")==std::string::npos && l.find("=======")==std::string::npos && l.find(">>>>>>>")==std::string::npos) s += l + "\n"; }
        f.close();
        json j; try { j = json::parse(s); } catch (...) { return; } // Abaikan jika JSON tidak valid
        playerTree.clear();
        if (j.contains("data")) for (auto& it : j["data"]) {
            PlayerData p;
            p.name            = it.value("username",           "?");
            p.score           = it.value("highest_score",      0);
            p.survivalTime    = it.value("survival_time",      0.0f);
            p.researchPoint   = it.value("research_point",     0);
            playerTree.insert(p);
        }
        SortPlayers(); // Urutkan berdasarkan score
    }

    // Gambar background: dark overlay, grid dot, bintang, scan line, efek glitch
    void DrawBg() {
        DrawRectangle(0,0,Config::screenWidth,Config::screenHeight,{0,0,0,170});
        // Grid dot pattern untuk efek retro
        for (int x=0;x<Config::screenWidth;x+=40) for (int y=0;y<Config::screenHeight;y+=40) DrawRectangle(x,y,1,1,{0,180,200,18});
        DrawStars();
        // Scan line horizontal bergerak ke bawah
        DrawRectangle(0,(int)scanLinePos,Config::screenWidth,2,{0,255,255,5});
        // Efek glitch saat transisi masuk/keluar: garis horizontal acak
        if ((isEntering||isExiting) && glitchIntensity>0.01f) { for (int i=0;i<10;i++) { int y=GetRandomValue(0,Config::screenHeight),h=GetRandomValue(2,15); DrawRectangle(0,y,Config::screenWidth,h,(i%2==0)?(Color){0,255,200,(unsigned char)(glitchIntensity*180)}:(Color){255,50,120,(unsigned char)(glitchIntensity*140)}); } }
    }

    // Gambar header "HALL OF CHAMPIONS" dengan border glitch
    void DrawHeader() {
        float a = (isEntering||isExiting) ? 0 : fadeIn;
        DrawRectangle(Config::screenWidth/2-250,18,500,55,{0,0,0,170});
        DrawRectangleLinesEx({(float)(Config::screenWidth/2-250),18,500,55},1.5f,ColorAlpha({0,220,200,255},a));
        const char* t = "HALL OF CHAMPIONS"; int tw = MeasureText(t,36);
        DrawText(t,Config::screenWidth/2-tw/2,28,36,ColorAlpha({0,245,225,255},a));
    }

    // Gambar podium untuk 3 besar dengan mahkota untuk rank 1
    void DrawPodium(float cx, float cy, float w, float h, int rank, const PlayerData* p, bool selected) {
        float a = (isEntering||isExiting) ? 0 : fadeIn * podiumRise[rank-1];
        Color c; if (rank==1) c={255,195,35,255}; else if (rank==2) c={185,195,210,255}; else c={195,130,45,255};
        float ct = cy - h/2;
        DrawRectangle(cx-w/2,ct,w,h,ColorAlpha(selected?(Color){18,48,58,220}:(Color){6,18,28,255},a*0.95f));
        DrawRectangleLinesEx({cx-w/2,ct,w,h},selected?2.5f:1.5f,ColorAlpha(selected?(Color){0,245,225,255}:c,a));
        if (selected) { DrawRectangle(cx-w/2,ct,4,h,ColorAlpha({0,245,225,255},a)); DrawText(">",cx-w/2+8,ct+h/2-9,16,ColorAlpha({0,245,225,255},a)); }
        if (p) {
            // Mahkota sederhana untuk juara 1
            if (rank==1) { DrawRectangle(cx-15,ct-20,30,5,ColorAlpha(GOLD,a)); DrawRectangle(cx-15,ct-28,6,12,ColorAlpha(GOLD,a)); DrawRectangle(cx-3,ct-32,6,16,ColorAlpha(GOLD,a)); DrawRectangle(cx+9,ct-28,6,12,ColorAlpha(GOLD,a)); }
            int nw = MeasureText(p->name.c_str(),20); DrawText(p->name.c_str(),cx-nw/2,ct+15,20,ColorAlpha(WHITE,a));
            const char* sc = TextFormat("%d",p->score); int sw = MeasureText(sc,26); DrawText(sc,cx-sw/2,ct+40,26,ColorAlpha({0,235,215,255},a));
        } else { DrawText("---",cx-18,ct+h/2-8,18,ColorAlpha({60,100,100,255},a*0.4f)); }
    }

    // Gambar satu baris data player di tabel peringkat
    void DrawRow(const PlayerData& p, int idx, float y, bool sel) {
        float a = (isEntering||isExiting) ? 0 : rowSlideIn[idx] * fadeIn;
        DrawRectangle(70,y,Config::screenWidth-140,40,ColorAlpha(sel?(Color){18,48,58,200}:(Color){8,15,22,200},a));
        if (sel) { DrawRectangle(70,y,4,40,ColorAlpha({0,245,225,255},a)); DrawText(">",82,y+12,16,ColorAlpha({0,245,225,255},a)); }

        // Rank badge: gold/silver/bronze untuk 3 besar
        if (p.rank == 1) {
            DrawCircle(100, y+20, 11, ColorAlpha({255,195,35,255},a));
            DrawText("1",95,y+11,13,ColorAlpha({0,0,0,255},a));
        } else if (p.rank == 2) {
            DrawCircle(100, y+20, 11, ColorAlpha({185,195,210,255},a));
            DrawText("2",95,y+11,13,ColorAlpha({0,0,0,255},a));
        } else if (p.rank == 3) {
            DrawCircle(100, y+20, 11, ColorAlpha({195,130,45,255},a));
            DrawText("3",95,y+11,13,ColorAlpha({0,0,0,255},a));
        } else {
            DrawText(TextFormat("#%02d",p.rank),100,y+11,17,ColorAlpha({180,200,210,255},a));
        }

        DrawText(p.name.c_str(),165,y+11,17,ColorAlpha(WHITE,a));
        const char* sc = TextFormat("%d",p.score); DrawText(sc,Config::screenWidth-100-MeasureText(sc,17),y+11,17,ColorAlpha({0,235,215,255},a));
    }

    // Footer: kredit dan petunjuk navigasi
    void DrawFooter() {
        float a = (isEntering||isExiting) ? 0 : fadeIn;
        DrawText("The Typing Guardians",60,Config::screenHeight-30,13,ColorAlpha({120,200,200,255},a*0.55f));
        DrawText("[ W/S ] NAVIGATE   [ ENTER ] DETAIL   [ ESC ] EXIT",Config::screenWidth-470,Config::screenHeight-30,13,ColorAlpha({120,200,200,255},a*0.55f));
    }

    void DrawDetail(const PlayerData& p) {
    // Overlay gelap
    DrawRectangle(0, 0, Config::screenWidth, Config::screenHeight, {0, 0, 0, 160});

    // Panel tengah
    float px = Config::screenWidth/2 - 220, py = Config::screenHeight/2 - 180;
    DrawRectangle(px, py, 440, 360, {6, 18, 28, 245});
    DrawRectangleLinesEx({px, py, 440, 360}, 1.5f, {0, 220, 200, 255});

    // Judul panel
    const char* title = "PLAYER DETAIL";
    DrawText(title, Config::screenWidth/2 - MeasureText(title, 22)/2, py + 18, 22, {0, 245, 225, 255});
    DrawLine(px + 20, py + 50, px + 420, py + 50, {0, 180, 160, 100});

    // Rank & Name
    DrawText(TextFormat("Rank   : #%02d", p.rank),       px+30, py+70,  18, WHITE);
    DrawText(TextFormat("Name   : %s",   p.name.c_str()),px+30, py+100, 18, WHITE);

    // Score
    DrawText(TextFormat("Score  : %d",   p.score),       px+30, py+130, 18, {0, 235, 215, 255});

    // Research Point
    DrawText(TextFormat("RP     : %d",   p.researchPoint), px+30, py+160, 18, {200, 170, 40, 255});

    // Stats (kalau ada datanya)
    DrawText(TextFormat("Time   : %.1fs",  p.survivalTime),    px+30, py+190, 18, {160, 200, 200, 255});

    // Petunjuk keluar
    const char* hint = "[ ENTER / ESC ] CLOSE";
    DrawText(hint, Config::screenWidth/2 - MeasureText(hint, 13)/2, py + 320, 13, {120, 200, 200, 150});
    }

    void Init() {
        FullReset(); LoadAudio(); LoadFromJSON("data/PlayerData.json");
    }
    void AddPlayerData(const PlayerData& d) { playerTree.insert(d); SortPlayers(); }

    void ClearData() { playerTree.clear(); sortedPlayers.clear(); }
    int GetPlayerCount() { return (int)sortedPlayers.size(); }

    void Update(bool& back) {
        float dt = GetFrameTime();
        // Update dan fade-in/out musik via AudioManager
        AudioManager::getInstance().updateMusic("bgmLeaderboard");
        if (!isExiting && musicVolume<MAX_VOLUME) { musicVolume+=dt*0.5f; if (musicVolume>MAX_VOLUME) musicVolume=MAX_VOLUME; AudioManager::getInstance().setMusicVolume("bgmLeaderboard",musicVolume); }
        if (isExiting) { musicVolume-=dt*3.0f; if (musicVolume<0) musicVolume=0; AudioManager::getInstance().setMusicVolume("bgmLeaderboard",musicVolume); }
        scanLinePos+=dt*150; if (scanLinePos>Config::screenHeight) scanLinePos=-5;  // Loop scan line
        UpdateStars();
        // Animasi masuk: glitch semakin pudar, lalu play glitch sound
        if (isEntering) {
            enterTimer+=dt; glitchIntensity=1-(enterTimer/ENTER_DURATION); if (glitchIntensity<0) glitchIntensity=0;
            if (!glitchEnterPlayed && enterTimer>0.05f) { AudioManager::getInstance().playSfxOnce("glitchMasuk"); glitchEnterPlayed=true; }
            if (enterTimer>=ENTER_DURATION) { isEntering=false; glitchIntensity=0; }
        }
        // Animasi keluar: glitch meningkat, setelah selesai set back=true
        if (isExiting) {
            exitTimer+=dt; glitchIntensity=exitTimer/EXIT_DURATION; if (glitchIntensity>1) glitchIntensity=1;
            if (!glitchExitPlayed && exitTimer>0.05f) { AudioManager::getInstance().playSfxOnce("glitchKeluar"); glitchExitPlayed=true; }
            if (exitTimer>=EXIT_DURATION) { isExiting=false; back=true; } return;
        }
        // Fade-in konten setelah glitch selesai
        if (!isEntering && fadeIn<1) { fadeIn+=dt*1.8f; if (fadeIn>1) fadeIn=1; }
        // Animasi naik podium (rank 1 naik duluan, 2&3 menyusul)
        if (fadeIn>0.2f) { podiumRise[0]=std::min(podiumRise[0]+dt*2.0f,1.0f); podiumRise[1]=std::min(podiumRise[1]+dt*2.3f,1.0f); podiumRise[2]=std::min(podiumRise[2]+dt*2.3f,1.0f); }
        // Animasi slide-in baris data (hanya untuk baris yang terlihat)
        if (fadeIn>0.5f) { for (int i=0;i<MAX_VISIBLE_ROWS;i++) { int idx=3+scrollOffset+i; if (idx<(int)sortedPlayers.size()) rowSlideIn[i]=std::min(rowSlideIn[i]+dt*1.8f,1.0f); } }
        // Mode detail: blocking, tunggu ENTER/ESC untuk keluar
        if (showingDetail) { if (IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_ESCAPE)) { showingDetail=false; } return; }
        // Navigasi W/S atau UP/DOWN dalam list dengan scroll (non-detail mode)
        int total=(int)sortedPlayers.size();
        if (total==0) return;
        if ((IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP))&&selectedIndex>0) {
            selectedIndex--;
            if (selectedIndex>=3 && selectedIndex<3+scrollOffset) scrollOffset--;
        }
        if ((IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN))&&selectedIndex<total-1) {
            selectedIndex++;
            if (selectedIndex>=3+scrollOffset+MAX_VISIBLE_ROWS) scrollOffset++;
        }
        // Clamp scrollOffset ke rentang valid
        int maxScrollOffset = std::max(0, total - 3 - MAX_VISIBLE_ROWS);
        scrollOffset = std::max(0, std::min(scrollOffset, maxScrollOffset));
        if (IsKeyPressed(KEY_ENTER)&&selectedIndex<total) { showingDetail=true; detailIndex=selectedIndex; }
        // ESC/BACKSPACE memulai animasi keluar
        if (IsKeyPressed(KEY_ESCAPE)||IsKeyPressed(KEY_BACKSPACE)) { isExiting=true; isEntering=false; exitTimer=0; glitchExitPlayed=false; }
    }

    void Draw() {
        DrawBg(); DrawHeader();
        float py=200;
        // Posisi podium: rank 2 di kiri, rank 1 di tengah (lebih lebar & tinggi), rank 3 di kanan
        struct { float cx,w,h; int r,i; } pd[3] = {{290,215,145,2,1},{540,245,165,1,0},{790,215,145,3,2}};
        PlayerData* t3[3] = {0}; for (int i=0;i<3&&i<(int)sortedPlayers.size();i++) t3[i]=&sortedPlayers[i];
        for (int i=0;i<3;i++) DrawPodium(pd[i].cx,py,pd[i].w,pd[i].h,pd[i].r,t3[pd[i].i],selectedIndex==pd[i].i);
        float sy=370, a=(isEntering||isExiting)?0:fadeIn;
        // Header kolom tabel
        DrawText("RANK",95,sy+10,12,ColorAlpha({200,170,40,255},a));
        DrawText("PLAYER",165,sy+10,12,ColorAlpha({200,170,40,255},a));
        DrawText("SCORE",Config::screenWidth-170,sy+10,12,ColorAlpha({200,170,40,255},a));
        // Baris data scrolling: mulai dari 3+scrollOffset, tampilkan MAX_VISIBLE_ROWS baris
        for (int i=0;i<MAX_VISIBLE_ROWS;i++) {
            int idx=3+scrollOffset+i;
            if (idx<(int)sortedPlayers.size()) DrawRow(sortedPlayers[idx],i,sy+26+i*44,idx==selectedIndex);
        }
        DrawFooter();

        // Tampilkan detail jika showingDetail aktif
        if (showingDetail && detailIndex >= 0 && detailIndex < (int)sortedPlayers.size()) {
            DrawDetail(sortedPlayers[detailIndex]);
        }
    }

    // Cleanup state (AssetManager handle lifecycle raylib resources)
    void Unload() {
        playerTree.clear(); sortedPlayers.clear(); stars.clear();
    }

}
