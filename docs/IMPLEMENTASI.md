# Implementasi — TypingDefense / Cosmic Keypad

Dokumen ini berisi daftar lengkap implementasi algoritma, struktur data, dan fitur
wajib yang telah diterapkan, lengkap dengan source code (SC) dan penjelasan kegunaannya.

---

## Daftar Isi

1. [Struktur Data](#struktur-data)
   - [Singly Linked List (Template)](#1-singly-linked-list-template)
   - [Circular Doubly Linked List](#2-circular-doubly-linked-list)
   - [Stack (Fixed Array)](#3-stack-fixed-array)
   - [Priority Queue](#4-priority-queue)
   - [Graph + BFS](#5-graph--bfs)
   - [Hash Table (unordered_map)](#6-hash-table-unordered_map)
   - [Manual Selection Sort](#7-manual-selection-sort)
   - [AVL Tree](#8-avl-tree)
2. [Algoritma & Fitur Wajib Lainnya](#algoritma--fitur-wajib-lainnya)
   - [Struct](#1-struct)
   - [References](#2-references)
   - [Pointer](#3-pointer)
   - [Namespace](#4-namespace)
   - [Callback Function](#5-callback-function)
   - [Default Argument](#6-default-argument)
   - [Function Overloading / Template](#7-function-overloading--template)
   - [Exception Handling](#8-exception-handling)
   - [STL — Vector](#9-stl--vector)
   - [STL — Iterator](#10-stl--iterator)
   - [STL — Sort](#11-stl--sort)
   - [STL — Find](#12-stl--find)
   - [STL — Count](#13-stl--count)
   - [File Handling](#14-file-handling)
   - [Lambda Function](#15-lambda-function)

---

# Struktur Data

## 1. Singly Linked List (Template)

**SC:** `include/SinglyLinkedList.h`

**Penjelasan:**
Class template `SinglyLinkedList<T>` yang mengelola node secara dinamis.
Setiap node (`Node<T>`) memiliki pointer `next` ke node berikutnya.
Digunakan di `AsteroidManager` untuk menyimpan event asteroid shower
yang jumlahnya tidak terbatas (berbeda dengan pool yang fixed-size).

**Untuk apa:**
Menyimpan node-node `Asteroid` untuk event hujan asteroid. Setiap node
baru ditambahkan secara periodik (setiap 60 detik via `timerAddNode`),
dan saat wave shower dipicu, asteroid dari linked list ini diaktifkan
satu per satu.

**Operasi:**

| Operasi | Deskripsi | Kompleksitas |
|---|---|---|
| `push_front(value)` | Tambah node di awal (sebelum head) | O(1) |
| `push_back(value)` | Tambah node di akhir (setelah tail) | O(n) |
| `pop_front()` | Hapus node pertama | O(1) |
| `pop_back()` | Hapus node terakhir | O(n) |
| `remove(value)` | Cari & hapus node pertama yang cocok | O(n) |
| `getCurrent()` | Ambil data di posisi kursor | O(1) |
| `next()` | Majukan kursor ke node berikutnya | O(1) |
| `clear()` | Hapus seluruh node | O(n) |
| `peekFront()` | Lihat data node pertama tanpa hapus | O(1) |
| `isEmpty()` | Cek apakah list kosong | O(1) |
| `getSize()` | Dapatkan jumlah node | O(1) |

**Kode (header — deklarasi template):**
```cpp
template <typename T>
struct Node {
    T data;
    Node *next;
    Node(const T& value) : data(value), next(nullptr) {}
};

template <typename T>
class SinglyLinkedList {
private:
    Node<T> *head;
    Node<T> *current;
    int size;

public:
    SinglyLinkedList() : head(nullptr), current(nullptr), size(0) {}
    ~SinglyLinkedList() { clear(); }

    void push_front(const T &value) { ... }
    void push_back(const T &value) { ... }
    void pop_front() { ... }
    void pop_back() { ... }
    bool remove(const T &value) { ... }
    void next() { ... }
    T *getCurrent() { ... }
    void clear() { ... }
    T *peekFront() const { ... }
    int getSize() const { return size; }
    bool isEmpty() const { return head == nullptr; }
};
```

**Kode (penggunaan di `AsteroidManager.h`):**
```cpp
SinglyLinkedList<Asteroid> asteroidShower;
```

---

## 2. Circular Doubly Linked List

**SC:** `include/MainMenu.h`, `src/MainMenu.cpp`, `include/UnlockedWords.h`, `src/UnlockedWords.cpp`

**Penjelasan:**
Setiap node memiliki pointer `prev` dan `next`, dan node terakhir (tail)
menunjuk kembali ke head (circular). Navigasi bisa maju/mundur tanpa
boundary check — setelah node terakhir, langsung kembali ke pertama.

**Untuk apa:**
- **MainMenu**: Navigasi 7 opsi menu (Play, Leaderboard, Skills, Word Bank,
  Credit, Logout, Quit) dengan tombol UP/DOWN. Circular berarti dari opsi
  terakhir (Quit) bisa langsung ke pertama (Play) tanpa henti.
- **UnlockedWords (Word Bank)**: Menyimpan kata-kata yang sudah di-unlock
  player. Circular memudahkan navigasi maju/mundur di daftar kata.

**Node:**
```cpp
// MainMenu
struct MenuNode {
    std::string data;
    MenuNode* prev;
    MenuNode* next;
    MenuNode(const std::string& value) : data(value), prev(nullptr), next(nullptr) {}
};

// UnlockedWords
struct WordNode {
    std::string word;
    std::string difficulty;
    std::string definition;
    WordNode* prev;
    WordNode* next;
    WordNode(const std::string& w, const std::string& d, const std::string& def);
};
```

**Operasi (MainMenu):**

| Operasi | Deskripsi | Lokasi |
|---|---|---|
| `AddOption(option)` | Tambah node baru di akhir (sebelum head) | `MainMenu.cpp:36` |
| `ClearOptions()` | Hapus semua node (do-while sampai kembali ke head) | `MainMenu.cpp:60` |
| `Update()` | Navigasi `current = current->next` (DOWN) / `current->prev` (UP) | `MainMenu.cpp:74` |
| `Draw()` | Traversal do-while dari head sampai kembali ke head | `MainMenu.cpp:98` |

**Kode — Membangun Circular DLL (MainMenu):**
```cpp
void MainMenu::AddOption(const std::string& option) {
    MenuNode* newNode = new MenuNode(option);
    if (head == nullptr) {
        head = newNode;
        newNode->next = head;  // Circular ke diri sendiri
        newNode->prev = head;
        current = head;
    } else {
        MenuNode* tail = head->prev;  // Tail = prev dari head
        tail->next = newNode;
        newNode->prev = tail;
        newNode->next = head;
        head->prev = newNode;
    }
    optionCount++;
}
```

**Kode — Navigasi Circular (MainMenu):**
```cpp
void MainMenu::Update() {
    if (IsKeyPressed(KEY_DOWN)) {
        selectedIndex++;
        if (selectedIndex >= optionCount) selectedIndex = 0; // Wrap
        current = current->next;
    }
    if (IsKeyPressed(KEY_UP)) {
        selectedIndex--;
        if (selectedIndex < 0) selectedIndex = optionCount - 1; // Wrap
        current = current->prev;
    }
    if (IsKeyPressed(KEY_ENTER)) optionChosen = true;
}
```

**Kode — Traversal Draw (MainMenu — do-while circular):**
```cpp
void MainMenu::Draw() {
    MenuNode* temp = head;
    int index = 0;
    do {
        // render tiap node...
        temp = temp->next;
        index++;
    } while (temp != head);  // Berhenti saat kembali ke head
}
```

**Operasi (UnlockedWords — Circular DLL):**

| Operasi | Deskripsi | Lokasi |
|---|---|---|
| `BuildFromPlayer(profile)` | Bangun Circular DLL dari daftar unlocked_words | `UnlockedWords.cpp:58` |
| `ClearList()` | Hapus semua node (do-while) | `UnlockedWords.cpp:24` |
| `RebuildFilter()` | Traverse Circular DLL, filter berdasarkan search | `UnlockedWords.cpp:115` |

**Kode — Membangun Circular DLL (UnlockedWords):**
```cpp
void UnlockedWords::BuildFromPlayer(const PlayerProfile& profile) {
    ClearList();
    for (const auto& word : profile.unlocked_words) {
        WordNode* newNode = new WordNode(word, difficulty, definition);
        if (m_head == nullptr) {
            m_head = newNode;
            newNode->next = m_head;  // Circular
            newNode->prev = m_head;
            m_current = m_head;
        } else {
            WordNode* tail = m_head->prev;
            tail->next = newNode;
            newNode->prev = tail;
            newNode->next = m_head;
            m_head->prev = newNode;
        }
        m_nodeCount++;
    }
}
```

---

## 3. Stack (Fixed Array)

**SC:** `include/ComboStack.h`, `src/ComboStack.cpp`

**Penjelasan:**
Stack dengan array tetap (fixed-size, maksimal 6 level). Setiap `Push()`
menggandakan multiplier level sebelumnya: 1× → 2× → 4× → 8× → 16× → 32× → 64×.
Ini adalah implementasi stack murni (LIFO) tanpa alokasi dinamis.

**Untuk apa:**
Menyimpan multiplier combo pemain. Setiap 5 kata berhasil diketik, `Push()`
dipanggil (combo naik). Saat salah ketik, `Pop()` dipanggil (combo turun satu level).
`Reset()` saat game over.

**Deklarasi:**
```cpp
class ComboStack {
private:
    static constexpr int MAX_COMBO_LEVEL = 6;
    int stack[MAX_COMBO_LEVEL]{};
    int top = 0;

public:
    void Push();            // Tambah combo: ×2 dari level sebelumnya
    void Pop();             // Kurangi combo: hapus level teratas
    void Reset();           // Reset ke 1× (kosongkan stack)
    int GetMultiplier() const;  // Dapatkan multiplier saat ini
};
```

**Operasi:**

| Operasi | Deskripsi | Kompleksitas |
|---|---|---|
| `Push()` | Tambah nilai ×2 di top, increment top | O(1) |
| `Pop()` | Kosongkan slot top, decrement top | O(1) |
| `Reset()` | Kosongkan seluruh stack | O(n) |
| `GetMultiplier()` | Return `stack[top-1]` atau 1 jika kosong | O(1) |

**Kode implementasi:**
```cpp
void ComboStack::Push() {
    if (top >= MAX_COMBO_LEVEL) return;
    int prev = top > 0 ? stack[top - 1] : 1;
    stack[top++] = prev * 2;
}

void ComboStack::Pop() {
    if (top > 0) {
        stack[top - 1] = 0;
        top--;
    }
}

void ComboStack::Reset() {
    for (int i = 0; i < top; i++) stack[i] = 0;
    top = 0;
}

int ComboStack::GetMultiplier() const {
    return top == 0 ? 1 : stack[top - 1];
}
```

---

## 4. Priority Queue

**SC:** `include/AsteroidManager.h:59`, `src/AsteroidManager.cpp`

**Penjelasan:**
`std::priority_queue<EventType>` — event dimasukkan dengan prioritas
numerik: `ASTEROID_SHOWER = 2` (lebih tinggi) dan `NORMAL = 1` (lebih rendah).
Setiap interval, event prioritas tertinggi dikeluarkan dari queue dan dieksekusi.

**Untuk apa:**
Memastikan event hujan asteroid (shower) diproses lebih dulu daripada
spawn asteroid normal, sehingga pemain mengalami variasi gameplay.

**Enum prioritas:**
```cpp
enum EventType {
    NORMAL = 1,
    ASTEROID_SHOWER = 2
};
```

**Deklarasi queue:**
```cpp
std::priority_queue<EventType> eventQueue;
```

**Operasi:**

| Operasi | Deskripsi | Kompleksitas |
|---|---|---|
| `push(event)` | Masukkan event ke queue | O(log n) |
| `pop()` | Keluarkan event prioritas tertinggi | O(log n) |
| `top()` | Lihat event prioritas tertinggi | O(1) |
| `empty()` | Cek apakah queue kosong | O(1) |

**Kode — eksekusi event (di `update`):**
```cpp
void AsteroidManager::executeEvent() {
    if (eventQueue.empty()) return;

    EventType event = eventQueue.top();
    eventQueue.pop();

    if (event == ASTEROID_SHOWER) {
        triggerShowerWave();
    } else if (event == NORMAL) {
        int diff = difficultyManager.getAsteroidTier();
        spawnPoolAsteroid(diff);
    }
}
```

---

## 5. Graph + BFS

**SC:** `include/TechTree.h`, `src/TechTree.cpp:138-176`

**Penjelasan:**
Tech Tree direpresentasikan sebagai graf berarah dengan adjacency list.
Setiap skill adalah node (`SkillName`), dan dependensi adalah edge
dari parent ke child. Algoritma BFS dari root (BARRIER) digunakan untuk
menentukan status UI setiap skill: LOCKED, AVAILABLE, atau UNLOCKED.

**Untuk apa:**
Memastikan player hanya bisa membeli skill yang sudah memiliki parent
terbuka. BFS menyebar dari BARRIER — jika parent UNLOCKED, child menjadi
AVAILABLE (bisa dibeli). Jika child juga sudah UNLOCKED, BFS lanjut ke
child tersebut.

**Struktur data graf:**
```cpp
std::unordered_map<SkillName, SkillData> skills;
std::unordered_map<SkillName, std::vector<SkillName>> adjList;
```

**Operasi:**

| Operasi | Deskripsi | Lokasi |
|---|---|---|
| `addDependency(parent, child)` | Tambah edge parent → child | `TechTree.cpp` |
| `updateSkillStates()` | BFS dari BARRIER, update uiState | `TechTree.cpp:138` |
| `unlockSkill(name, profile)` | Unlock skill jika AVAILABLE + RP cukup | `TechTree.cpp:101` |
| `handleInput(mousePos, click)` | Deteksi klik pada node AVAILABLE | `TechTree.cpp:114` |

**Kode — BFS untuk update state:**
```cpp
void TechTree::updateSkillStates() {
    // Step 1: Set state dasar berdasarkan isUnlocked
    for (auto& [key, skill] : skills) {
        skill.uiState = skill.isUnlocked ? UNLOCKED : LOCKED;
    }

    // Step 2: BFS dari BARRIER
    std::queue<SkillName> q;
    std::unordered_map<SkillName, bool> visited;

    q.push(SkillName::BARRIER);
    visited[SkillName::BARRIER] = true;

    while (!q.empty()) {
        SkillName current = q.front();
        q.pop();

        if (skills[current].isUnlocked) {
            for (SkillName child : adjList[current]) {
                if (!visited[child]) {
                    visited[child] = true;
                    if (!skills[child].isUnlocked)
                        skills[child].uiState = AVAILABLE;
                    else
                        q.push(child);  // Lanjut BFS
                }
            }
        }
    }
}
```

---

## 6. Hash Table (unordered_map)

**SC:** `include/word_module.h`, `include/TechTree.h:39`

**Penjelasan:**
`std::unordered_map` menyediakan operasi lookup O(1) rata-rata.
Digunakan di dua tempat utama.

**Untuk apa:**
- **word_module.h**: Memetakan kata (string) ke definisi Bahasa Indonesianya.
  Tiga map terpisah untuk Easy, Medium, Hard.
- **TechTree.h**: Memetakan `SkillName` (enum) ke `SkillData` (struct data skill).
  Juga untuk adjacency list: `SkillName` → `vector<SkillName>` (child nodes).

**Kode — Definisi kata:**
```cpp
inline const std::unordered_map<std::string, std::string> easy_definitions = {
    {"a", "Artikel tak tentu dalam bahasa Inggris"},
    {"ox", "Sapi jantan besar..."},
    // ...50 entries
};
```

**Kode — Tech Tree data:**
```cpp
std::unordered_map<SkillName, SkillData> skills;
std::unordered_map<SkillName, std::vector<SkillName>> adjList;
```

**Operasi:**

| Operasi | Deskripsi | Kompleksitas |
|---|---|---|
| `map[key] = value` | Insert/update | O(1) rata-rata |
| `map.at(key)` | Access dengan bounds check | O(1) rata-rata |
| `map.find(key)` | Cari key, return iterator | O(1) rata-rata |
| Range-based for | Iterasi semua entry | O(n) |

---

## 7. Manual Selection Sort

**SC:** `src/UnlockedWords.cpp:138-158`

**Penjelasan:**
Algoritma Selection Sort O(n²) yang diimplementasikan secara manual
(tidak memakai `std::sort`). Mengurutkan `m_filteredNodes` (vector
of `WordNode*`) berdasarkan `word` secara alfabetis.

**Untuk apa:**
Fitur sorting di Word Bank — tekan tombol **S** untuk A-Z (ascending)
atau **D** untuk Z-A (descending). Tidak menggunakan `std::sort` sebagai
syarat implementasi sorting manual.

**Kode:**
```cpp
void UnlockedWords::SortManual() {
    int n = (int)m_filteredNodes.size();
    for (int i = 0; i < n - 1; i++) {
        int best = i;
        for (int j = i + 1; j < n; j++) {
            if (m_sortAscending) {
                if (m_filteredNodes[j]->word < m_filteredNodes[best]->word)
                    best = j;
            } else {
                if (m_filteredNodes[j]->word > m_filteredNodes[best]->word)
                    best = j;
            }
        }
        if (best != i) {
            WordNode* temp = m_filteredNodes[i];
            m_filteredNodes[i] = m_filteredNodes[best];
            m_filteredNodes[best] = temp;
        }
    }
}
```

**Kompleksitas:** O(n²) — worst case dan average case.

---

## 8. AVL Tree

**SC:** `include/Leaderboard.h:26-58`, `src/Leaderboard.cpp:39-112`

**Penjelasan:**
AVL Tree (self-balancing binary search tree) yang menyimpan data pemain
terurut secara **descending by score**. Setiap insert otomatis melakukan
rotasi kiri/kanan untuk menjaga tinggi tree tetap O(log n).

**Untuk apa:**
Menyimpan dan mengurutkan data leaderboard. Saat data dimuat dari JSON,
setiap player di-insert ke AVL Tree. In-order traversal (left→root→right)
langsung menghasilkan urutan descending karena insert logic menempatkan
score lebih besar di kiri.

**Struktur Node:**
```cpp
struct AVLNode {
    PlayerData  data;
    AVLNode*    left;
    AVLNode*    right;
    int         height;
    AVLNode(const PlayerData& d)
        : data(d), left(nullptr), right(nullptr), height(1) {}
};
```

**Operasi:**

| Operasi | Deskripsi | Kompleksitas |
|---|---|---|
| `insert(d)` | Insert descending by score, otomatis balance | O(log n) |
| `toSortedVector(out)` | In-order traversal → vector descending | O(n) |
| `clear()` | Hapus semua node (post-order) | O(n) |
| `empty()` | Cek apakah tree kosong | O(1) |

**Kode — Rotasi Kanan:**
```cpp
AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* x  = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left  = T2;
    updateHeight(y);
    updateHeight(x);
    return x;
}
```

**Kode — Rotasi Kiri:**
```cpp
AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    AVLNode* y  = x->right;
    AVLNode* T2 = y->left;
    y->left  = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);
    return y;
}
```

**Kode — Balance (cek balance factor + double rotation jika perlu):**
```cpp
AVLNode* AVLTree::balance(AVLNode* n) {
    updateHeight(n);
    int bf = balanceFactor(n);
    // Left heavy
    if (bf > 1) {
        if (balanceFactor(n->left) < 0) n->left = rotateLeft(n->left);
        return rotateRight(n);
    }
    // Right heavy
    if (bf < -1) {
        if (balanceFactor(n->right) > 0) n->right = rotateRight(n->right);
        return rotateLeft(n);
    }
    return n;
}
```

**Kode — Insert descending (score besar di kiri):**
```cpp
AVLNode* AVLTree::insert(AVLNode* n, const PlayerData& d) {
    if (!n) return new AVLNode(d);
    if (d.score >= n->data.score) n->left  = insert(n->left,  d);
    else                          n->right = insert(n->right, d);
    return balance(n);
}
```

**Kode — In-order traversal (left → root → right = descending):**
```cpp
void AVLTree::inorderDesc(AVLNode* n, std::vector<PlayerData>& out) const {
    if (!n) return;
    inorderDesc(n->left,  out);
    out.push_back(n->data);
    inorderDesc(n->right, out);
}
```

---

# Algoritma & Fitur Wajib Lainnya

## 1. Struct

**SC:** Banyak file

**Penjelasan:**
Struct digunakan di seluruh proyek untuk mengelompokkan data terkait
tanpa method kompleks. Ini memenuhi syarat implementasi struct.

**Daftar struct:**

| Struct | File | Untuk apa |
|---|---|---|
| `PlayerProfile` | `DataManager.h:9` | Data profil player (username, score, RP, unlocked items) |
| `Asteroid` | `Asteroid.h:5` | Data satu asteroid (posisi, velocity, kata, status) |
| `SkillData` | `TechTree.h:27` | Data skill di tech tree (nama, cost, posisi, state) |
| `DictionaryEntry` | `Dictionary.h:6` | Entry kamus (kata + label kesulitan) |
| `WordNode` | `UnlockedWords.h:6` | Node Circular DLL untuk Word Bank |
| `MenuNode` | `MainMenu.h:6` | Node Circular DLL untuk Main Menu |
| `CreditEntry` | `Credit.h:7` | Entry credit screen (role + nama) |
| `Node<T>` (template) | `SinglyLinkedList.h:7` | Node generic untuk Singly Linked List |

---

## 2. References

**SC:** Banyak file (pass-by-reference di parameter function)

**Penjelasan:**
Parameter pass-by-reference (`&`) digunakan untuk menghindari kopi data
dan memungkinkan modifikasi langsung. Memenuhi syarat implementasi references.

**Contoh — TechTreeUI menyimpan referensi ke TechTree:**
```cpp
// TechTreeUI.h
class TechTreeUI {
    TechTree& m_tree;  // Reference ke backend tree
    // ...
};

// TechTreeUI.cpp — constructor
TechTreeUI::TechTreeUI(TechTree& tree) : m_tree(tree) { ... }
```

**Contoh — Callback reference parameter:**
```cpp
// GameplayManager.h
using AsteroidDestroyedCallback = std::function<void(const std::string&)>;
```

**Contoh — Fungsi dengan reference parameter:**
```cpp
// DataManager
bool FindPlayer(const std::string& username, PlayerProfile& outProfile);
void SavePlayer(const PlayerProfile& profile);
```

---

## 3. Pointer

**SC:** `GameplayManager.h:29`, `Game.h:33`, linked list nodes

**Penjelasan:**
Pointer digunakan untuk alokasi dinamis (heap) dan navigasi linked list.

**Contoh — Pointer ke objek:**
```cpp
// Game.h
GameplayManager* gameplayManager;  // Pointer ke gameplay manager

// Game.cpp
void Game::restartGame() {
    auto* newMgr = new GameplayManager();
    delete gameplayManager;
    gameplayManager = newMgr;
}
```

**Contoh — Pointer ke asteroid target:**
```cpp
// GameplayManager.h
Asteroid* currentTarget;  // Pointer ke asteroid yang sedang diketik
```

**Contoh — Pointer di linked list nodes:**
```cpp
// SinglyLinkedList.h
Node<T> *head;
Node<T> *current;
```

---

## 4. Namespace

**SC:** `include/GameConfig.h:5`, `src/Asteroid.cpp:11`

**Penjelasan:**
`namespace` digunakan untuk mengelompokkan konstanta dan menghindari
polusi nama global. Juga anonymous namespace untuk fungsi helper
yang hanya dipakai di satu file.

**Contoh — Namespace Config:**
```cpp
// GameConfig.h
namespace Config {
    constexpr int screenWidth{1080};
    constexpr int screenHeight{720};
    constexpr Vector2 playerStartPos{screenWidth - 80, screenHeight / 2};
    // ...
}
```

**Contoh — Anonymous namespace:**
```cpp
// Asteroid.cpp (atau WordGenerator.cpp)
namespace {
    std::mt19937 rng(std::random_device{}());
    const std::string& pick(const std::vector<std::string>& words) {
        std::uniform_int_distribution<> dist(0, words.size() - 1);
        return words[dist(rng)];
    }
}
```

---

## 5. Callback Function

**SC:** `include/GameplayManager.h:33-34`, `src/Game.cpp:58-67`

**Penjelasan:**
Callback function (`std::function`) digunakan agar `GameplayManager`
dapat berkomunikasi kembali ke `Game` tanpa akses langsung ke
`m_currentPlayer`. Callback dipasang di constructor Game dan di
`restartGame()`.

**Untuk apa:**
Saat pemain berhasil mengetik satu kata penuh, `GameplayManager`
memanggil callback `m_asteroidDestroyedCallback` dengan kata tersebut.
`Game` kemudian menambahkan kata ke `unlocked_words`, menambah RP,
dan menyimpan ke file.

**Deklarasi:**
```cpp
// GameplayManager.h
using AsteroidDestroyedCallback = std::function<void(const std::string&)>;

AsteroidDestroyedCallback m_asteroidDestroyedCallback;
void SetAsteroidDestroyedCallback(AsteroidDestroyedCallback cb);
```

**Pemasangan callback (Game.cpp):**
```cpp
void Game::setupCallbacks() {
    gameplayManager->SetAsteroidDestroyedCallback(
        [this](const std::string& word) {
            for (const auto& w : m_currentPlayer.unlocked_words)
                if (w == word) return;
            m_currentPlayer.unlocked_words.push_back(word);
            m_currentPlayer.research_point += 10;
        }
    );
}
```

---

## 6. Default Argument

**SC:** `include/Asteroid.h:26`

**Penjelasan:**
Parameter fungsi dengan nilai default sehingga bisa dipanggil tanpa
argumen tertentu.

**Untuk apa:**
Fungsi `asteroidType()` bisa dipanggil dengan atau tanpa parameter tier.
Jika tanpa argumen, tier default = 1 (Easy).

```cpp
// Asteroid.h
void asteroidType(const int tier = 1);
```

---

## 7. Function Overloading / Template

**SC:** `include/GameplayManager.h:48-49`, `include/SinglyLinkedList.h`

**Penjelasan:**
- **Function overloading**: Dua fungsi dengan nama sama `AddScore` tetapi
  parameter berbeda.
- **Template**: Class `SinglyLinkedList<T>` bisa dipakai dengan tipe data
  apa pun.

**Function overloading:**
```cpp
// GameplayManager.h
void AddScore(int baseScore);
void AddScore(int baseScore, int multiplier);
```

**Template class:**
```cpp
// SinglyLinkedList.h
template <typename T>
class SinglyLinkedList { ... };

// Usage di AsteroidManager.h
SinglyLinkedList<Asteroid> asteroidShower;
```

---

## 8. Exception Handling

**SC:** `src/DataManager.cpp`, `src/TechTree.cpp`

**Penjelasan:**
Try-catch untuk menangani error runtime tanpa crash:
- Parse error saat load JSON
- File write error saat save
- Type mismatch saat baca data player
- Invalid skill name saat load dari profile

**DataManager — try-catch:**
```cpp
void DataManager::load(const std::string& path) {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            m_data = json::array();
            return;
        }
        file >> m_data;  // Bisa throw json::parse_error
    } catch (const json::parse_error& e) {
        // Handle error: reset ke array kosong
        m_data = json::array();
    } catch (const std::exception& e) {
        // Fallback
        m_data = json::array();
    }
}
```

**TechTree — throw:**
```cpp
SkillName TechTree::SkillNameFromString(const std::string& str) {
    if (str == "BARRIER") return BARRIER;
    if (str == "AURA_FIELD") return AURA_FIELD;
    // ...
    throw std::invalid_argument("Unknown skill: " + str);
}
```

---

## 9. STL — Vector

**SC:** Banyak file

**Penjelasan:**
`std::vector` digunakan di banyak tempat untuk menyimpan kumpulan data
dinamis.

**Contoh penggunaan:**
```cpp
// DataManager.h
std::vector<std::string> unlocked_words;
std::vector<std::string> unlocked_skills;

// Dictionary.h
std::vector<DictionaryEntry> m_entries;
std::vector<DictionaryEntry> m_filteredEntries;
std::vector<std::string> m_definitionLines;

// TechTree.h
std::vector<SkillName> — adjacency list child nodes

// UnlockedWords.h
std::vector<WordNode*> m_filteredNodes;
```

---

## 10. STL — Iterator

**SC:** `src/Dictionary.cpp:37-38`, `src/Dictionary.cpp:130-131`

**Penjelasan:**
Iterator digunakan untuk traversing kontainer STL.

**Contoh — Loop dengan begin/end:**
```cpp
void Dictionary::LoadWords() {
    // Gabungkan easy, medium, hard dari word_module.h
    for (auto it = easy.begin(); it != easy.end(); ++it) {
        m_entries.push_back({*it, "Easy"});
    }
    // ...
}
```

---

## 11. STL — Sort

**SC:** `src/Dictionary.cpp:32-35`

**Penjelasan:**
`std::sort` untuk mengurutkan dictionary entries secara alfabetis.

```cpp
void Dictionary::LoadWords() {
    // ... gabung semua kata ...
    std::sort(m_entries.begin(), m_entries.end(),
        [](const DictionaryEntry& a, const DictionaryEntry& b) {
            return a.word < b.word;
        });
}
```

---

## 12. STL — Find

**SC:** `src/Dictionary.cpp:79-82`, `src/UnlockedWords.cpp:55-68`

**Penjelasan:**
`std::find` untuk mencari substring atau kata dalam kontainer.

**Dictionary — boost search dengan std::find:**
```cpp
// Cari kata di m_wordStrings untuk auto-boost
auto it = std::find(m_wordStrings.begin(), m_wordStrings.end(), query);
```

**UnlockedWords — cari kata di pool:**
```cpp
auto it = std::find(easy.begin(), easy.end(), word);
if (it != easy.end()) {
    difficulty = "Easy";
    definition = easy_definitions.at(word);
}
```

---

## 13. STL — Count

**SC:** `src/Dictionary.cpp:44-46`

**Penjelasan:**
`std::count` untuk menghitung jumlah kata per tingkat kesulitan.

```cpp
m_easyCount   = std::count(m_entries.begin(), m_entries.end(),
    [](const DictionaryEntry& e) { return e.difficulty == "Easy"; });
```

---

## 14. File Handling

**SC:** `src/DataManager.cpp`

**Penjelasan:**
Baca/tulis file `PlayerData.json` menggunakan `ifstream`/`ofstream`
dan library JSON nlohmann.

```cpp
void DataManager::load(const std::string& path) {
    std::ifstream file(path);
    if (file.is_open()) {
        file >> m_data;
    }
}

void DataManager::save() const {
    std::ofstream file(m_filePath);
    file << m_data.dump(4);  // Indentasi 4 spasi
}
```

**Path data:** `./data/PlayerData.json`

---

## 15. Lambda Function

**SC:** `src/Game.cpp:59-66`, `src/Dictionary.cpp:33-34`, `src/GameplayManager.cpp:93-96`

**Penjelasan:**
Lambda digunakan untuk callback, comparator, dan predicate scan.

**Callback lambda (Game.cpp):**
```cpp
gameplayManager->SetAsteroidDestroyedCallback(
    [this](const std::string& word) {
        m_currentPlayer.unlocked_words.push_back(word);
        m_currentPlayer.research_point += 10;
    }
);
```

**Sort comparator lambda (Dictionary.cpp):**
```cpp
std::sort(m_entries.begin(), m_entries.end(),
    [](const DictionaryEntry& a, const DictionaryEntry& b) {
        return a.word < b.word;
    });
```

**Scan predicate lambda (GameplayManager.cpp):**
```cpp
asteroidManager.scanAllAsteroids([&](const Asteroid& ast) {
    return ast.active;
});
```
