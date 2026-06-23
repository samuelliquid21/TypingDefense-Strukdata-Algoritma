#pragma once

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

public:
    SinglyLinkedList() : head(nullptr), current(nullptr) {}

    ~SinglyLinkedList() { clear(); }

    // ==== METODE SINGLY LINKED LIST ====

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
    }

    // Kembalikan current ke posisi head
    void resetCursor() { current = head; }

    // Dapatkan pointer head untuk iterasi manual dari luar kelas
    Node<T>* getHead() const { return head; }



    // Cek apakah list kosong (head == nullptr)
    bool isEmpty() const {
        return head == nullptr;
    }
};
