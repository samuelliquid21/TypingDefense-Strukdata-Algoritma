#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include <queue>

// =============================================
// EventNotification - Sistem notifikasi in-game
// Tampil di BAWAH layar (y ~660-710)
// Tidak mengganggu gameplay (semi-transparan)
// Bisa ENTER untuk close lebih cepat
// Ada timer otomatis kalau tidak di-close
// =============================================

enum class NotifType {
    METEOR_SHOWER,   // Hujan meteor datang
    BOSS_WARNING,    // Peringatan dari Captain Voss
    FIND_NOVA,       // Reminder misi mencari Nova
    ENEMY_UPGRADE,   // Musuh makin kuat
    SYSTEM_ALERT     // Alert sistem lainnya
};

struct NotifEvent {
    NotifType type;
    std::string message;
    std::string speakerName;   // siapa yang ngomong (Captain Voss / SYSTEM)
    float duration;            // berapa lama tampil (detik)
    Color color;               // warna teks
};

class EventNotification {
public:
    EventNotification();

    // Tambah notifikasi ke queue
    void Push(NotifType type, const std::string& message,
              const std::string& speaker = "SYSTEM",
              float duration = 6.0f);

    // Preset notifikasi yang udah jadi
    void TriggerMeteorShower();
    void TriggerBossWarning(const std::string& msg);
    void TriggerEnemyUpgrade(int level);
    void TriggerFindNova();

    void Update(float deltaTime);  // update timer & animasi
    void Draw();                   // render di bagian bawah layar

    bool HasActive() const { return isActive; }

private:
    std::queue<NotifEvent> notifQueue;
    NotifEvent current;
    bool isActive;

    // Timer & animasi
    float timer;          // waktu tersisa
    float slideY;         // posisi Y (slide dari bawah)
    float alpha;          // opacity
    float pulseTimer;     // untuk efek pulse/blink pada ikon
    bool  isSliding;      // sedang slide masuk?
    bool  isSlideOut;     // sedang slide keluar?

    // Ikon per tipe
    const char* GetIcon(NotifType t) const;
    Color GetTypeColor(NotifType t) const;

    void ShowNext();
    void DrawNotifBox();
};
