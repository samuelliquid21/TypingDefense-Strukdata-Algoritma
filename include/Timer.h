// Struktur Timer sederhana untuk menghitung mundur berbasis deltaTime
struct Timer
{
    float lifeTime = 0.0f; // Waktu yang sudah berjalan (dalam detik)
    float duration = 1.0f; // Durasi total timer (dalam detik)
    bool active = false;   // Apakah timer sedang berjalan?

    // ==== METODE UNTUK DIPAKAI DI MODUL LAIN ====

    // Mulai timer dengan durasi tertentu (reset lifetime, set active = true)
    void start(float dur);
    // Update timer setiap frame: tambah lifetime, nonaktifkan jika sudah mencapai duration
    void update(float deltaTime);
    // Cek apakah timer sudah selesai (lifeTime >= duration, tidak aktif)
    bool isDone() const;
    // Dapatkan progress 0.0 - 1.0 (berguna untuk animasi / progress bar)
    float getProgress() const;
};
