#pragma once

// Node untuk singly linked list stack
struct Node {
    int multiplier;  // Multiplier value (1, 2, 4, 8, 16, 32, 64)
    Node* next;      // Pointer ke node berikutnya (LIFO)
};

// Class untuk Combo Stack menggunakan singly linked list
// Push: tambah combo (pas ngetik benar)
// Pop:  kurangin combo (pas ngetik salah)
// Stack semantics: LIFO (Last In First Out)
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
    
    // Top: lihat multiplier sekarang
    int Top() const;
    
    // Reset: balikin ke 1x
    void Reset();
    
    // Get multiplier value
    int GetMultiplier() const;
    
    // Get combo count (0-6)
    int GetComboCount() const;
    
    // Check if max combo
    bool IsMaxCombo() const;
    
    // Check if empty
    bool IsEmpty() const;
};