#pragma once

#include <iostream>

// menyimpan node dengan tipe data T
template <typename T>
struct Node
{
    T data;
    Node *next;

    Node(const T& value) : data(value), next(nullptr) {}
};

// class singly linked list yang bisa digunakan untuk semua tipe data
template <typename T>
class SinglyLinkedList
{
private:
    Node<T> *head;
    Node<T> *current;
    int size;

public:
    SinglyLinkedList() : head(nullptr), current(nullptr), size(0) {}

    ~SinglyLinkedList() { clear(); }

    // ==== SINGLY LINKED LIST METHOD ====

    // menambahkan data ke depan linked list
    // @param value = data yang ingin ditambahkan
    void push_front(const T &value) { 
        Node<T>* newNode = new Node<T>(value);
        newNode->next = head;
        head = newNode;
        if (current == nullptr) current = head;
        size++;
    }

    // menambahkan data ke belakang linked list
    // @param value = data yang ingin ditambahkan
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

    // menghapus 1 data yang ada di depan
    void pop_front() {
        if (isEmpty()) return;
        if (current == head) current = head->next;
        Node<T>* temp = head;
        head = head->next;
        delete temp;
        size--;
    }

    // menghapus 1 data yang ada di belakang
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

    // menghapus data sesuai dengan value
    // @param value = data yang ingin dihapus
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

    // memindahkan current node ke node selanjutnya
    void next() {
        if (current != nullptr) {
            current = current->next;
        }
    }

    // mengambil alamat memori current node
    T *getCurrent() {
        if (current == nullptr) return nullptr;
        return &(current->data);
    }

    // membersihkan memori
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

    // menampilkan data di linked list
    void display() const {
        Node<T>* temp = head;
        while (temp != nullptr) {
            std::cout << temp->data << " -> ";
            temp = temp->next;
        }
        std::cout << "nullptr" << std::endl;
    }

    // current ke posisi awal (head)
    void resetCursor() { current = head; }

    // mendapatkan pointer ke head node (untuk iterasi eksternal)
    Node<T>* getHead() const { return head; }

    // melihat data head
    T *peekFront() const {
        if (isEmpty()) return nullptr;
        return &(head->data);
    }

    // mendapatkan size dari list
    int getSize() const {
        return size;
    }

    // mengecek apakah list kosoong atau tidak
    bool isEmpty() const {
        return head == nullptr;
    }
};
