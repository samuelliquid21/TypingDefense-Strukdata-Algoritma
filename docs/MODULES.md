# Referensi Modul

Semua header di `include/` kecuali `AsteroidManager2.h` (masih WIP).  
Hanya API publik yang dicantumkan — member private tidak disertakan.

---

## State

**Berkas:** `include/State.h`

```cpp
enum class GameState {
    MENU, GAMEPLAY, PAUSE, GAME_OVER, LEADERBOARD, CREDIT
};
```

Enum state untuk state machine game. Digunakan oleh `Game` untuk mengganti layar.

---

## GameConfig

**Berkas:** `include/GameConfig.h` — namespace `Config`

| Konstanta | Tipe | Nilai | Kegunaan |
|---|---|---|---|
| `screenWidth` | `int` | `1080` | Lebar window |
| `screenHeight` | `int` | `720` | Tinggi window |
| `maxAsteroids` | `int` | `100` | Kapasitas maksimal asteroid |
| `playerStartPos` | `Vector2` | `{1000, 360}` | Posisi awal spaceship |
| `playerHitbox` | `int` | `30` | Radius collision spaceship |
| `asteroidShowerInterval` | `float` | `0.1f` | Interval spawn saat shower |
| `showerEventInterval` | `float` | `30` | Detik antar event shower |
| `asteroidEventInterval` | `float` | `30` | Detik antar spawn normal |

---

## WordGenerator

**Berkas:** `include/WordGenerator.h` — namespace `WordSystem`

```cpp
enum class Difficulty { EASY, MEDIUM, HARD };
const std::string& getRandomWord(Difficulty diff);
```

Mengembalikan kata acak dari daftar bawaan berdasarkan tingkat kesulitan.

---

## Timer

**Berkas:** `include/Timer.h`

```cpp
struct Timer {
    float lifeTime;   // waktu yang sudah berjalan
    float duration;   // durasi target
    bool active;      // apakah timer sedang berjalan

    void start(float dur);
    void update(float deltaTime);
    bool isDone() const;
    float getProgress() const;  // 0.0 – 1.0
};
```

Timer hitung mundur satu kali. Panggil `start()`, lalu `update()` setiap frame, cek `isDone()`.

---

## DifficultyManager

**Berkas:** `include/DifficultyManager.h`

```cpp
struct DifficultyManager {
    float counter;  // waktu bermain yang sudah berlalu

    void updateTime();
    void resetTime();
    int  getAsteroidTier() const;                // 1–6, semakin sulit seiring waktu
    int  getAsteroidCountForSpecialSpawn() const; // 5–15
};
```

Kesulitan berbasis waktu. Tier naik seiring bertambahnya `counter`. Digunakan oleh `AsteroidManager`.

---

## Background

**Berkas:** `include/Background.h`

```cpp
class Background {
    Background();
    void Load(const char* fileName, float speed);
    void Unload();
    void Update();
    void Draw();
};
```

Background scrolling. Panggil `Update()` setiap frame untuk menggulir, `Draw()` untuk merender.

---

## SpaceShip

**Berkas:** `include/SpaceShip.h`

```cpp
class SpaceShip {
    SpaceShip();
    ~SpaceShip();
    void init();
    bool asteroiHitSpaceship(Vector2& asteroidPosition, int asteroidRadius);
    void activateLaser(Vector2& target);
    void update(float deltaTime);
    void draw();
};
```

Spaceship pemain. Memiliki deteksi tabrakan dan animasi laser yang menyala ke arah target.

---

## Asteroid

**Berkas:** `include/Asteroid.h`

```cpp
struct Asteroid {
    Vector2 position;
    Vector2 velocity;
    std::string word;
    int radius;
    int textureId;
    bool active;
    bool targeted;
    static int counter;

    Asteroid();
    ~Asteroid();
    void asteroidType(const int tier = 1);
    int typingAsteroid(char characterTyped);
    void update(float deltaTime);
    void draw();
    void drawTargeted();
};
```

`typingAsteroid()` mengecek karakter terhadap `word[0]`, menghapusnya jika cocok, mengembalikan `1` jika kena atau `0` jika salah. Menonaktifkan diri sendiri saat `word` habis.

---

## AsteroidManager

**Berkas:** `include/AsteroidManager.h`

**Struktur data yang digunakan:**
- Circular array buffer (size 20) — pool asteroid
- Singly linked list — asteroid shower
- Circular buffer priority queue (size 10) — penjadwalan event

```cpp
class AsteroidManager {
    AsteroidManager();
    ~AsteroidManager();
    void clear();
    Asteroid* getTarget(char charTyped);
    bool collisionWithPlayer();
    void update(float deltaTime);
    void draw();
};
```

| Fungsi | Perilaku |
|---|---|
| `getTarget(char)` | Memindai pool + shower untuk asteroid aktif dengan `word[0] == char`. Mengembalikan yang paling kanan (terdekat dengan player). |
| `collisionWithPlayer()` | Pengecekan circle-circle terhadap `Config::playerStartPos` / `Config::playerHitbox`. |
| `update(dt)` | Menggerakkan asteroid, memasukkan event NORMAL setiap 2dtk, SHOWER setiap 30dtk, mengeksekusi satu event per detik, menambah node shower setiap 60dtk. |

---

## ComboStack

**Berkas:** `include/Combostack.h`

