#pragma once

// Fixed-size stack untuk combo multiplier — maksimal 6 level
// Setiap Push menggandakan multiplier (×2, ×4, ×8, ..., ×64)
class ComboStack {
private:
    static constexpr int MAX_COMBO_LEVEL = 6;   // Level maksimal combo (6 = ×64)
    int stack[MAX_COMBO_LEVEL]{};                // Array untuk menyimpan nilai multiplier tiap level
    int top = 0;                                 // Index top (0 = stack kosong)

public:
    ComboStack() = default;
    void Push();                                 // Tambah combo: ×2 dari level sebelumnya
    void Pop();                                  // Kurangi combo: hapus level teratas
    int GetMultiplier() const;                   // Dapatkan multiplier saat ini (1× jika kosong)
};
