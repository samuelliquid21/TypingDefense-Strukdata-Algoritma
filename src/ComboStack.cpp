#include "Combostack.h"

// tambah combo (nilai ×2 dari sebelumnya)
void ComboStack::Push() {
    if (top >= MAX_COMBO_LEVEL) return;         // sudah max, abaikan
    int prev = top > 0 ? stack[top - 1] : 1;    // ambil nilai sebelumnya (default 1)
    stack[top++] = prev * 2;                    // simpan nilai ×2 lalu increment top
}

// kurangi combo (hapus nilai teratas)
void ComboStack::Pop() {
    if (top > 0) {
        stack[top - 1] = 0;                     // kosongkan slot
        top--;                                  // decrement top
    }
}

// balikin semua ke 1×
void ComboStack::Reset() {
    for (int i = 0; i < top; i++) stack[i] = 0; // kosongkan semua slot
    top = 0;                                    // reset top ke 0
}

// ambil nilai multiplier saat ini (1× jika kosong)
int ComboStack::GetMultiplier() const {
    return top == 0 ? 1 : stack[top - 1];       // return nilai teratas, default 1
}
