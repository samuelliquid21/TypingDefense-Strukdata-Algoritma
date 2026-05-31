#include "AsteroidManager.h"
#include "raylib.h"
#include "GameConfig.h"

// ==== CONSTRUCTOR DAN DESTRUCTOR ====

AsteroidManager::AsteroidManager() {
    // inisiasi awal ketika class dipanggil
    for (int i = 0; i < 5; i++) addShowerNode(); // buat 5 node asteroid shower di awal
    timerNormal.start(2.0f);
    timerShower.start(30.0f);
    timerExecute.start(1.0f);
    timerAddNode.start(60.0f);
}

AsteroidManager::~AsteroidManager() {}

// ==== FUNGSI YANG DIPAKAI MODUL LAIN ====

std::vector<Asteroid*> AsteroidManager::scanAllAsteroids(std::function<bool(const Asteroid&)> predicate) {
    std::vector<Asteroid*> result;
    // scan semua asteroid yang ada di asteroid pool, ketika memenuhi predicate maka push alamat memori ke result
    for (auto &ast : poolAsteroid) {
        if (predicate(ast)) result.push_back(&ast);
    }
    // scan semua asteroid yang ada di asteroid shower, ketika memenuhi predicate maka push alamat memori ke result
    for (auto* node = asteroidShower.getHead(); node; node = node->next) {
        if (predicate(node->data)) result.push_back(&node->data);
    }
    // mengembalikan hasil
    return result;
}

Asteroid* AsteroidManager::scanAsteroid(std::function<bool(const Asteroid&)> predicate) {
    // scan semua asteroid yang ada di asteorid pool, jika memenuhi predicate maka berikan alamat memeorinya
    for (auto &ast : poolAsteroid) {
        if (predicate(ast)) return &ast;
    }
    // scan semua asteroid yang ada di asteorid shower, jika memenuhi predicate maka berikan alamat memeorinya
    for (auto* node = asteroidShower.getHead(); node; node = node->next) {
        if (predicate(node->data)) return &node->data;
    }
    // jika tidak ada yang memenuhi, maka hasil nullptr
    return nullptr;
}

// ==== DATA DAN FUNGSI HELPER ====

// - ASTEROID POOL

void AsteroidManager::spawnPoolAsteroid(int diff) {
    int idx = asteroidCurrent;
    // hanya mengaktifkan asteorid yang belum aktif dan skip yang sudah
    for (int i = 0; i < 50; i++) {
        if (!poolAsteroid[idx].active) {
            poolAsteroid[idx].asteroidType(diff);
            asteroidCurrent = (idx + 1) % 50;
            if constexpr (Config::enableAsteroidLog) // trace log untuk testing dan debugging
                TraceLog(LOG_INFO, "[%.1f] spawnPoolAsteroid tier %d idx %d", difficultyManager.counter, diff, idx);
            return;
        }
        idx = (idx + 1) % 50;
    }
    if constexpr (Config::enableAsteroidLog) // trace log untuk testing dan debugging
        TraceLog(LOG_WARNING, "[%.1f] spawnPoolAsteroid FAILED - pool full", difficultyManager.counter);
}

// - EVENT ASTEROID SHOWER

void AsteroidManager::addShowerNode() {
    Asteroid ast;
    asteroidShower.push_back(ast);
}

void AsteroidManager::triggerShowerWave() {
    // sebagai pemantik untuk spawn asteroid shower saat dieksekusi di event queue
    int count = 0;
    for (auto* n = asteroidShower.getHead(); n; n = n->next) count++;
    if constexpr (Config::enableAsteroidLog) // trace log untuk testing dan debugging
        TraceLog(LOG_INFO, "[%.1f] triggerShowerWave (%d nodes)", difficultyManager.counter, count);
    asteroidShower.resetCursor();
    timerShowerInterval.start(0.2f);
    showerWaveActive = true;
}

