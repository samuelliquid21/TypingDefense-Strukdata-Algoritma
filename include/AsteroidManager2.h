#pragma once
#include "Asteroid.h"
#include "DifficultyManager.h"
#include "Timer.h"
#include "SinglyLinkedList.h"
#include <queue>


enum EventType {
    NORMAL = 1,
    ASTEROID_SHOWER = 2
};

struct AsteroidShowerNode
{
    Asteroid asteroid;
    AsteroidShowerNode *next;
};

class AsteroidManager2
{
public:
    AsteroidManager2();
    ~AsteroidManager2();

    // === FUNGSI YANG DIPAKAI DI MODUL LAIN ===

    void clear();
    Asteroid *scanAsteroid();

    void update(float deltaTime);
    void draw();

private:
    DifficultyManager difficultyManager;

    Asteroid poolAsteroid[50]; // untuk dijadikan asteroid pooling (event normal)
    int asteroidCurrent = 0; 
    void spawnPoolAsteroid(int diff); // mengaktifkan asteroid 
    Asteroid *scanAsteroidInPool(); // untuk mengambil alamat asteroid yang aktif yang huruf pertama sama dengan apa yang di klik player

    SinglyLinkedList<Asteroid> asteroidShower;
    Asteroid *showerHead = nullptr; // head list asteroid shower
    Asteroid *showerCurrent = nullptr; 
    void initShowerList(); // generate list awal
    void addShowerNode(); // membuat node asteroid baru
    void executeShowerWave(); // memunculkan asteroid sesuai interval
    Asteroid *scanAsteroidInShower(); // untuk mengambil alamat asteroid di list yang aktif yang huruf pertama sama dengan apa yang di klik player

    std::priority_queue<EventType> eventQueue; // untuk intruksi event
    void addEvent(); // menambah event sesuai jadwal (push)
    void executeEvent(); // eksekusi event
};
