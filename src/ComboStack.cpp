#include "ComboStack.h"
#include <cstdio>

// Static constants untuk ComboStack
// Multiplier: 1, 2, 4, 8, 16, 32, 64 (sesuai level combo)
const int ComboStack::MULTIPLIERS[] = {1, 2, 4, 8, 16, 32, 64};
const int ComboStack::MAX_COMBO_LEVEL = 6;  // Max level combo (index 6)

ComboStack::ComboStack() {
    topNode = nullptr;   // Stack kosong
    currentCombo = 0;    // Level combo 0 (1x multiplier)
}

ComboStack::~ComboStack() {
    Reset();  // Cleanup semua nodes dari singly linked list
}

// Push: tambah combo (pas ngetik benar)
// LIFO (Last In First Out) semantics untuk stack
void ComboStack::Push() {
    if (currentCombo >= MAX_COMBO_LEVEL) {
        return;  // Sudah combo max (64x)
    }
    
    // Bikin node baru untuk singly linked list stack
    Node* newNode = new Node();
    newNode->multiplier = MULTIPLIERS[currentCombo + 1];  // Next multiplier
    newNode->next = topNode;  // Next pointer ke current top
    topNode = newNode;        // Update top pointer ke node baru
    
    currentCombo++;  // Increment combo level
}

// Pop: kurangin combo (pas ngetik salah)
void ComboStack::Pop() {
    if (topNode == nullptr) {
        currentCombo = 0;  // Stack kosong -> combo 0 (1x)
        return;
    }
    
    // Hapus node paling atas (LIFO)
    Node* temp = topNode;
    topNode = topNode->next;
    delete temp;
    
    currentCombo--;
    
    if (currentCombo < 0) {
        currentCombo = 0;
    }
}

int ComboStack::Top() const {
    if (topNode == nullptr) {
        return MULTIPLIERS[0];  // Return 1x
    }
    return topNode->multiplier;
}

void ComboStack::Reset() {
    // Hapus semua node
    while (topNode != nullptr) {
        Node* temp = topNode;
        topNode = topNode->next;
        delete temp;
    }
    currentCombo = 0;
}

int ComboStack::GetMultiplier() const {
    return Top();
}

int ComboStack::GetComboCount() const {
    return currentCombo;
}

bool ComboStack::IsMaxCombo() const {
    return currentCombo >= MAX_COMBO_LEVEL;
}

bool ComboStack::IsEmpty() const {
    return topNode == nullptr;
}