void AsteroidManager::updateShowerWave(float deltaTime) {
    // jika showerWaveActive tidak aktif, maka akan diabaikan (keluar dari fungsi)
    if (!showerWaveActive) return;  

    timerShowerInterval.update(deltaTime);

    // jika belum mencapai interval, maka akan diabaikan 
    if (!timerShowerInterval.isDone()) return;

    auto* ast = asteroidShower.getCurrent();
    // jika belum nullptr, maka generate asteroid baru, pindahkan node asteorid ke setelahnya
    // dan start timer interval dari awal
    if (ast != nullptr) {
        int tier = GetRandomValue(1, 2);
        ast->asteroidType(tier);
        if constexpr (Config::enableAsteroidLog) // trace log untuk testing dan debugging
            TraceLog(LOG_INFO, "[%.1f] updateShowerWave activate tier %d", difficultyManager.counter, tier);
        asteroidShower.next();
        timerShowerInterval.start(0.2f);
    // jika nullptr, maka event asteroid shower telah berakhir
    } else {
        if constexpr (Config::enableAsteroidLog) // trace log untuk testing dan debugging
            TraceLog(LOG_INFO, "[%.1f] updateShowerWave DONE", difficultyManager.counter);
        showerWaveActive = false;
    }
}

// - QUEUE EKSEKUSI EVENT

void AsteroidManager::executeEvent() {
    if (eventQueue.empty()) return;

    // mengambil data terdepan dan menghapusnya di queue
    EventType type = eventQueue.top();
    eventQueue.pop();

    // eksekusi event sesuai type yang didapat
    if constexpr (Config::enableAsteroidLog) // trace log untuk testing dan debugging
        TraceLog(LOG_INFO, "[%.1f] executeEvent %s", difficultyManager.counter, type == NORMAL ? "NORMAL" : "ASTEROID_SHOWER");
    if (type == NORMAL) {
        spawnPoolAsteroid(difficultyManager.getAsteroidTier());
    } else if (type == ASTEROID_SHOWER) {
        triggerShowerWave();
    }
}

// - TRACE LOG DRAW

int AsteroidManager::getActiveAsteroidCount() const {
    int count = 0;
    for (auto &ast : poolAsteroid) if (ast.active) count++;
    for (auto* n = asteroidShower.getHead(); n; n = n->next) if (n->data.active) count++;
    return count;
}

// - UPDATE & DRAW

void AsteroidManager::update(float deltaTime) {
    difficultyManager.updateTime();

    // jika asteroid aktif, maka update logikanya
    for (auto &ast : poolAsteroid) {
        if (ast.active) ast.update(deltaTime);
    }
    for (auto* node = asteroidShower.getHead(); node; node = node->next) {
        if (node->data.active) node->data.update(deltaTime);
    }

    // aktik ketika showerWaveActive = true
    updateShowerWave(deltaTime);

    // tambahkan event asteroid normal ke queue setiap 2 detik
    timerNormal.update(deltaTime);
    if (timerNormal.isDone()) {
        eventQueue.push(NORMAL);
        timerNormal.start(2.0f);
    }

    // tambahkan event asteroid shower ke queue setiap 30 detik
    timerShower.update(deltaTime);
    if (timerShower.isDone()) {
        eventQueue.push(ASTEROID_SHOWER);
        timerShower.start(30.0f);
    }

    // tambahkan node asteroid baru di saat event asteroid shower setiap 1 menit
    timerAddNode.update(deltaTime);
    if (timerAddNode.isDone()) {
        addShowerNode();
        timerAddNode.start(60.0f);
    }

    // eksekuis event setiap 2 detik
    timerExecute.update(deltaTime);
    if (timerExecute.isDone()) {
        executeEvent();
        timerExecute.start(2.0f);
    }
}

void AsteroidManager::draw() {
    for (auto &ast : poolAsteroid) if (ast.active) ast.draw();
    for (auto* node = asteroidShower.getHead(); node; node = node->next)
        if (node->data.active) node->data.draw();

    // trace log draw untuk testing dan debugging
    if constexpr (Config::enableAsteroidLog)
        DrawText(TextFormat("Time: %.1f  Active: %d  Wave: %s",
                 difficultyManager.counter, getActiveAsteroidCount(),
                 showerWaveActive ? "ON" : "OFF"), 10, 10, 20, GREEN);
}