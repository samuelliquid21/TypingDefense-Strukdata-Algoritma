#include "Asteroid.h"
#include "WordGenerator.h"
#include "GameConfig.h"
#include "raymath.h"

// ===============================
// ☄️ ASTEROID
// ===============================
int Asteroid::counter = 0;

// == FUNGSI PEMBANTU ANONYM ==
// Hanya bisa digunakan di file ini, tidak bisa diakses modul lain
namespace {

    // Hitung kecepatan Y agar asteroid bergerak menuju posisi player
    float velocityYToPlayer(const Asteroid& asteroid) {
        Vector2 direction = Vector2Subtract(Config::playerStartPos, asteroid.position);
        direction = Vector2Normalize(direction);
        return direction.y * fabsf(asteroid.velocity.x);
    }

    // Hitung kecepatan Y random: ke bawah jika di atas layar, ke atas jika di bawah
    float randomVelocityY(const Asteroid& asteroid) {
        if (asteroid.position.y < Config::screenHeight / 2) {
            return GetRandomValue(5, 50);   // Bergerak ke bawah
        } else {
            return GetRandomValue(-50, -5); // Bergerak ke atas
        }
    }

}

Asteroid::Asteroid() { counter++; }  // Tambah counter saat objek dibuat
Asteroid::~Asteroid() { counter--; } // Kurang counter saat objek dihancurkan

int Asteroid::typingAsteroid(char characterTyped) {
    // Jika asteroid tidak aktif atau kata habis, tidak ada yang diproses
    if (!active || word.empty()) return 0;
    // Karakter yang diketik harus cocok dengan huruf pertama kata yang tersisa
    if (characterTyped != word[0]) return 0;

    // Tandai sebagai sedang ditarget, hapus huruf pertama dari kata
    targeted = true;
    word.erase(0, 1);

    // Jika kata sudah habis, nonaktifkan asteroid (hancur)
    if (word.empty()) {
        active = false;
        if (onDestroyed) onDestroyed(position);
    }

    return 1; // Berhasil: 1 karakter diproses
}

void Asteroid::asteroidType(const int tier) {
    // Tentukan kata dan kecepatan berdasarkan tier (1-6)
    // Tier ganjil: speed lambat (50), genap: speed cepat (80)
    switch (tier) {
    case 1:
        word = WordSystem::getRandomWord(Difficulty::EASY);
        velocity.x = 50;
        break;
    case 2:
        word = WordSystem::getRandomWord(Difficulty::EASY);
        velocity.x = 80;
        break;
    case 3:
        word = WordSystem::getRandomWord(Difficulty::MEDIUM);
        velocity.x = 50;
        break;
    case 4:
        word = WordSystem::getRandomWord(Difficulty::MEDIUM);
        velocity.x = 80;
        break;
    case 5:
        word = WordSystem::getRandomWord(Difficulty::HARD);
        velocity.x = 50;
        break;
    case 6:
        word = WordSystem::getRandomWord(Difficulty::HARD);
        velocity.x = 80;
        break;
    default:
        break;
    }

    originalWord = word; // Simpan kata asli sebelum diketik

    // Muncul dari tepi kiri layar, posisi Y random
    position.x = -radius;
    position.y = GetRandomValue(0, Config::screenHeight);
    // Radius proporsional dengan panjang kata
    radius = 20 + (word.length() * 2);
    textureId = GetRandomValue(0, 3); // Variasi visual random
    active = true;
    targeted = false;

    // 50% chance: asteroid bergerak menuju player, 50% random
    int toPlayer = GetRandomValue(0, 1);
    if (toPlayer) {
        velocity.y = velocityYToPlayer(*this);
    } else {
        velocity.y = randomVelocityY(*this);
    }
}

void Asteroid::update(float deltaTime) {
    // Update posisi berdasarkan velocity hanya jika aktif
    if (this->active) {
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;

        // Nonaktifkan jika keluar dari batas layar (semua 4 sisi)
        if (position.x - radius > Config::screenWidth) {
            this->active = false;
        }
        if (position.x + radius < 0) {
            this->active = false;
        }
        if (position.y - radius > Config::screenHeight) {
            this->active = false;
        }
        if (position.y + radius < 0) {
            this->active = false;
        }
    }
}

void Asteroid::draw() {
    // Gambar lingkaran asteroid abu-abu
    DrawCircleV(position, radius, GRAY);

    // Gambar sisa kata di tengah asteroid
    int textWidth = MeasureText(word.c_str(), 20);
    int textHeight = 20;
    DrawText(word.c_str(), position.x - textWidth/2, position.y - textHeight/2, 20, WHITE);

    // Jika sedang ditarget, gambar highlight tambahan
    if (targeted) {
        drawTargeted();
    }
}

void Asteroid::drawTargeted() {
    // Gambar kotak semi-transparan merah di sekeliling asteroid yang ditarget
    float size = radius * 2.2f;

    DrawRectangle(
        position.x - size/2,
        position.y - size/2,
        size,
        size,
        Fade(RED, 0.5f)
    );

    // Label "TARGETED" di atas asteroid
    const char* targetText = "TARGETED";
    int targetWidth = MeasureText(targetText, 20);
    DrawText(
        targetText,
        position.x - targetWidth/2,
        position.y - radius - 30,
        20,
        RED
    );
}
