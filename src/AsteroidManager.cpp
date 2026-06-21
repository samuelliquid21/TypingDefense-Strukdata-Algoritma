#include "AsteroidManager.h"
#include "raylib.h"
#include "GameConfig.h"

// ==== CONSTRUCTOR DAN DESTRUCTOR ====

AsteroidManager::AsteroidManager() {
    // Inisialisasi awal: buat node shower awal, start semua timer
    for (int i = 0; i < Config::initialShowerNodes; i++) addShowerNode();
    timerNormal.start(Config::normalSpawnInterval);
    timerShower.start(Config::showerEventInterval);
    timerExecute.start(Config::executeEventInterval);
    timerAddNode.start(Config::addShowerNodeInterval);
}

AsteroidManager::~AsteroidManager() {}

void AsteroidManager::setExplosionCallback(std::function<void(Vector2)> cb) {
    m_explosionCallback = cb;
    // Update existing shower nodes yang dibuat sebelum setter dipanggil
    for (auto* node = asteroidShower.getHead(); node; node = node->next)
        node->data.onDestroyed = cb;
    // Update pool asteroid yang sudah aktif
    for (auto &ast : poolAsteroid)
        if (ast.active) ast.onDestroyed = cb;
}

// ==== SCANNING ====

std::vector<Asteroid*> AsteroidManager::scanAllAsteroids(std::function<bool(const Asteroid&)> predicate) {
    std::vector<Asteroid*> result;
    // Scan semua asteroid di pool, kumpulkan yang memenuhi predicate
    for (auto &ast : poolAsteroid) {
        if (predicate(ast)) result.push_back(&ast);
    }
    // Scan semua asteroid di shower (linked list), kumpulkan yang memenuhi predicate
    for (auto* node = asteroidShower.getHead(); node; node = node->next) {
        if (predicate(node->data)) result.push_back(&node->data);
    }
    return result;
}

Asteroid* AsteroidManager::scanAsteroid(std::function<bool(const Asteroid&)> predicate) {
    // Cari satu asteroid di pool yang memenuhi predicate
    for (auto &ast : poolAsteroid) {
        if (predicate(ast)) return &ast;
    }
    // Cari satu asteroid di shower yang memenuhi predicate
    for (auto* node = asteroidShower.getHead(); node; node = node->next) {
        if (predicate(node->data)) return &node->data;
    }
    return nullptr; // Tidak ditemukan
}

// ==== POOL MANAGEMENT ====

void AsteroidManager::spawnPoolAsteroid(int diff) {
    int idx = asteroidCurrent;
    // Cari slot pool yang tidak aktif secara round-robin, maksimal 50 iterasi
    for (int i = 0; i < 50; i++) {
        if (!poolAsteroid[idx].active) {
            // Aktifkan asteroid dengan tier yang ditentukan
            poolAsteroid[idx].asteroidType(diff);
            poolAsteroid[idx].onDestroyed = m_explosionCallback;
            asteroidCurrent = (idx + 1) % 50; // Geser index untuk spawn berikutnya
            if constexpr (Config::enableAsteroidLog)
                TraceLog(LOG_INFO, "[%.1f] spawnPoolAsteroid tier %d idx %d", difficultyManager.counter, diff, idx);
            return;
        }
        idx = (idx + 1) % 50;
    }
    // Jika semua slot terisi, pool penuh
    if constexpr (Config::enableAsteroidLog)
        TraceLog(LOG_WARNING, "[%.1f] spawnPoolAsteroid FAILED - pool full", difficultyManager.counter);
}

// ==== SHOWER EVENT ====

void AsteroidManager::addShowerNode() {
    // Tambah node asteroid kosong ke linked list shower
    Asteroid ast;
    ast.onDestroyed = m_explosionCallback;
    asteroidShower.push_back(ast);
}

void AsteroidManager::triggerShowerWave() {
    // Memulai wave shower: hitung jumlah node, reset cursor, start timer interval
    int count = 0;
    for (auto* n = asteroidShower.getHead(); n; n = n->next) count++;
    if constexpr (Config::enableAsteroidLog)
        TraceLog(LOG_INFO, "[%.1f] triggerShowerWave (%d nodes)", difficultyManager.counter, count);
    asteroidShower.resetCursor();
    timerShowerInterval.start(Config::showerWaveInterval);
    showerWaveActive = true;
}

