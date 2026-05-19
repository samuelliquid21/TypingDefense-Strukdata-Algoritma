struct Timer
{
    float lifeTime = 0.0f;
    float duration = 1.0f;
    bool active = false;

    // ==== FUNGSI YANG DI PAKAI DI MODUL LAIN ====

    void start(float dur);          // memulai waktu
    void update(float deltaTime);   // update waktu (taruh di game logic bagian update)
    bool isDone() const;            // cek apakah timer sudah selesai
    float getProgress() const;      // cek progress timer (berguna untuk animasi)
};
