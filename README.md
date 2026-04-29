## 📋 Requirements To Build

### Windows
- **CMake** (minimal v3.15)
- **C++ Compiler** (supporting C++17)
  - MinGW-w64 (recommended)
  - MSVC (Visual Studio 2019 atau lebih baru)

### Linux
- **CMake** (minimal v3.15)
- **C++ Compiler** (supporting C++17)
  - GCC 7 atau lebih baru
  - Clang 5 atau lebih baru
- **raylib** (library grafik)
  - Install via package manager

## 🔧 Instalasi & Kompilasi

### Windows
1. **Install CMake dan Compiler**:
   - Download CMake dari [cmake.org](https://cmake.org/download/)
   - Install MinGW-w64 atau Visual Studio

2. **Setup project**:
```bash
# Clone repository (jika belum)
git clone [repository-url]

# Navigasi ke direktori proyek
cd TypingDefense

# Buat folder build
mkdir build
cd build

# Generate project dengan CMake
cmake .. -G "MinGW Makefiles"  # Untuk MinGW
# atau
cmake ..  # Untuk Visual Studio

# Kompilasi
cmake --build .
```

3. **Jalankan game**:
```bash
# Dari folder build
game.exe
# atau
.\game.exe
```

### Linux
1. **Install dependencies**:
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install cmake g++ libraylib-dev

# Fedora
sudo dnf install cmake gcc-c++ raylib-devel

# Arch Linux
sudo pacman -S cmake gcc raylib
```

2. **Setup dan kompilasi**:
```bash
# Navigasi ke direktori proyek
cd TypingDefense

# Buat folder build
mkdir build && cd build

# Generate Makefile
cmake ..

# Kompilasi
make

# Jalankan game
./game
```

## 🚀 Cara Bermain
1. **Mulai game** dari menu utama
2. **Ketika asteroid muncul**, ketik kata yang tertera di atas asteroid
3. **Hindari tabrakan** dengan asteroid
4. **Dapatkan skor tinggi** untuk masuk ke leaderboard

### Kontrol
- **Keyboard**: Mengetik kata-kata yang muncul di asteroid
- **Escape**: Pause menu / kembali ke menu

## 📁 Struktur Proyek
```
TypingDefense/
├── assets/           # Aset game (gambar, suara)
│   ├── img/         # Gambar dan sprite
│   └── sound/       # Efek suara dan musik
├── data/            # Data game (leaderboard, config)
├── ext/             # Library external
│   ├── raylib/     # Raylib untuk Windows
│   └── nlohmann/   # JSON parser
├── include/         # Header files (.h)
├── src/            # Source files (.cpp)
├── build/          # Folder kompilasi
├── CMakeLists.txt  # Konfigurasi CMake
└── README.md       # Dokumentasi ini
```

## 🛠️ Troubleshooting

### Windows
**Error: "raylib.h not found"**
- Pastikan compiler mendukung C++17
- Re-run `cmake ..` untuk regenerate proyek

**Error: "undefined reference to raylib functions"**
- Gunakan command `cmake .. -G "MinGW Makefiles"` untuk MinGW
- Untuk Visual Studio, gunakan generator yang sesuai

### Linux
**Error: "Package 'raylib' not found"**
```bash
sudo apt install libraylib-dev  # Ubuntu/Debian
# atau
sudo dnf install raylib-devel   # Fedora
```

**Error saat kompilasi:**
```bash
# Bersihkan build folder dan coba lagi
rm -rf build/*
mkdir build && cd build
cmake ..
make
```

## 🔗 Links
- [Raylib](https://www.raylib.com/) - Library grafik 2D/3D sederhana
- [CMake](https://cmake.org/) - Build system
- [nlohmann/json](https://github.com/nlohmann/json) - JSON parser untuk C++
