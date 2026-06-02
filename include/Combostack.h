#pragma once

class ComboStack {
private:
    static constexpr int MAX_COMBO_LEVEL = 6;   // level maksimal combo (6)
    int stack[MAX_COMBO_LEVEL]{};               // array stack untuk menyimpan nilai multiplier
    int top = 0;                                // index top dari stack (0 = kosong)

public:
    ComboStack() = default;
    void Push();                                // menambah combo (×2) setelah 5 kata benar
    void Pop();                                 // mengurangi combo saat salah ketik
    void Reset();                               // reset combo ke 1×
    int GetMultiplier() const;                  // mendapatkan multiplier saat ini
};
