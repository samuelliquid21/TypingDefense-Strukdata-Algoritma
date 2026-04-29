#pragma once

// Node untuk singly linked list stack
struct Node {
    int multiplier;  
    Node* next;      
};

// Class untuk Combo Stack menggunakan singly linked list
class ComboStack {
private:
    Node* topNode;       // Pointer ke top stack
    int currentCombo;    // Level combo saat ini (0-6)
    
    static const int MULTIPLIERS[];      // Array static untuk mapping level ke multiplier
    static const int MAX_COMBO_LEVEL;    // Max combo level (6)
    
public:
    ComboStack();
    ~ComboStack();
    
    // Push: tambah combo (pas ngetik benar)
    void Push();
    
    // Pop: kurangin combo (pas ngetik salah)
    void Pop();
    
    // Reset: balikin ke 1x
    void Reset();
    
    // mendapatkan nilai teratas
    int GetMultiplier() const;
    
    // mendapatkan banyak kombo (0-6)
    int GetComboCount() const;
    
    // cek jika combo max
    bool IsMaxCombo() const;
    
    // cek jika stack kosong
    bool IsEmpty() const;
};