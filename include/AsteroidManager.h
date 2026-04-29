#pragma once
#include "Asteroid.h"
#include "DifficultyManager.h"

// Event types untuk priority queue
enum EventType {
    NORMAL = 1,          // Spawn asteroid normal
    ASTEROID_SHOWER = 2  // Spawn banyak asteroid (shower)
};

// Struct node untuk singly linked list asteroid shower
struct AsteroidShowerData {
    Asteroid asteroids;          // Data asteroid
    AsteroidShowerData *next;    // Pointer ke node berikutnya
};

// Struct untuk priority event (disimpan dalam queue)
struct PriorityEvent {
    int type;      // EventType (NORMAL atau ASTEROID_SHOWER)
    int priority;  // Priority (higher = lebih penting)
    bool valid;    // Flag apakah event valid
};

// Kelas untuk mengatur asteroid menggunakan multiple data structures:
// 1. Circular array buffer untuk pooling asteroids (asteroid[20])
// 2. Singly linked list untuk asteroid shower
// 3. Priority queue untuk event spawning
class AsteroidManager
{
private:
    DifficultyManager difficultyManager;  // Manajer kesulitan

    // Circular array buffer untuk asteroid pooling
    Asteroid asteroid[20];
    int asteroidFront = 0;  // Front index (untuk dequeuing)
    int asteroidBack = 0;   // Back index (untuk enqueuing)
    int asteroidCount = 0;  // Jumlah asteroid aktif
    void activateAsteroid(int diff);  // Aktifkan asteroid baru
    Asteroid* checkAsteroid(char charTyped);  // Cari asteroid dengan karakter pertama

    // Singly linked list untuk asteroid shower
    AsteroidShowerData* asteroidShowerHead = nullptr;
    AsteroidShowerData* current = nullptr;
    float showerActivateTime = 0.0f;
    void initShowerLinkedList();  // Inisialisasi linked list shower
    void addShowerNode();          // Tambah node baru ke linked list
    void executeShowerWave();      // Eksekusi wave shower
    Asteroid* checkAsteroidShower(char charTyped);  // Cari asteroid di shower

    // Priority queue untuk events (circular buffer)
    PriorityEvent eventQueue[10];
    int eventFront = 0;
    int eventBack = 0;
    int eventCount = 0;
    float eventNormalTime = 0.0f;
    float eventShowerTime = 0.0f;
    float eventExecuteTime = 0.0f;
    float eventAddNode = 0.0f;
    void enqueueEvent(int type, int priority);  // Tambah event ke queue
    PriorityEvent dequeueEvent();               // Ambil event priority tertinggi
    void executeEvent(const PriorityEvent& event);  // Eksekusi event

public:
    AsteroidManager();
    ~AsteroidManager();

    void clear();  // Reset semua structures

    Asteroid *getTarget(char charTyped);  // Cari target berdasarkan input karakter
    bool collisionWithPlayer();           // Cek collision dengan player

    void update(float deltaTime);  // Update semua systems
    void draw();                   // Render semua asteroid
};