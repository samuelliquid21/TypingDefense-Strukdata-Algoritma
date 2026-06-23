# Materi Presentasi 5 — Leaderboard + Data

## 📋 Ringkasan

Bagian ini membahas **Leaderboard** (papan peringkat pemain) dengan struktur data **AVL Tree** untuk sorting skor, dan **Data Management** (load/save JSON, dictionary kata, unlocked words). Juga mencakup **File Handling** untuk baca/tulis file JSON.

---

## 1. Leaderboard

**File:** `src/Leaderboard.cpp`, `include/Leaderboard.h`

Leaderboard menampilkan peringkat semua pemain berdasarkan **highest score**, menggunakan **AVL Tree** (self-balancing binary search tree) untuk sorting otomatis.

### Data Player di Leaderboard

```cpp
struct PlayerData {
    string name;           // Username
    int score;              // Highest score
    int wordsTyped;
    float survivalTime;     // Waktu bertahan hidup
    int rank;               // Peringkat (dihitung otomatis)
    int researchPoint;      // Research Points
};
```

### Tampilan Leaderboard

```
┌──────────────────────────────────────────────────────────────┐
│                                                              │
│                    HALL OF CHAMPIONS                         │
│                                                              │
│    ┌─────────┐         ┌─────────┐         ┌─────────┐      │
│    │  Rank 2 │         │  Rank 1 │         │  Rank 3 │      │
│    │  Nova   │         │Hyperion │         │  Orion  │      │
│    │  1800   │         │  👑 2500│         │  1500   │      │
│    └─────────┘         └─────────┘         └─────────┘      │
│                                                              │
│  RANK  │  PLAYER          │  SCORE                           │
│  ───────────────────────────────────────                     │
│  #04   │  Vega            │  1200                            │
│  #05   │  Pulsar          │  1100                            │
│  #06   │  Nebula          │  950                             │
│  #07   │  Comet           │  800                             │
│  #08   │  Stella          │  700                             │
│  #09   │  Lyra            │  600                             │
│  #10   │  Andromeda       │  500                             │
│                                                              │
│  The Typing Guardians          [W/S] NAVIGATE [ENTER] DETAIL │
└──────────────────────────────────────────────────────────────┘
```

---

## 2. AVL Tree

### Struktur Node

```cpp
struct AVLNode {
    PlayerData data;
    AVLNode* left;
    AVLNode* right;
    int height;  // Tinggi node untuk balancing
    
    AVLNode(const PlayerData& d)
        : data(d), left(nullptr), right(nullptr), height(1) {}
};
```

### Properties AVL Tree

| Aspek | Value |
|-------|-------|
| Tipe | Self-balancing Binary Search Tree |
| Balance Factor | `height(left) - height(right)` |
| Balance condition | -1 ≤ balance factor ≤ 1 |
| Rotasi | Left rotate, Right rotate, Left-Right, Right-Left |
| Kompleksitas insert | O(log n) |
| Kompleksitas traversal | O(n) |
| File | `Leaderboard.h:26-58`, `Leaderboard.cpp:40-113` |

### Insert dengan Balancing

Insert dilakukan **descending by score** (skor lebih besar → kiri):

```cpp
AVLNode* AVLTree::insert(AVLNode* n, const PlayerData& d) {
    if (!n) return new AVLNode(d);           // Buat node baru
    
    if (d.score >= n->data.score)
        n->left = insert(n->left, d);        // Skor lebih besar → subtree kiri
    else
        n->right = insert(n->right, d);      // Skor lebih kecil → subtree kanan
    
    return balance(n);                        // Balance setelah insert
}
```

### Balance Factor & Rotasi

```cpp
int balanceFactor(AVLNode* n) const {
    return n ? height(n->left) - height(n->right) : 0;
}
```

| Balance Factor | Kondisi | Aksi |
|:---:|---|---|
| > 1 | Left heavy (kiri lebih tinggi) | Right rotate |
| < -1 | Right heavy (kanan lebih tinggi) | Left rotate |
| > 1 dan balance factor left child < 0 | Left-Right case | Left rotate child, lalu Right rotate |
| < -1 dan balance factor right child > 0 | Right-Left case | Right rotate child, lalu Left rotate |

### Rotasi Kanan (Right Rotate)