```cpp
using ComboChangedCallback = void(*)(int newComboLevel, int newMultiplier);

class ComboStack {
    ComboStack();
    ~ComboStack();
    void Push();
    void Pop();
    void Reset();
    int GetMultiplier() const;       // 1,2,4,8,16,32,64
    int GetComboCount() const;       // 0–6
    bool IsMaxCombo() const;
    bool IsEmpty() const;
    void SetComboCallback(ComboChangedCallback cb);
};
```

Stack menggunakan singly linked list. `Push()` dipanggil setiap 5 kata selesai, `Pop()` saat salah ketik.

---

## GameplayManager

**Berkas:** `include/GameplayManager.h`

```cpp
using ScoreCallback = void(*)(int score, int multiplier);
using AsteroidDestroyedCallback = void(*)(const char* word);

class GameplayManager {
    int score;

    GameplayManager() = default;
    ~GameplayManager();

    void AddScore(int points);
    void AddScore(int basePoints, int multiplier);
    void SetScoreCallback(ScoreCallback cb);
    void SetAsteroidDestroyedCallback(AsteroidDestroyedCallback cb);
    void textureInit();
    bool isHit();
    void update(float deltaTime);
    void draw();
    void reset();
};
```

Koordinator gameplay utama. Memiliki `SpaceShip`, `AsteroidManager`, `ComboStack`.  
`update(dt)` membaca `GetCharPressed()` dan memasukkan karakter ke sistem pengetikan asteroid.

---

## MainMenu

**Berkas:** `include/MainMenu.h`

**Struktur data:** Circular doubly linked list (`MenuNode` dengan `prev`/`next`).

```cpp
class MainMenu {
    MainMenu();
    ~MainMenu();
    void Update();
    void Draw();
    int GetSelectedIndex() const;
    bool IsOptionChosen() const;
    void ResetChoice();
};
```

Opsi menu dibuat di constructor. Navigasi melingkar (circular).

---

## Pause

**Berkas:** `include/Pause.h`

```cpp
class PauseMenu {
    PauseMenu();
    void Update();
    void Draw();
    int GetSelectedIndex() const;
    bool IsOptionChosen() const;
    bool IsCountingDown() const;
    bool IsCountdownFinished() const;
    void StartCountdown();
    void Reset();
};
```

Layar pause dengan countdown 4 detik sebelum kembali ke permainan.

---

## GameOver

**Berkas:** `include/GameOver.h`

```cpp
class GameOver {
    GameOver();
    void Reset();
    void SetScore(int s);
    void SetHighestScore(int hs);
    void Update();
    void Draw();
    bool IsSaved() const;
    bool ShouldReturnToMenu() const;
};
```

Layar game over. Menyimpan skor ke `data/data.json` melalui `ScoreManager`.  
`ShouldReturnToMenu()` mengembalikan `true` saat pemain menekan ENTER.

---

## Credit

**Berkas:** `include/Credit.h`

```cpp
struct CreditEntry {
    std::string role;
    std::string name;
    bool isHeader;
};

class Credit {
    Credit();
    void Update(bool& backToMenu);
    void Draw();
};
```

Credit roll yang menggulir otomatis. Mengisi `backToMenu = true` saat ESC ditekan.

---

## Leaderboard

**Berkas:** `include/Leaderboard.h` — namespace `LeaderboardSystem`

```cpp
struct PlayerData {
    std::string name;
    int score;
    float accuracy;
    int wordsTyped;
    int enemiesDefeated;
    float survivalTime;
    int rank;
};

void Init();
void Update(bool& backToMenu);
void Draw();
void Unload();
```

Papan peringkat layar penuh dengan animasi podium, baris bergulir, transisi glitch.  
Data dimuat dari `data/data.json`. Navigasi: W/S, ENTER untuk detail, ESC untuk keluar.

---

## ScoreManager

**Berkas:** `include/ScoreManager.h` — namespace `ScoreManager`

Semua fungsi `inline` (didefinisikan di header).

```cpp
json Load(const std::string& path);
void Save(const std::string& path, const json& j);
void InsertOrUpdate(json& j, const std::string& username, int score);
int GetHighest(const json& j);
```

Baca/tulis file JSON dengan `nlohmann/json`.  
Format JSON: `{"data": [{"username": "...", "score": N}, ...]}`

---

## SinglyLinkedList

**Berkas:** `include/SinglyLinkedList.h` — **header-only template**

```cpp
template <typename T>
class SinglyLinkedList {
    SinglyLinkedList();
    ~SinglyLinkedList();

    void push_front(const T& value);
    void push_back(const T& value);
    void pop_front();
    void pop_back();
    bool remove(const T& value);
    void next();
    T* getCurrent();
    void clear();
    void display() const;
    int getSize() const;
    bool isEmpty() const;
};
```

Generic singly linked list dengan kursor traversal internal (`current`).  
Mendukung semua tipe `T` yang bisa di-copy.

---

## Game

**Berkas:** `include/Game.h`

```cpp
class Game {
    Game();
    ~Game();
    void Run();
};
```

Pengontrol game tingkat atas. Membuat window (1080×720, 60 FPS, tutup dengan F12), audio, tekstur.  
Menjalankan state machine: `MENU → GAMEPLAY ⇄ PAUSE → GAME_OVER → LEADERBOARD / CREDIT → MENU`.
