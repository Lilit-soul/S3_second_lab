#include <iostream>
#include <string>
#include <sstream>
#include <limits>

using namespace std;

struct CacheNode {
    string key;
    int value;
    int freq;
    int order;
    CacheNode* next;
};

struct LFUCache {
    int capacity;
    int size;
    CacheNode* head;
    int globalOrder;
};

void initLFU(LFUCache& cache, int cap) {
    if (cap <= 0) {
        cout << "Error: Capacity must be > 0. Setting to 1.\n";
        cap = 1;
    }
    cache.capacity = cap;
    cache.size = 0;
    cache.head = nullptr;
    cache.globalOrder = 0;
}

void removeNode(LFUCache& cache, CacheNode* prev, CacheNode* curr) {
    if (prev) prev->next = curr->next;
    else cache.head = curr->next;
    delete curr;
    cache.size--;
}

void setLFU(LFUCache& cache, const string& key, int value) {
    CacheNode* curr = cache.head;
    while (curr) {
        if (curr->key == key) {
            curr->value = value;
            curr->freq++;
            curr->order = ++cache.globalOrder;
            return;
        }
        curr = curr->next;
    }

    if (cache.size >= cache.capacity) {
        CacheNode* minNode = cache.head;
        CacheNode* minPrev = nullptr;
        CacheNode* p = nullptr;
        CacheNode* c = cache.head;
        while (c) {
            if (c->freq < minNode->freq ||
                (c->freq == minNode->freq && c->order < minNode->order)) {
                minNode = c;
                minPrev = p;
            }
            p = c;
            c = c->next;
        }
        if (minNode) removeNode(cache, minPrev, minNode);
    }

    CacheNode* newNode = new CacheNode{key, value, 1, ++cache.globalOrder, cache.head};
    cache.head = newNode;
    cache.size++;
}

int getLFU(LFUCache& cache, const string& key) {
    CacheNode* curr = cache.head;
    while (curr) {
        if (curr->key == key) {
            curr->freq++;
            curr->order = ++cache.globalOrder;
            return curr->value;
        }
        curr = curr->next;
    }
    return -1;
}

void clearLFU(LFUCache& cache) {
    CacheNode* curr = cache.head;
    while (curr) {
        CacheNode* t = curr;
        curr = curr->next;
        delete t;
    }
    cache.head = nullptr;
}

bool readInt(int& val) {
    string input;
    if (!getline(cin, input)) return false;
    bool isEmpty = true;
    for (char c : input) if (c != ' ' && c != '\t') { isEmpty = false; break; }
    if (isEmpty) return false;
    try {
        size_t pos;
        val = stoi(input, &pos);
        for (size_t i = pos; i < input.length(); ++i)
            if (input[i] != ' ' && input[i] != '\t') return false;
        return true;
    } catch (...) {
        return false;
    }
}

int main() {
    LFUCache cache;
    int cap, q;

    cout << "Введите ёмкость кэша: ";
    if (!readInt(cap)) {
        cout << "Error: invalid input\n";
        return 1;
    }
    initLFU(cache, cap);

    cout << "Введите количество запросов: ";
    if (!readInt(q)) {
        cout << "Error: invalid input\n";
        clearLFU(cache);
        return 1;
    }


    cout << "Введите запросы (например: SET 1 10, GET 1):\n";
    for (int i = 0; i < q; ++i) {
        string line;
        if (!getline(cin, line)) break;
        bool isEmpty = true;
        for (char c : line) if (c != ' ' && c != '\t') { isEmpty = false; break; }
        if (isEmpty) {
            cout << "Error: empty query\n";
            clearLFU(cache);
            return 1;
        }

        istringstream iss(line);
        string cmd, key;
        int value;
        iss >> cmd;

        if (cmd == "SET") {
            if (!(iss >> key >> value)) {
                cout << "Error: invalid SET format (expected: SET key value)\n";
                clearLFU(cache);
                return 1;
            }
            setLFU(cache, key, value);
            cout << "OK\n";
        } else if (cmd == "GET") {
            if (!(iss >> key)) {
                cout << "Error: invalid GET format (expected: GET key)\n";
                clearLFU(cache);
                return 1;
            }
            cout << getLFU(cache, key) << "\n";
        } else {
            cout << "Error: unknown command '" << cmd << "'\n";
        }
    }

    clearLFU(cache);
    return 0;
}