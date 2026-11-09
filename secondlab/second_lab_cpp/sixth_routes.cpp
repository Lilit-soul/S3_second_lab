#include <iostream>
#include <string>
#include <sstream>
#include <climits>

using namespace std;


template <typename T>
struct DynamicArray {
    T* data;
    int size;
    int capacity;
};

template <typename T>
void initArray(DynamicArray<T>& arr, int initialCapacity) {
    arr.capacity = initialCapacity > 0 ? initialCapacity : 1;
    arr.size = 0;
    arr.data = new T[arr.capacity];
}

template <typename T>
void pushArray(DynamicArray<T>& arr, const T& val) {
    if (arr.size >= arr.capacity) {
        int newCap = arr.capacity * 2;
        T* newData = new T[newCap];
        for (int i = 0; i < arr.size; ++i) newData[i] = arr.data[i];
        delete[] arr.data;
        arr.data = newData;
        arr.capacity = newCap;
    }
    arr.data[arr.size++] = val;
}

template <typename T>
void clearArray(DynamicArray<T>& arr) {
    delete[] arr.data;
    arr.data = nullptr;
    arr.size = 0;
    arr.capacity = 0;
}


struct ChainNode {
    string key;
    int value;
    ChainNode* next;
};

struct ChainingTable {
    ChainNode** table;
    int size;
};

void initChaining(ChainingTable& ht, int size) {
    ht.size = size;
    ht.table = new ChainNode*[size];
    for (int i = 0; i < size; ++i) ht.table[i] = nullptr;
}

int hash1(const string& key, int size) {
    int h = 0;
    for (char c : key) h = (h * 31 + c) % size;
    return h;
}

void insertChaining(ChainingTable& ht, const string& key, int value) {
    int idx = hash1(key, ht.size);
    ChainNode* curr = ht.table[idx];
    while (curr) {
        if (curr->key == key) { curr->value = value; return; }
        curr = curr->next;
    }
    ht.table[idx] = new ChainNode{key, value, ht.table[idx]};
}

int getChaining(ChainingTable& ht, const string& key) {
    int idx = hash1(key, ht.size);
    for (ChainNode* curr = ht.table[idx]; curr; curr = curr->next)
        if (curr->key == key) return curr->value;
    return -1;
}

void clearChaining(ChainingTable& ht) {
    for (int i = 0; i < ht.size; ++i) {
        ChainNode* curr = ht.table[i];
        while (curr) { ChainNode* t = curr; curr = curr->next; delete t; }
    }
    delete[] ht.table;
}


struct CuckooEntry {
    string key;
    int value;
    bool isEmpty;
};

struct CuckooTable {
    CuckooEntry* table1;
    CuckooEntry* table2;
    int size;
};

void initCuckoo(CuckooTable& ht, int size) {
    ht.size = size;
    ht.table1 = new CuckooEntry[size];
    ht.table2 = new CuckooEntry[size];
    for (int i = 0; i < size; ++i) {
        ht.table1[i].isEmpty = true;
        ht.table2[i].isEmpty = true;
    }
}

int hash2(const string& key, int size) {
    int h = 0;
    for (char c : key) h = (h * 17 + c + 7) % size;
    return h;
}

bool insertCuckoo(CuckooTable& ht, const string& key, int value, int attempt = 0) {
    if (attempt > ht.size) {
        cout << "Error: Cuckoo cycle" << endl;
        return false;
    }

    int idx1 = hash1(key, ht.size);
    if (ht.table1[idx1].isEmpty) {
        ht.table1[idx1] = {key, value, false};
        return true;
    }
    string ek = ht.table1[idx1].key;
    int ev = ht.table1[idx1].value;
    ht.table1[idx1] = {key, value, false};

    int idx2 = hash2(ek, ht.size);
    if (ht.table2[idx2].isEmpty) {
        ht.table2[idx2] = {ek, ev, false};
        return true;
    }
    string ek2 = ht.table2[idx2].key;
    int ev2 = ht.table2[idx2].value;
    ht.table2[idx2] = {ek, ev, false};

    return insertCuckoo(ht, ek2, ev2, attempt + 1);
}

int getCuckoo(CuckooTable& ht, const string& key) {
    int i1 = hash1(key, ht.size);
    if (!ht.table1[i1].isEmpty && ht.table1[i1].key == key) return ht.table1[i1].value;
    int i2 = hash2(key, ht.size);
    if (!ht.table2[i2].isEmpty && ht.table2[i2].key == key) return ht.table2[i2].value;
    return -1;
}

