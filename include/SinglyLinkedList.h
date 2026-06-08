#pragma once

#include <iostream>

// Node generic untuk Singly Linked List
template <typename T>
struct Node
{
    T data;
    Node *next;

    Node(const T& value) : data(value), next(nullptr) {}
};

// Class template Singly Linked List — struktur data wajib (akademik)
// Digunakan di AsteroidManager untuk mengelola node event asteroid shower
template <typename T>
class SinglyLinkedList
{
private:
    Node<T> *head;    // Pointer ke node pertama
    Node<T> *current; // Pointer untuk kursor iterasi internal
    int size;         // Jumlah node dalam list

public:
    SinglyLinkedList() : head(nullptr), current(nullptr), size(0) {}

    ~SinglyLinkedList() { clear(); }

    // ==== METODE SINGLY LINKED LIST ====

    // Menambahkan node baru di awal list (sebelum head)
    void push_front(const T &value) { 
        Node<T>* newNode = new Node<T>(value);
        newNode->next = head;
        head = newNode;
        if (current == nullptr) current = head;
        size++;
    }

    // Menambahkan node baru di akhir list (setelah tail)
    void push_back(const T &value) {
        Node<T>* newNode = new Node<T>(value);
        if (isEmpty()) {
            head = newNode;
            current = head;
        } else {
            Node<T>* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
        size++;
    }

    // Hapus node pertama (head), geser head ke node berikutnya
    void pop_front() {
        if (isEmpty()) return;
        if (current == head) current = head->next;
        Node<T>* temp = head;
        head = head->next;
        delete temp;
        size--;
    }

    // Hapus node terakhir, cari node sebelum tail lalu hapus tail
    void pop_back() {
        if (isEmpty()) return;
        if (head->next == nullptr) {
            delete head;
            head = nullptr;
            current = nullptr;
        } else {
            Node<T>* temp = head;
            while (temp->next->next != nullptr) {
                temp = temp->next;
            }
            if (current == temp->next) current = head;
            delete temp->next;
            temp->next = nullptr;
        }
        size--;
    }

    // Cari dan hapus node pertama yang datanya sama dengan value
    bool remove(const T &value) {
        if (isEmpty()) return false;
        if (head->data == value) {
            pop_front();
            return true;
        }
        Node<T>* temp = head;
        while (temp->next != nullptr) {
            if (temp->next->data == value) {
                if (current == temp->next) current = temp;
                Node<T>* toDelete = temp->next;
                temp->next = toDelete->next;
                delete toDelete;
                size--;
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

    // Pindahkan kursor current ke node berikutnya
    void next() {
        if (current != nullptr) {
            current = current->next;
        }
    }

    // Ambil pointer ke data di node yang sedang ditunjuk current
    T *getCurrent() {
        if (current == nullptr) return nullptr;
        return &(current->data);
    }

    // Hapus seluruh node dan reset list ke kosong
    void clear() {
        Node<T>* temp = head;
        while (temp != nullptr) {
            Node<T>* nextNode = temp->next;
            delete temp;
            temp = nextNode;
        }
        head = nullptr;
        current = nullptr;
        size = 0;
    }

    // Tampilkan seluruh isi list ke console (untuk debugging)
    void display() const {
        Node<T>* temp = head;
        while (temp != nullptr) {
            std::cout << temp->data << " -> ";
            temp = temp->next;
        }
        std::cout << "nullptr" << std::endl;
    }

    // Kembalikan current ke posisi head
    void resetCursor() { current = head; }

    // Dapatkan pointer head untuk iterasi manual dari luar kelas
    Node<T>* getHead() const { return head; }

    // Lihat data di node pertama tanpa menghapus
    T *peekFront() const {
        if (isEmpty()) return nullptr;
        return &(head->data);
    }

    // Dapatkan jumlah node saat ini
    int getSize() const {
        return size;
    }

    // Cek apakah list kosong (head == nullptr)
    bool isEmpty() const {
        return head == nullptr;
    }
};