```
    y                    x
   / \                  / \
  x   T3   →           T1  y
 / \                      / \
T1  T2                   T2  T3
```

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

### Rotasi Kiri (Left Rotate)

```
    x                    y
   / \                  / \
  T1  y      →         x   T3
     / \              / \
    T2  T3           T1  T2
```

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

### In-Order Traversal (Descending)

```cpp
void AVLTree::inorderDesc(AVLNode* n, vector<PlayerData>& out) const {
    if (!n) return;
    inorderDesc(n->left, out);    // Kunjungi kiri (score lebih besar)
    out.push_back(n->data);       // Simpan node saat ini
    inorderDesc(n->right, out);   // Kunjungi kanan (score lebih kecil)
}
```

Hasil traversal: **descending by score** (dari skor tertinggi ke terendah).

### Alur Leaderboard

```
Game.Init() → LeaderboardSystem::Init()
  ↓
Reset semua state (FullReset)
  ↓
Load audio + mulai musik BGM
  ↓
LoadFromJSON("data/PlayerData.json"):
  - Buka file JSON
  - Filter merge conflict markers (<<<<<<<, =======, >>>>>>>)
  - Parse JSON
  - Insert setiap player ke AVL Tree
  - Traversal AVL → sortedPlayers vector
  - Assign rank
  ↓
Update() setiap frame:
  - Update animasi (glitch, fade-in, slide)
  - Navigasi W/S untuk pilih player
  - ENTER untuk detail player
  - ESC untuk kembali (dengan animasi glitch keluar)
  ↓
Draw() setiap frame:
  - Background: grid dots, bintang, scan line
  - Glitch effect saat transisi
  - Podium (rank 1-3)
  - Tabel peringkat (rank 4+)
  - Detail popup (jika aktif)
  ↓
ESC → animasi glitch keluar → back = true → state = MENU
```

### Animasi Leaderboard

| Animasi | Durasi | Deskripsi |
|---------|--------|-----------|
| Glitch masuk | 0.6 detik | Garis acak + suara, semakin pudar |
| Fade-in konten | ~0.5 detik | Podium dan tabel muncul perlahan |
| Podium rise | ~0.5 detik | Rank 1 naik duluan, 2 & 3 menyusul |
| Row slide-in | ~0.5 detik | Baris tabel slide dari samping |
| Scan line | Terus-menerus | Garis horizontal bergerak ke bawah |
| Bintang | Terus-menerus | 90 bintang bergerak ke bawah |
| Detail popup | Instant | Overlay dengan info lengkap player |

### Navigasi Leaderboard

| Tombol | Aksi |
|--------|------|
| W / ↑ | Pilih player sebelumnya |
| S / ↓ | Pilih player berikutnya |
| ENTER | Tampilkan detail player |
| ENTER / ESC (di detail) | Tutup detail |
| ESC / BACKSPACE | Kembali ke menu (dengan glitch) |

### Detail Player Popup

```
┌────────────────────────────────────┐
│          PLAYER DETAIL             │
│────────────────────────────────────│
│  Rank   : #01                      │
│  Name   : hyperion                 │
│  Score  : 2500                     │
│  RP     : 420                      │
│  Time   : 120.5s                   │
│                                    │
│        [ENTER / ESC] CLOSE         │
└────────────────────────────────────┘
```

---

## 3. Data Management (JSON)

**File:** `src/DataManager.cpp`, `include/DataManager.h`

### Singleton Pattern

```cpp
class DataManager {
public:
    static DataManager& getInstance();  // Thread-safe (C++11 static local)
    // ...
private:
    DataManager() = default;            // Constructor private
    DataManager(const DataManager&) = delete;  // Non-copyable
};
```

### File Handling

**Load dari file:**
```cpp
void DataManager::load(const string& path) {
    try {
        ifstream file(m_filePath);
        if (file.is_open()) {
            file >> m_data;  // Parse JSON langsung dari file stream
        }
    } catch (const nlohmann::detail::parse_error& e) {
        // File corrupt → reset data
        TraceLog(LOG_WARNING, "DataManager: parse error, reset data.");
        m_data = json::object();
    }
}
```