void clearCuckoo(CuckooTable& ht) {
    delete[] ht.table1;
    delete[] ht.table2;
}


struct RouteEdge {
    string to;
    int weight;
    RouteEdge* next;
};

struct GraphNode {
    string server;
    RouteEdge* edges;
    GraphNode* next;
};

struct GraphHashTable {
    GraphNode** table;
    int size;
};

void initGraph(GraphHashTable& g, int size) {
    g.size = size;
    g.table = new GraphNode*[size];
    for (int i = 0; i < size; ++i) g.table[i] = nullptr;
}

int graphHash(const string& key, int size) {
    int h = 0;
    for (char c : key) h = (h * 31 + c) % size;
    return h;
}

GraphNode* findNode(GraphHashTable& g, const string& server) {
    int idx = graphHash(server, g.size);
    for (GraphNode* c = g.table[idx]; c; c = c->next)
        if (c->server == server) return c;
    return nullptr;
}

GraphNode* getOrCreateNode(GraphHashTable& g, const string& server) {
    if (GraphNode* n = findNode(g, server)) return n;
    GraphNode* n = new GraphNode{server, nullptr, nullptr};
    int idx = graphHash(server, g.size);
    n->next = g.table[idx];
    g.table[idx] = n;
    return n;
}

void addRoute(GraphHashTable& g, const string& from, const string& to, int weight) {
    GraphNode* node = getOrCreateNode(g, from);
    for (RouteEdge* e = node->edges; e; e = e->next)
        if (e->to == to) { e->weight = weight; return; }
    node->edges = new RouteEdge{to, weight, node->edges};
    getOrCreateNode(g, to);
}


int findMinDelay(GraphHashTable& g, const string& start, const string& end) {
    DynamicArray<string> names;
    DynamicArray<int> distances;
    DynamicArray<bool> visited;
    initArray(names, 10);
    initArray(distances, 10);
    initArray(visited, 10);

    auto addNode = [&](const string& name) {
        for (int i = 0; i < names.size; ++i) if (names.data[i] == name) return;
        pushArray(names, name);
        pushArray(distances, INT_MAX);
        pushArray(visited, false);
    };

    for (int i = 0; i < g.size; ++i) {
        for (GraphNode* c = g.table[i]; c; c = c->next) {
            addNode(c->server);
            for (RouteEdge* e = c->edges; e; e = e->next) addNode(e->to);
        }
    }

    int startIdx = -1;
    for (int i = 0; i < names.size; ++i)
        if (names.data[i] == start) { startIdx = i; break; }
    if (startIdx == -1) {
        clearArray(names); clearArray(distances); clearArray(visited);
        return -1;
    }
    distances.data[startIdx] = 0;

    for (int i = 0; i < names.size; ++i) {
        int minDist = INT_MAX, minIdx = -1;
        for (int j = 0; j < names.size; ++j)
            if (!visited.data[j] && distances.data[j] < minDist) {
                minDist = distances.data[j]; minIdx = j;
            }
        if (minIdx == -1) break;
        visited.data[minIdx] = true;
        string u = names.data[minIdx];

        GraphNode* node = findNode(g, u);
        if (node) {
            for (RouteEdge* e = node->edges; e; e = e->next) {
                int vIdx = -1;
                for (int k = 0; k < names.size; ++k)
                    if (names.data[k] == e->to) { vIdx = k; break; }
                if (vIdx != -1 && distances.data[minIdx] + e->weight < distances.data[vIdx])
                    distances.data[vIdx] = distances.data[minIdx] + e->weight;
            }
        }
    }

    int endIdx = -1;
    for (int i = 0; i < names.size; ++i)
        if (names.data[i] == end) { endIdx = i; break; }

    int result = -1;
    if (endIdx != -1 && distances.data[endIdx] != INT_MAX) result = distances.data[endIdx];

    clearArray(names); clearArray(distances); clearArray(visited);
    return result;
}

void clearGraph(GraphHashTable& g) {
    for (int i = 0; i < g.size; ++i) {
        GraphNode* c = g.table[i];
        while (c) {
            RouteEdge* e = c->edges;
            while (e) { RouteEdge* t = e; e = e->next; delete t; }
            GraphNode* t = c; c = c->next; delete t;
        }
    }
    delete[] g.table;
}


