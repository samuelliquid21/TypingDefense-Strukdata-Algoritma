#pragma once
#include <vector>
#include <functional>

template<typename K, typename V>
class HashMap {
private:
    struct Entry {
        K key;
        V value;
        Entry* next;

        Entry(const K& k, const V& v)
            : key(k), value(v), next(nullptr) {}
    };

    Entry** buckets;
    int bucketCount;
    int elementCount;

    int hashIndex(const K& key) const {
        return (int)(std::hash<K>{}(key) % bucketCount);
    }

    void rehash(int newSize) {
        Entry** oldBuckets = buckets;
        int oldCount = bucketCount;

        bucketCount = newSize;
        buckets = new Entry*[bucketCount]();
        elementCount = 0;

        for (int i = 0; i < oldCount; i++) {
            Entry* entry = oldBuckets[i];
            while (entry) {
                Entry* next = entry->next;
                entry->next = nullptr;
                int idx = hashIndex(entry->key);
                if (buckets[idx])
                    entry->next = buckets[idx];
                buckets[idx] = entry;
                elementCount++;
                entry = next;
            }
        }
        delete[] oldBuckets;
    }

public:
    HashMap(int initialSize = 16)
        : bucketCount(initialSize), elementCount(0) {
        buckets = new Entry*[bucketCount]();
        for (int i = 0; i < bucketCount; i++)
            buckets[i] = nullptr;
    }

    ~HashMap() {
        clear();
        delete[] buckets;
    }

    void put(const K& key, const V& value) {
        if ((float)elementCount / bucketCount > 0.75f)
            rehash(bucketCount * 2);

        int idx = hashIndex(key);
        Entry* entry = buckets[idx];
        while (entry) {
            if (entry->key == key) {
                entry->value = value;
                return;
            }
            entry = entry->next;
        }

        Entry* newEntry = new Entry(key, value);
        newEntry->next = buckets[idx];
        buckets[idx] = newEntry;
        elementCount++;
    }

    V* get(const K& key) {
        int idx = hashIndex(key);
        Entry* entry = buckets[idx];
        while (entry) {
            if (entry->key == key)
                return &entry->value;
            entry = entry->next;
        }
        return nullptr;
    }

    const V* get(const K& key) const {
        int idx = hashIndex(key);
        Entry* entry = buckets[idx];
        while (entry) {
            if (entry->key == key)
                return &entry->value;
            entry = entry->next;
        }
        return nullptr;
    }

    bool contains(const K& key) const {
        int idx = hashIndex(key);
        Entry* entry = buckets[idx];
        while (entry) {
            if (entry->key == key)
                return true;
            entry = entry->next;
        }
        return false;
    }

    void remove(const K& key) {
        int idx = hashIndex(key);
        Entry* entry = buckets[idx];
        Entry* prev = nullptr;
        while (entry) {
            if (entry->key == key) {
                if (prev)
                    prev->next = entry->next;
                else
                    buckets[idx] = entry->next;
                delete entry;
                elementCount--;
                return;
            }
            prev = entry;
            entry = entry->next;
        }
    }

    void clear() {
        for (int i = 0; i < bucketCount; i++) {
            Entry* entry = buckets[i];
            while (entry) {
                Entry* next = entry->next;
                delete entry;
                entry = next;
            }
            buckets[i] = nullptr;
        }
        elementCount = 0;
    }

    int size() const {
        return elementCount;
    }

    std::vector<K> keys() const {
        std::vector<K> result;
        result.reserve(elementCount);
        for (int i = 0; i < bucketCount; i++) {
            Entry* entry = buckets[i];
            while (entry) {
                result.push_back(entry->key);
                entry = entry->next;
            }
        }
        return result;
    }
};
