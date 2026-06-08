#include "Combostack.h"

// Push: tambah combo dengan nilai ×2 dari level sebelumnya.
// Multiplier: 1× → 2× → 4× → 8× → 16× → 32× → 64× (maks 6 level)
void ComboStack::Push() {
    if (top >= MAX_COMBO_LEVEL) return;          // Max level tercapai, abaikan
    int prev = top > 0 ? stack[top - 1] : 1;     // Ambil nilai level sebelumnya (default 1 jika kosong)
    stack[top++] = prev * 2;                     // Simpan nilai ×2, lalu increment top
}

// Pop: kurangi combo — hapus level teratas
void ComboStack::Pop() {
    if (top > 0) {
        stack[top - 1] = 0;                      // Kosongkan slot (opsional, untuk keamanan)
        top--;                                   // Decrement top
    }
}

// Reset: kosongkan semua level, kembali ke multiplier 1×
void ComboStack::Reset() {
    for (int i = 0; i < top; i++) stack[i] = 0;  // Kosongkan semua slot yang terpakai
    top = 0;                                      // Reset top ke 0
}

// GetMultiplier: ambil nilai multiplier saat ini.
// Return 1× jika stack kosong (tidak ada combo aktif).
int ComboStack::GetMultiplier() const {
    return top == 0 ? 1 : stack[top - 1];         // Nilai teratas, default 1 jika kosong
}
