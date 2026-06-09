#pragma once
#include "raylib.h"
#include <string>
#include <vector>

// Namespace untuk state dan rendering fitur leaderboard
// Berisi data player, logika sorting, animasi masuk/keluar, serta input navigation

namespace LeaderboardSystem {

    // Data satu entri leaderboard (disimpan dari JSON dan ditampilkan)
    struct PlayerData {
        std::string name;       
        int score;
        float accuracy;
        int wordsTyped;
        int enemiesDefeated;
        float survivalTime;
        int rank;               // Peringkat (diisi otomatis setelah sorting)
        int researchPoint;  // dari "research_point" di JSON
    };

    void Init();                // Inisialisasi: reset state, load audio & data dari JSON
    void Update(bool& backToMenu); // Update frame: input, animasi, musik
    void Draw();                // Render leaderboard dengan animasi entry/exit
    void Unload();              // Cleanup resources (audio)

        // Node AVL Tree
    struct AVLNode {
        PlayerData  data;
        AVLNode*    left;
        AVLNode*    right;
        int         height;
        AVLNode(const PlayerData& d)
            : data(d), left(nullptr), right(nullptr), height(1) {}
        };

// AVL Tree sorted descending by score
    class AVLTree {
    public:
        AVLTree()  : root(nullptr) {}
        ~AVLTree() { clear(root); root = nullptr; }

        void insert(const PlayerData& d);
        void clear();
        void toSortedVector(std::vector<PlayerData>& out) const;
        bool empty() const { return root == nullptr; }

    private:
        AVLNode* root;
        int      height(AVLNode* n) const;
        int      balanceFactor(AVLNode* n) const;
        void     updateHeight(AVLNode* n);
        AVLNode* rotateRight(AVLNode* y);
        AVLNode* rotateLeft(AVLNode* x);
        AVLNode* balance(AVLNode* n);
        AVLNode* insert(AVLNode* n, const PlayerData& d);
        void     inorderDesc(AVLNode* n, std::vector<PlayerData>& out) const;
        void     clear(AVLNode* n);
    };
} 
