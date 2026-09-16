#ifndef TMAP_H
#define TMAP_H
#include <cstddef>
#include <functional>

template <typename K, typename V>
class TMap
{

    struct Node {
        K key;
        V value;
        Node* next;
        Node(const K& k, const V& v) : key(k), value(v), next(nullptr) {};
    };

    Node** table;
    size_t capacity;
    size_t size;

public:

    void add(const K& key, const V& value)
    {
        size_t idx = getBucketIdx(key);
        Node* current = table[idx];

        while (current != nullptr)
        {
            if (current->key == key)
            {
                current->value = value;
                return;
            }
            current = current->next;
        }

        Node* newNode = new Node(key, value);
        newNode->next = table[idx];
        table[idx] = newNode;

        size++;
        if((double)size / capacity < 0.75) return;

        rehash();
    }

    V* find(const K& key)
    {
        size_t idx = getBucketIdx(key);
        Node* current = table[idx];
        while (current != nullptr)
        {
            if (current->key == key)
            {
                return &current->value;
            }
            current = current->next;
        }

        return nullptr;
    }

    bool remove(const K& key)
    {
        size_t idx = getBucketIdx(key);
        Node* current = table[idx];
        Node* prev = nullptr;

        while (current != nullptr)
        {
            if (current->key == key)
            {
                if (prev == nullptr)
                {
                    table[idx] = current->next;
                }
                else
                {
                    prev->next = current->next;
                }

                delete current;
                size--;
                return true;
            }

            prev = current;
            current = current->next;
        }

        return false;
    }

    TMap(size_t cap = 8) : capacity(cap), size(0)
    {
        table = new Node*[capacity];

        for(size_t idx = 0; idx < capacity; idx++)
        {
            table[idx] = nullptr;
        }
    }

    size_t getBucketIdx(const K& key) const
    {
        size_t hash = std::hash<K>{}(key);

        return hash % capacity;
    }

    ~TMap()
    {
        for(size_t idx = 0; idx < capacity; idx++)
        {
            Node* current = table[idx];
            while (current != nullptr)
            {
                Node* nextNode = current->next;
                delete current;
                current = nextNode;
            }
        }
        delete[] table;
    };

private:
    void rehash()
    {
        size_t oldCapacity = capacity;

        capacity *= 2;
        Node** oldTable = table;
        table = new Node*[capacity];

        for(size_t idx = 0; idx < capacity; idx++)
        {
            table[idx] = nullptr;
        }

        for (size_t i = 0; i < oldCapacity; ++i)
        {
            Node* current = oldTable[i];

            while (current != nullptr)
            {
                Node* nextNode = current->next;

                size_t newIdx = getBucketIdx(current->key);

                current->next = table[newIdx];
                table[newIdx] = current;

                current = nextNode;
            }
        }

        delete[] oldTable;
    }
};

#endif // TMAP_H