void runChaining() {
    ChainingTable ht;
    string input;
    cout << "\nВведите размер таблицы: ";
    getline(cin, input);
    try {
        int size = stoi(input);
        if (size <= 0) { cout << "Error: size must be positive\n"; return; }
        initChaining(ht, size);
    } catch (...) {
        cout << "Error: invalid size format\n";
        return;
    }

    cout << "Введите ключи и значения (через пробел): ";
    getline(cin, input);
    istringstream iss(input);
    string k; int v; int count = 0;
    while (iss >> k >> v) {
        if (v < 0) continue;
        insertChaining(ht, k, v);
        ++count;
    }
    cout << "Добавлено элементов: " << count << "\n";

    cout << "Введите ключ для поиска: ";
    getline(cin, k);
    int res = getChaining(ht, k);
    cout << (res == -1 ? "Не найдено" : "Значение: " + to_string(res)) << "\n";

    clearChaining(ht);
}

void runCuckoo() {
    CuckooTable ht;
    string input;
    int size;

    cout << "\nВведите размер таблицы: ";
    getline(cin, input);
    bool isEmpty = true;
    for (char c : input) if (c != ' ' && c != '\t') isEmpty = false;
    if (isEmpty) { cout << "Error: empty size" << endl; return; }

    try {
        size = stoi(input);
    } catch (...) {
        cout << "Error: invalid size format" << endl;
        return;
    }
    if (size <= 0) { cout << "Error: size must be positive" << endl; return; }
    initCuckoo(ht, size);

    cout << "Введите ключи и значения (через пробел): ";
    getline(cin, input);
    isEmpty = true;
    for (char c : input) if (c != ' ' && c != '\t') isEmpty = false;
    if (isEmpty) {
        cout << "Error: empty input. Table created but empty." << endl;
        clearCuckoo(ht);
        return;
    }

    istringstream iss(input);
    string k;
    int v;
    int count = 0;
    int skipped = 0;
    int maxLoad = ht.size; 

    while (iss >> k >> v) {
        if (k.empty() || v < 0) {
            cout << "Warning: invalid pair ignored (" << k << ")" << endl;
            continue;
        }
        if (count >= maxLoad) {
            skipped++;
            continue;
        }
        insertCuckoo(ht, k, v);
        count++;
    }

    cout << "Добавлено элементов: " << count << endl;

    cout << "Введите ключ для поиска: ";
    getline(cin, input);
    isEmpty = true;
    for (char c : input) if (c != ' ' && c != '\t') isEmpty = false;

    if (isEmpty) {
        cout << "Error: empty search key" << endl;
    } else {
        string cleanKey = "";
        for (char c : input) if (c != ' ' && c != '\t') cleanKey += c;

        int res = getCuckoo(ht, cleanKey);
        cout << (res == -1 ? "Не найдено" : "Значение: " + to_string(res)) << endl;
    }
    clearCuckoo(ht);
}

void runGraph(GraphHashTable& g) {
    cout << "\nВведите маршруты (Откуда Куда Задержка ...) или Enter:\n";
    cout << "Маршруты: ";
    string line;
    getline(cin, line);
    if (!line.empty()) {
        istringstream iss(line);
        string from, to; int w; int count = 0;
        while (iss >> from >> to >> w) {
            if (w > 0) { addRoute(g, from, to, w); ++count; }
        }
        if (count > 0) cout << "Добавлено маршрутов: " << count << "\n";
    }

    cout << "Введите начальный сервер (Enter - пропустить): ";
    string start;
    getline(cin, start);
    if (start.empty()) return;

    cout << "Введите конечный сервер: ";
    string end;
    getline(cin, end);
    if (end.empty()) { cout << "Error: empty end server\n"; return; }

    int res = findMinDelay(g, start, end);
    if (res == -1) cout << "Маршрут не найден\n";
    else cout << "Минимальная задержка: " << res << "\n";
}

int main() {
    GraphHashTable g;
    initGraph(g, 100);

    int choice;
    while (true) {
        cout << "\n1.Цепочки 2.Кукушка 3.Граф 0.Выход\nВыбор: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(32767, '\n');
            continue;
        }
        cin.ignore();

        if (choice == 0) break;
        if (choice == 1) runChaining();
        else if (choice == 2) runCuckoo();
        else if (choice == 3) runGraph(g);
    }

    clearGraph(g);
    return 0;
}