**Save ke file:**
```cpp
void DataManager::save() const {
    try {
        ofstream file(m_filePath);
        if (!file.is_open()) {
            TraceLog(LOG_WARNING, "DataManager: cannot write %s");
            return;
        }
        file << m_data.dump(4);  // Simpan dengan indentasi 4 spasi
    } catch (const exception& e) {
        TraceLog(LOG_ERROR, "DataManager: save failed. %s", e.what());
    }
}
```

### Operasi Player

```cpp
// Cari player by username
bool FindPlayer(const string& username, PlayerProfile& outProfile);

// Update data player + save ke file
void SavePlayer(const PlayerProfile& profile);

// Buat akun baru + save ke file
void CreatePlayer(const string& username);
```

Semua operasi punya **exception handling** untuk mencegah crash saat file corrupt.

### Struktur File JSON

```json
{
    "data": [
        {
            "username": "hyperion",
            "highest_score": 2500,
            "research_point": 420,
            "survival_time": 120.5,
            "free_spin": 2,
            "active_skin": 7,
            "unlocked_words": ["ASTEROID", "SPACE", "COMET"],
            "unlocked_skills": ["barrier", "shockwave", "instant_crit"],
            "unlocked_skins": [0, 5, 10, 1, 6, 7]
        }
    ]
}
```

---

## 4. Word Dictionary & Unlocked Words

### Dictionary

**File:** `src/Dictionary.cpp`, `include/Dictionary.h`

Fitur untuk **melihat daftar kata** yang tersedia di game (150 kata: 50 Easy, 50 Medium, 50 Hard) lengkap dengan **definisi Bahasa Indonesia**.

**File:** `include/word_module.h` — 150 kata dengan definisi:
- Easy: BASIC, SPACE, TIME, GAME, WORD, TYPE, SHOOT, SCORE, LEVEL, LIFE, dll
- Medium: ASTEROID, ORBIT, COMET, NEBULA, PLANET, ROCKET, LASER, SHIELD, BOMBER, dll
- Hard: GALAXY, BLACKHOLE, COSMIC, SOLARIS, METEOR, DIAMOND, QUANTUM, dll

Navigasi: F1 dari menu utama untuk buka dictionary, scroll dengan W/S, ESC untuk kembali.

### Unlocked Words

**File:** `src/UnlockedWords.cpp`, `include/UnlockedWords.h`

Menampilkan kata-kata yang sudah berhasil diketik player saat gameplay.

**Struktur data: Circular Doubly Linked List**

```cpp
struct WordNode {
    string word;               // Kata
    string difficulty;         // Easy / Medium / Hard
    string definition;         // Definisi Bahasa Indonesia
    WordNode* prev;            // Pointer ke node sebelumnya
    WordNode* next;            // Pointer ke node berikutnya
};
```

Setiap kali player berhasil mengetik habis satu kata: kata ditambahkan ke unlocked_words dan player mendapat **+10 RP**.

**Selection Sort (manual)**

**File:** `UnlockedWords.cpp:81-130` (perkiraan)

Kata-kata bisa diurutkan secara **ascending/descending** dengan algoritma **Selection Sort** manual:

```cpp
void UnlockedWords::SelectionSort() {
    if (!m_head) return;
    
    // Selection Sort pada Circular Doubly Linked List
    WordNode* current = m_head;
    do {
        WordNode* minNode = current;
        WordNode* search = current->next;
        
        while (search != m_head) {
            if (m_sortAscending ? (search->word < minNode->word) 
                                : (search->word > minNode->word)) {
                minNode = search;
            }
            search = search->next;
        }
        
        if (minNode != current) {
            swap(current->word, minNode->word);
            swap(current->difficulty, minNode->difficulty);
            swap(current->definition, minNode->definition);
        }
        
        current = current->next;
    } while (current != m_head);
}
```

---

## 5. Implementasi Algoritma & Struktur Data

### AVL Tree

| Aspek | Detail |
|-------|--------|
| Struktur data | Self-balancing Binary Search Tree |
| Kegunaan | Menyimpan player di leaderboard, sorting otomatis by score |
| Operasi | insert, balance, inorder traversal |
| Kompleksitas | O(log n) untuk insert, O(n) untuk traversal |
| File | `Leaderboard.h:26-58`, `Leaderboard.cpp:40-113` |

### File Handling

