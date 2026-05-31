#include "AsteroidManager2.h"
#include "raylib.h"

// ==== CONSTRUCTOR DAN DESTRUCTOR ====

AsteroidManager2::AsteroidManager2() {
    // inisiasi awal ketika class dipanggil
    for (int i = 0; i < 5; i++) addShowerNode(); // buat 5 node asteroid shower di awal
    timerNormal.start(2.0f);
    timerShower.start(30.0f);
    timerExecute.start(1.0f);
    timerAddNode.start(60.0f);
}

AsteroidManager2::~AsteroidManager2() {}

// ==== FUNGSI YANG DIPAKAI MODUL LAIN ====

std::vector<Asteroid*> AsteroidManager2::scanAllAsteroids(std::function<bool(const Asteroid&)> predicate) {
    std::vector<Asteroid*> result;
    // scan semua asteroid yang ada di asteroid pool, ketika memenuhi predicate maka push alamat memori ke result
    for (auto &ast : poolAsteroid) {
        if (predicate(ast)) result.push_back(&ast);
    }
    // scan semua asteroid yang ada di asteroid shower, ketika memenuhi predicate maka push alamat memori ke result
    asteroidShower.resetCursor();
    while (auto* ast = asteroidShower.getCurrent()) {
        if (predicate(*ast)) result.push_back(ast);
        asteroidShower.next();
    }
    // mengembalikan hasil
    return result;
}

Asteroid* AsteroidManager2::scanAsteroid(std::function<bool(const Asteroid&)> predicate) {
    // scan semua asteroid yang ada di asteorid pool, jika memenuhi predicate maka berikan alamat memeorinya
    for (auto &ast : poolAsteroid) {
        if (predicate(ast)) return &ast;
    }
    // scan semua asteroid yang ada di asteorid shower, jika memenuhi predicate maka berikan alamat memeorinya
    asteroidShower.resetCursor();
    while (auto* ast = asteroidShower.getCurrent()) {
        if (predicate(*ast)) return ast;
        asteroidShower.next();
    }
    // jika tidak ada yang memenuhi, maka hasil nullptr
    return nullptr;
}

// ==== DATA DAN FUNGSI HELPER ====

// - ASTEROID POOL

void AsteroidManager2::spawnPoolAsteroid(int diff) {
    int idx = asteroidCurrent;
    // hanya mengaktifkan asteorid yang belum aktif dan skip yang sudah
    for (int i = 0; i < 50; i++) {
        if (!poolAsteroid[idx].active) {
            poolAsteroid[idx].asteroidType(diff);
            asteroidCurrent = (idx + 1) % 50;
            return;
        }
        idx = (idx + 1) % 50;
    }
}

// - EVENT ASTEROID SHOWER

void AsteroidManager2::addShowerNode() {
    Asteroid ast;
    asteroidShower.push_back(ast);
}

void AsteroidManager2::triggerShowerWave() {
    // sebagai pemantik untuk spawn asteroid shower saat dieksekusi di event queue
    asteroidShower.resetCursor();
    timerShowerInterval.start(0.2f);
    showerWaveActive = true;
}

void AsteroidManager2::updateShowerWave(float deltaTime) {
    // jika showerWaveActive tidak aktif, maka akan diabaikan (keluar dari fungsi)
    if (!showerWaveActive) return;  

    timerShowerInterval.update(deltaTime);

    // jika belum mencapai interval, maka akan diabaikan 
    if (!timerShowerInterval.isDone()) return;

    auto* ast = asteroidShower.getCurrent();
    // jika belum nullptr, maka generate asteroid baru, pindahkan node asteorid ke setelahnya
    // dan start timer interval dari awal
    if (ast != nullptr) {
        ast->asteroidType(GetRandomValue(1, 2));
        asteroidShower.next();
        timerShowerInterval.start(0.2f);
    // jika nullptr, maka event asteroid shower telah berakhir
    } else {
        showerWaveActive = false;
    }
}

// - QUEUE EKSEKUSI EVENT

void AsteroidManager2::executeEvent() {
    if (eventQueue.empty()) return;

    // mengambil data terdepan dan menghapusnya di queue
    EventType type = eventQueue.top();
    eventQueue.pop();

    // eksekusi event sesuai type yang didapat
    if (type == NORMAL) {
        spawnPoolAsteroid(difficultyManager.getAsteroidTier());
    } else if (type == ASTEROID_SHOWER) {
        triggerShowerWave();
    }
}

// - UPDATE & DRAW

void AsteroidManager2::update(float deltaTime) {
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

void AsteroidManager2::draw() {
    // draw asteroid yang ada di pool
    for (auto &ast : poolAsteroid) {
        if (ast.active) ast.draw();
    }
    // draw asteroid yang ada di asteroid shower
    for (auto* node = asteroidShower.getHead(); node; node = node->next) {
        if (node->data.active) node->data.draw();
    }
}