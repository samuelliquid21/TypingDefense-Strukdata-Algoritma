#include "MainMenu.h"

MainMenu::MainMenu() {
    title = "Cosmic Keypad";

    head = nullptr;
    current = nullptr;
    selectedIndex = 0;
    optionCount = 0;
    optionChosen = false;

    titleFontSize = 100;
    menuFontSize = 30;

    // Tambah options ke Circular Doubly Linked List
    AddOption("Play");
    AddOption("Leaderboard");
    AddOption("Skills");
    AddOption("Word Bank");
    AddOption("Credit");
    AddOption("Quit");
}

MainMenu::~MainMenu() {
    ClearOptions();  // Cleanup memory untuk semua nodes
}

// Tambah node baru ke Circular Doubly Linked List
// Setiap node baru diletakkan di akhir list (sebelum head),
// lalu prev/next di-link agar tetap circular.
void MainMenu::AddOption(const std::string& option) {
    MenuNode* newNode = new MenuNode(option);

    if (head == nullptr) {
        // List kosong: node menjadi head dan circular ke dirinya sendiri
        // prev dan next sama-sama menunjuk ke node sendiri
        head = newNode;
        newNode->next = head;
        newNode->prev = head;
        current = head;
    } else {
        // Tambah node baru di akhir list (sebelum head)
        // Karena circular, tail = head->prev
        MenuNode* tail = head->prev;  // Tail adalah prev dari head (circular)
        tail->next = newNode;
        newNode->prev = tail;
        newNode->next = head;
        head->prev = newNode;
    }
    optionCount++;
}

// Hapus semua nodes dari Circular Doubly Linked List
// Menggunakan do-while karena circular (berhenti saat temp kembali ke head)
void MainMenu::ClearOptions() {
    if (head == nullptr) return;

    MenuNode* temp = head;
    do {
        MenuNode* next = temp->next;
        delete temp;      // Free memory untuk setiap node
        temp = next;
    } while (temp != head);

    head = nullptr;
    current = nullptr;
}

void MainMenu::Update() {
    if (IsKeyPressed(KEY_DOWN)) {
        selectedIndex++;
        if (selectedIndex >= optionCount)
            selectedIndex = 0;  // Wrap-around ke index 0 (circular)

        current = current->next;  // Navigasi: pindah ke next node (DOWN)
    }

    if (IsKeyPressed(KEY_UP)) {
        selectedIndex--;
        if (selectedIndex < 0)
            selectedIndex = optionCount - 1;  // Wrap-around ke akhir (circular)

        current = current->prev;  // Navigasi: pindah ke prev node (UP)
    }

    if (IsKeyPressed(KEY_ENTER)) {
        optionChosen = true;  // Tandai bahwa user telah memilih option
    }
}

// Render menu dengan traversal Circular Doubly Linked List
// Traversal dimulai dari head dan berhenti saat kembali ke head (do-while)
void MainMenu::Draw() {
    int titleWidth = MeasureText(title.c_str(), titleFontSize);
    // Gambar title di tengah layar horizontal, posisi Y=100
    DrawText(
        title.c_str(),
        (1080 - titleWidth) / 2,
        100,
        titleFontSize,
        WHITE
    );

    // Traversal circular list: start dari head, stop saat kembali ke head
    MenuNode* temp = head;
    int index = 0;
    do {
        std::string text = temp->data;
        int textWidth = MeasureText(text.c_str(), menuFontSize);

        int x = (1080 - textWidth) / 2;
        int y = 370 + index * 55;

        // Option yang terpilih diberi warna YELLOW, sisanya WHITE
        if (index == selectedIndex)
            DrawText(text.c_str(), x, y, menuFontSize, YELLOW);
        else
            DrawText(text.c_str(), x, y, menuFontSize, WHITE);

        temp = temp->next;  // Pindah ke node berikutnya
        index++;
    } while (temp != head);  // Berhenti saat kembali ke head (circular)
}

int MainMenu::GetSelectedIndex() const {
    return selectedIndex;
}

bool MainMenu::IsOptionChosen() const {
    return optionChosen;
}

void MainMenu::ResetChoice() {
    optionChosen = false;
}