| Aspek | Detail |
|-------|--------|
| Library | `fstream` (ifstream/ofstream) + `nlohmann/json` |
| Kegunaan | Load/save data player, leaderboard |
| Format | JSON dengan indentasi 4 spasi |
| Error handling | try-catch untuk parse error, write error |
| File | `DataManager.cpp:16-40`, `Leaderboard.cpp:159-176` |

### Exception Handling

```cpp
// Saat load JSON
try {
    ifstream file(m_filePath);
    file >> m_data;
} catch (const nlohmann::detail::parse_error& e) {
    // File corrupt → reset data
    TraceLog(LOG_WARNING, "DataManager: parse error, reset data.");
    m_data = json::object();
}

// Saat cari player
try {
    if (entry.contains("username") && entry["username"].get<string>() == username) {
        copyJsonToProfile(entry, outProfile, username);
        return true;
    }
} catch (const nlohmann::detail::type_error& e) {
    TraceLog(LOG_WARNING, "DataManager: skip corrupt entry.");
    continue;
}

// Saat simpan
try {
    ofstream file(m_filePath);
    file << m_data.dump(4);
} catch (const exception& e) {
    TraceLog(LOG_ERROR, "DataManager: save failed.");
}
```

### Selection Sort (Manual)

| Aspek | Detail |
|-------|--------|
| Algoritma | Selection Sort pada Circular DLL |
| Kegunaan | Mengurutkan unlocked words (ascending/descending) |
| Kompleksitas | O(n²) |
| File | `UnlockedWords.cpp` |

### Circular Doubly Linked List (Word Bank)

| Aspek | Detail |
|-------|--------|
| Struktur data | Circular Doubly Linked List |
| Kegunaan | Menyimpan unlocked words |
| Navigasi | next/prev untuk scroll daftar kata |
| File | `UnlockedWords.h`, `UnlockedWords.cpp` |

### Filter Merge Conflict

Leaderboard punya filter khusus saat load JSON untuk membersihkan merge conflict markers:

```cpp
while (getline(f, l)) {
    if (l.find("<<<<<<<") == string::npos && 
        l.find("=======") == string::npos && 
        l.find(">>>>>>>") == string::npos) {
        s += l + "\n";
    }
}
```

---

## 6. File yang Relevan

| File | Baris | Kegunaan |
|------|-------|----------|
| `src/Leaderboard.cpp` | 360 | AVL Tree + render leaderboard |
| `include/Leaderboard.h` | 59 | Header leaderboard (AVLNode, AVLTree) |
| `src/DataManager.cpp` | 127 | Load/save JSON, player CRUD |
| `include/DataManager.h` | 40 | Header data manager, PlayerProfile struct |
| `src/Dictionary.cpp` | 344 | Dictionary display + search |
| `include/Dictionary.h` | 78 | Header dictionary |
| `include/word_module.h` | 253 | 150 kata + definisi Bahasa Indonesia |
| `src/UnlockedWords.cpp` | 449 | Unlocked words + selection sort |
| `include/UnlockedWords.h` | 96 | Header unlocked words |
| `src/WordGenerator.cpp` | 34 | Random word picker per difficulty |
| `data/PlayerData.json` | - | Database player |

---

## 7. Jawaban untuk Tanya Jawab

| Pertanyaan | Jawaban |
|------------|---------|
| Struktur data untuk leaderboard? | AVL Tree (self-balancing BST) |
| Kenapa pake AVL Tree? | Agar sorting skor tetap balance O(log n), tidak miring seperti BST biasa |
| Apa itu balance factor? | Selisih tinggi subtree kiri dan kanan (-1, 0, atau 1) |
| Rotasi apa aja yang dipakai? | Left Rotate, Right Rotate, Left-Right, Right-Left |
| Gimana cara simpan data? | JSON file via nlohmann/json library |
| Gimana handle file corrupt? | Exception handling (try-catch), reset data |
| Selection sort untuk apa? | Mengurutkan unlocked words ascending/descending |
| Struktur data untuk unlocked words? | Circular Doubly Linked List |
| Ada berapa kata di dictionary? | 150 kata (50 Easy, 50 Medium, 50 Hard) |
| STL apa aja yang dipakai? | vector, string, find, sort, fstream (ifstream/ofstream) |
