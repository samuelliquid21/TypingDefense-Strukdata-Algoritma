#include "ComboStack.h"
#include <cstdio>

// Multiplier: 1, 2, 4, 8, 16, 32, 64 (sesuai level combo)
const int ComboStack::MULTIPLIERS[] = {1, 2, 4, 8, 16, 32, 64};
const int ComboStack::MAX_COMBO_LEVEL = 6;  // Max level combo (index 6)

ComboStack::ComboStack() {
    topNode = nullptr;   // Stack kosong
    currentCombo = 0;    // Level combo 0 (1x multiplier)
}

// Cleanup semua nodes dari singly linked list
ComboStack::~ComboStack() {
    Reset();  
}

// Push: tambah combo 
void ComboStack::Push() {
    if (currentCombo >= MAX_COMBO_LEVEL) {
        return;  
    }
    
    Node* newNode = new Node();
    newNode->multiplier = MULTIPLIERS[currentCombo + 1];  
    newNode->next = topNode;  
    topNode = newNode;        
    
    currentCombo++;  
}

// Pop: kurangin combo (pas ngetik salah)
void ComboStack::Pop() {
    if (topNode == nullptr) {
        currentCombo = 0;  
        return;
    }
    
    Node* temp = topNode;
    topNode = topNode->next;
    delete temp;
    
    currentCombo--;
    
    if (currentCombo < 0) {
        currentCombo = 0;
    }
}

// Hapus semua node
void ComboStack::Reset() {
    while (topNode != nullptr) {
        Node* temp = topNode;
        topNode = topNode->next;
        delete temp;
    }
    currentCombo = 0;
}

// Mengambil nilai paling atas
int ComboStack::GetMultiplier() const {
    if (topNode == nullptr) {
        return MULTIPLIERS[0];  
    }
    return topNode->multiplier;
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