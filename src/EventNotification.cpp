#include "EventNotification.h"
#include <cmath>
#include <cstring>

EventNotification::EventNotification() {
    isActive   = false;
    timer      = 0.0f;
    slideY     = 720.0f;
    alpha      = 0.0f;
    pulseTimer = 0.0f;
    isSliding  = false;
    isSlideOut = false;
}

// =============================================
//  PRESET NOTIFIKASI
// =============================================

void EventNotification::TriggerMeteorShower() {
    Push(NotifType::METEOR_SHOWER,
         "INCOMING ASTEROID SHOWER — Brace yourself, Ace!",
         "CAPTAIN VOSS", 7.0f);
}

void EventNotification::TriggerBossWarning(const std::string& msg) {
    Push(NotifType::BOSS_WARNING, msg, "CAPTAIN VOSS", 8.0f);
}

void EventNotification::TriggerEnemyUpgrade(int level) {
    std::string msg = "Enemy density increased — Level ";
    msg += std::to_string(level);
    msg += ". Stay focused!";
    Push(NotifType::ENEMY_UPGRADE, msg, "CAPTAIN VOSS", 6.0f);
}

void EventNotification::TriggerFindNova() {
    Push(NotifType::FIND_NOVA,
         "Still no signal from Nova. Keep the ship alive — she's counting on you.",
         "CAPTAIN VOSS", 7.0f);
}

// =============================================
//  PUSH KE QUEUE
// =============================================

void EventNotification::Push(NotifType type, const std::string& message,
                              const std::string& speaker, float duration) {
    NotifEvent e;
    e.type        = type;
    e.message     = message;
    e.speakerName = speaker;
    e.duration    = duration;
    e.color       = GetTypeColor(type);
    notifQueue.push(e);

    // Kalau tidak ada yang aktif, langsung tampilkan
    if (!isActive) ShowNext();
}

void EventNotification::ShowNext() {
    if (notifQueue.empty()) {
        isActive = false;
        return;
    }
    current    = notifQueue.front();
    notifQueue.pop();

    isActive   = true;
    timer      = current.duration;
    slideY     = 740.0f;
    alpha      = 0.0f;
    isSliding  = true;
    isSlideOut = false;
    pulseTimer = 0.0f;
}

// =============================================
//  ICON & COLOR PER TIPE
// =============================================

const char* EventNotification::GetIcon(NotifType t) const {
    switch (t) {
        case NotifType::METEOR_SHOWER:  return "[!]";
        case NotifType::BOSS_WARNING:   return "[W]";
        case NotifType::FIND_NOVA:      return "[?]";
        case NotifType::ENEMY_UPGRADE:  return "[^]";
        case NotifType::SYSTEM_ALERT:   return "[i]";
        default:                        return "[!]";
    }
}

Color EventNotification::GetTypeColor(NotifType t) const {
    switch (t) {
        case NotifType::METEOR_SHOWER:  return { 255, 100, 60, 255 };   // Merah oranye
        case NotifType::BOSS_WARNING:   return { 255, 190, 50, 255 };   // Kuning peringatan
        case NotifType::FIND_NOVA:      return { 180, 100, 255, 255 };  // Ungu Nova
        case NotifType::ENEMY_UPGRADE:  return { 60, 200, 255, 255 };   // Cyan
        case NotifType::SYSTEM_ALERT:   return { 180, 180, 180, 255 };  // Abu
        default:                        return WHITE;
    }
}

// =============================================
//  UPDATE
// =============================================

void EventNotification::Update(float deltaTime) {
    if (!isActive) return;

    pulseTimer += deltaTime * 3.5f;

    // Slide in dari bawah
    if (isSliding) {
        float targetY = 660.0f;
        slideY = slideY + (targetY - slideY) * deltaTime * 8.0f;
        alpha  = fminf(alpha + deltaTime * 4.0f, 1.0f);
        if (fabsf(slideY - targetY) < 1.0f) {
            slideY    = targetY;
            isSliding = false;
        }
        return;
    }

    // Slide out kalau timer habis atau ENTER
    if (!isSlideOut) {
        timer -= deltaTime;

        // ENTER untuk close notif lebih cepat
        if (IsKeyPressed(KEY_ENTER) || timer <= 0.0f) {
            isSlideOut = true;
        }
    } else {
        slideY += deltaTime * 300.0f;  // slide ke bawah
        alpha   = fmaxf(alpha - deltaTime * 3.5f, 0.0f);
        if (alpha <= 0.0f) {
            isActive = false;
            ShowNext();  // cek queue berikutnya
        }
    }
}

// =============================================
//  DRAW
// =============================================

void EventNotification::DrawNotifBox() {
    if (!isActive || alpha <= 0.01f) return;

    unsigned char a = (unsigned char)(alpha * 255);

    // --- Ukuran & posisi box ---
    float boxW  = 700.0f;
    float boxH  = 60.0f;
    float boxX  = 1080.0f / 2.0f - boxW / 2.0f;
    float boxY  = slideY;

    // --- Background box ---
    DrawRectangle((int)boxX, (int)boxY, (int)boxW, (int)boxH,
        { 5, 10, 25, (unsigned char)(a * 0.88f) });

    // --- Garis atas warna sesuai tipe ---
    Color c = current.color;
    DrawRectangle((int)boxX, (int)boxY, (int)boxW, 3,
        { c.r, c.g, c.b, a });

    // --- Garis kiri aksen ---
    DrawRectangle((int)boxX, (int)boxY, 4, (int)boxH,
        { c.r, c.g, c.b, a });

    // --- Icon ---
    const char* icon = GetIcon(current.type);
    float pulse = 0.7f + 0.3f * sinf(pulseTimer);
    DrawText(icon, (int)boxX + 12, (int)boxY + 10, 22,
        { c.r, c.g, c.b, (unsigned char)(a * pulse) });

    // --- Speaker name ---
    const char* spk = current.speakerName.c_str();
    DrawText(spk, (int)boxX + 55, (int)boxY + 8, 13,
        { c.r, c.g, c.b, (unsigned char)(a * 0.9f) });

    // --- Message text ---
    DrawText(current.message.c_str(),
        (int)boxX + 55, (int)boxY + 28, 16,
        { 220, 235, 255, a });

    // --- Timer bar di bawah box ---
    float timerRatio = fmaxf(0.0f, timer / current.duration);
    DrawRectangle((int)boxX, (int)boxY + (int)boxH - 3,
        (int)(boxW * timerRatio), 3,
        { c.r, c.g, c.b, (unsigned char)(a * 0.6f) });

    // --- Hint [ENTER] ---
    const char* hint = "[ENTER] dismiss";
    int hw = MeasureText(hint, 12);
    DrawText(hint,
        (int)(boxX + boxW - hw - 8),
        (int)(boxY + boxH - 18),
        12, { 80, 100, 130, (unsigned char)(a * 0.7f) });
}

void EventNotification::Draw() {
    DrawNotifBox();
}