void AsteroidManager::updateShowerWave(float deltaTime) {
    if (!showerWaveActive) return; // Tidak ada wave berlangsung

    timerShowerInterval.update(deltaTime);
    if (!timerShowerInterval.isDone()) return; // Belum waktunya spawn asteroid berikutnya

    // Ambil node asteroid saat ini dari cursor linked list
    auto* ast = asteroidShower.getCurrent();
    if (ast != nullptr) {
        // Aktifkan asteroid dengan tier random 1-2, lalu maju ke node berikutnya
        int tier = GetRandomValue(1, 2);
        ast->asteroidType(tier);
        if constexpr (Config::enableAsteroidLog)
            TraceLog(LOG_INFO, "[%.1f] updateShowerWave activate tier %d", difficultyManager.counter, tier);
        asteroidShower.next();
        timerShowerInterval.start(Config::showerWaveInterval);
    } else {
        // Semua node sudah diaktifkan, wave selesai
        if constexpr (Config::enableAsteroidLog)
            TraceLog(LOG_INFO, "[%.1f] updateShowerWave DONE", difficultyManager.counter);
        showerWaveActive = false;
    }
}

// ==== PRIORITY QUEUE ====

void AsteroidManager::executeEvent() {
    if (eventQueue.empty()) return;

    // Ambil event dengan prioritas tertinggi (ASTEROID_SHOWER > NORMAL)
    EventType type = eventQueue.top();
    eventQueue.pop();

    if constexpr (Config::enableAsteroidLog)
        TraceLog(LOG_INFO, "[%.1f] executeEvent %s", difficultyManager.counter, type == NORMAL ? "NORMAL" : "ASTEROID_SHOWER");
    if (type == NORMAL) {
        // Spawn asteroid normal sesuai tier dari difficulty manager
        spawnPoolAsteroid(difficultyManager.getAsteroidTier());
    } else if (type == ASTEROID_SHOWER) {
        // Trigger wave asteroid shower
        triggerShowerWave();
    }
}

// ==== DEBUG ====

int AsteroidManager::getActiveAsteroidCount() const {
    int count = 0;
    for (auto &ast : poolAsteroid) if (ast.active) count++;
    for (auto* n = asteroidShower.getHead(); n; n = n->next) if (n->data.active) count++;
    return count;
}

// ==== UPDATE & DRAW ====

void AsteroidManager::update(float deltaTime) {
    difficultyManager.updateTime();

    // Update semua asteroid aktif di pool
    for (auto &ast : poolAsteroid) {
        if (ast.active) ast.update(deltaTime);
    }
    // Update semua asteroid aktif di shower
    for (auto* node = asteroidShower.getHead(); node; node = node->next) {
        if (node->data.active) node->data.update(deltaTime);
    }

    // Proses wave shower jika sedang aktif
    updateShowerWave(deltaTime);

    // Timer spawn normal: push event NORMAL ke queue setiap interval
    timerNormal.update(deltaTime);
    if (timerNormal.isDone()) {
        eventQueue.push(NORMAL);
        timerNormal.start(Config::normalSpawnInterval);
    }

    // Timer shower: push event ASTEROID_SHOWER ke queue setiap 30 detik
    timerShower.update(deltaTime);
    if (timerShower.isDone()) {
        eventQueue.push(ASTEROID_SHOWER);
        timerShower.start(Config::showerEventInterval);
    }

    // Timer penambahan node shower: tambah node baru setiap 60 detik
    timerAddNode.update(deltaTime);
    if (timerAddNode.isDone()) {
        addShowerNode();
        timerAddNode.start(Config::addShowerNodeInterval);
    }

    // Timer eksekusi: proses satu event dari queue setiap 1 detik
    timerExecute.update(deltaTime);
    if (timerExecute.isDone()) {
        executeEvent();
        timerExecute.start(Config::executeEventInterval);
    }
}

void AsteroidManager::draw() {
    // Render semua asteroid aktif
    for (auto &ast : poolAsteroid) if (ast.active) ast.draw();
    for (auto* node = asteroidShower.getHead(); node; node = node->next)
        if (node->data.active) node->data.draw();

    // Debug overlay (hanya jika diaktifkan di config)
    if constexpr (Config::enableAsteroidLog)
        DrawText(TextFormat("Time: %.1f  Active: %d  Wave: %s",
                 difficultyManager.counter, getActiveAsteroidCount(),
                 showerWaveActive ? "ON" : "OFF"), 10, 10, 20, GREEN);
}
