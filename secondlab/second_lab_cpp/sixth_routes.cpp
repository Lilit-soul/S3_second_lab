#include <iostream>
#include <string>
#include <sstream>
#include <climits>

using namespace std;

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
    for (int i = 0; i < size; i++) ht.table[i] = nullptr;
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
    ChainNode* newNode = new ChainNode;
    newNode->key = key;
    newNode->value = value;
    newNode->next = ht.table[idx];
    ht.table[idx] = newNode;
}

int getChaining(ChainingTable& ht, const string& key) {
    int idx = hash1(key, ht.size);
    ChainNode* curr = ht.table[idx];
    while (curr) {
        if (curr->key == key) return curr->value;
        curr = curr->next;
    }
    return -1;
}

void clearChaining(ChainingTable& ht) {
    for (int i = 0; i < ht.size; i++) {
        ChainNode* curr = ht.table[i];
        while (curr) {
            ChainNode* t = curr;
            curr = curr->next;
            delete t;
        }
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
    for (int i = 0; i < size; i++) {
        ht.table1[i].isEmpty = true;
        ht.table2[i].isEmpty = true;
    }
}

int hash2(const string& key, int size) {
    int h = 0;
    for (char c : key) h = (h * 17 + c + 7) % size;
    return h;
}

void insertCuckoo(CuckooTable& ht, string key, int value, int attempt = 0) {
    if (attempt > ht.size) {
        cout << "Error: Cuckoo cycle" << endl;
        return;
    }

    int idx1 = hash1(key, ht.size);
    if (ht.table1[idx1].isEmpty) {
        ht.table1[idx1].key = key;
        ht.table1[idx1].value = value;
        ht.table1[idx1].isEmpty = false;
        return;
    }

    string evictedKey = ht.table1[idx1].key;
    int evictedValue = ht.table1[idx1].value;
    ht.table1[idx1].key = key;
    ht.table1[idx1].value = value;

    int idx2 = hash2(evictedKey, ht.size);
    if (ht.table2[idx2].isEmpty) {
        ht.table2[idx2].key = evictedKey;
        ht.table2[idx2].value = evictedValue;
        ht.table2[idx2].isEmpty = false;
        return;
    }

    string evictedKey2 = ht.table2[idx2].key;
    int evictedValue2 = ht.table2[idx2].value;
    ht.table2[idx2].key = evictedKey;
    ht.table2[idx2].value = evictedValue;

    insertCuckoo(ht, evictedKey2, evictedValue2, attempt + 1);
}

int getCuckoo(CuckooTable& ht, const string& key) {
    int idx1 = hash1(key, ht.size);
    if (!ht.table1[idx1].isEmpty && ht.table1[idx1].key == key) return ht.table1[idx1].value;

    int idx2 = hash2(key, ht.size);
    if (!ht.table2[idx2].isEmpty && ht.table2[idx2].key == key) return ht.table2[idx2].value;

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
    for (int i = 0; i < size; i++) g.table[i] = nullptr;
}

int graphHash(const string& key, int size) {
    int h = 0;
    for (char c : key) h = (h * 31 + c) % size;
    return h;
}

GraphNode* findNode(GraphHashTable& g, const string& server) {
    int idx = graphHash(server, g.size);
    GraphNode* curr = g.table[idx];
    while (curr) {
        if (curr->server == server) return curr;
        curr = curr->next;
    }
    return nullptr;
}

GraphNode* getOrCreateNode(GraphHashTable& g, const string& server) {
    GraphNode* node = findNode(g, server);
    if (node) return node;

    node = new GraphNode;
    node->server = server;
    node->edges = nullptr;
    int idx = graphHash(server, g.size);
    node->next = g.table[idx];
    g.table[idx] = node;
    return node;
}

void addRoute(GraphHashTable& g, const string& from, const string& to, int weight) {
    GraphNode* node = getOrCreateNode(g, from);
    RouteEdge* curr = node->edges;
    while (curr) {
        if (curr->to == to) {
            curr->weight = weight;
            return;
        }
        curr = curr->next;
    }
    
    RouteEdge* newEdge = new RouteEdge;
    newEdge->to = to;
    newEdge->weight = weight;
    newEdge->next = node->edges;
    node->edges = newEdge;

    getOrCreateNode(g, to);
}

int findMinDelay(GraphHashTable& g, const string& start, const string& end) {
    const int MAX_NODES = 100;
    string names[MAX_NODES];
    int distances[MAX_NODES];
    bool visited[MAX_NODES];
    int nodeCount = 0;

    auto addNodeName = [&](const string& name) {
        for (int i = 0; i < nodeCount; i++) if (names[i] == name) return;
        if (nodeCount < MAX_NODES) {
            names[nodeCount] = name;
            distances[nodeCount] = INT_MAX;
            visited[nodeCount] = false;
            nodeCount++;
        }
    };

    for (int i = 0; i < g.size; i++) {
        GraphNode* curr = g.table[i];
        while (curr) {
            addNodeName(curr->server);
            RouteEdge* e = curr->edges;
            while (e) {
                addNodeName(e->to);
                e = e->next;
            }
            curr = curr->next;
        }
    }

    int startIdx = -1;
    for (int i = 0; i < nodeCount; i++) if (names[i] == start) startIdx = i;
    if (startIdx == -1) return -1; 
    distances[startIdx] = 0;

    for (int i = 0; i < nodeCount; i++) {
        int minDist = INT_MAX;
        int minIdx = -1;
        for (int j = 0; j < nodeCount; j++) {
            if (!visited[j] && distances[j] < minDist) {
                minDist = distances[j];
                minIdx = j;
            }
        }
        
        if (minIdx == -1) break;
        visited[minIdx] = true;
        string u = names[minIdx];

        GraphNode* node = findNode(g, u);
        if (node) {
            RouteEdge* edge = node->edges;
            while (edge) {
                int vIdx = -1;
                for (int k = 0; k < nodeCount; k++) if (names[k] == edge->to) vIdx = k;
                if (vIdx != -1 && distances[minIdx] + edge->weight < distances[vIdx]) {
                    distances[vIdx] = distances[minIdx] + edge->weight;
                }
                edge = edge->next;
            }
        }
    }

    int endIdx = -1;
    for (int i = 0; i < nodeCount; i++) if (names[i] == end) endIdx = i;
    
    if (endIdx == -1 || distances[endIdx] == INT_MAX) return -1;
    return distances[endIdx];
}

void clearGraph(GraphHashTable& g) {
    for (int i = 0; i < g.size; i++) {
        GraphNode* curr = g.table[i];
        while (curr) {
            RouteEdge* e = curr->edges;
            while (e) {
                RouteEdge* t = e;
                e = e->next;
                delete t;
            }
            GraphNode* t = curr;
            curr = curr->next;
            delete t;
        }
    }
    delete[] g.table;
}

void runChaining() {
    ChainingTable ht;
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

    if (size <= 0) {
        cout << "Error: size must be positive" << endl;
        return;
    }
    initChaining(ht, size);

    cout << "Введите ключи и значения (через пробел, например: apple 10 banana 20): ";
    getline(cin, input);
    
    isEmpty = true;
    for (char c : input) if (c != ' ' && c != '\t') isEmpty = false;
    if (isEmpty) {
        cout << "Error: empty input. Table created but empty." << endl;
        clearChaining(ht);
        return;
    }

    istringstream iss(input);
    string k;
    int v;
    int count = 0;
    while (iss >> k >> v) {
        if (k.empty() || v < 0) {
            cout << "Warning: invalid pair ignored (" << k << ")" << endl;
            continue;
        }
        insertChaining(ht, k, v);
        count++;
    }
    
    if (count == 0) {
        cout << "Error: no valid pairs found" << endl;
        clearChaining(ht);
        return;
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
        
        int res = getChaining(ht, cleanKey);
        cout << (res == -1 ? "Не найдено" : "Значение: " + to_string(res)) << endl;
    }
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

    if (size <= 0) {
        cout << "Error: size must be positive" << endl;
        return;
    }
    initCuckoo(ht, size);

    cout << "Введите ключи и значения (через пробел, например: apple 10 banana 20): ";
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
    while (iss >> k >> v) {
        if (k.empty() || v < 0) {
            cout << "Warning: invalid pair ignored (" << k << ")" << endl;
            continue;
        }
        insertCuckoo(ht, k, v);
        count++;
    }
    
    if (count == 0) {
        cout << "Error: no valid pairs found" << endl;
        clearCuckoo(ht);
        return;
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
    cout << "\nВведите маршруты (Откуда Куда Задержка ... (Пример: A B 1 B C 2)) или Enter, чтобы просто найти путь\n";
    cout << "Маршруты: ";
    
    string line;
    getline(cin, line);
    
    if (!line.empty()) {
        istringstream iss(line);
        string from, to;
        int weight;
        int count = 0;
        
        while (iss >> from >> to >> weight) {
            if (weight <= 0) {
                cout << "Error: delay must be positive. Route " << from << "->" << to << " ignored." << endl;
                continue;
            }
            addRoute(g, from, to, weight);
            count++;
        }
        
        if (count > 0) cout << "Добавлено маршрутов: " << count << endl;
        else cout << "Новых валидных маршрутов не добавлено." << endl;
    }
    
    cout << "Введите начальный сервер (Enter - пропустить поиск): ";
    string start;
    getline(cin, start);
    
    if (start.empty()) {
        return;
    }
    
    cout << "Введите конечный сервер: ";
    string end;
    getline(cin, end);
    
    if (end.empty()) {
        cout << "Error: empty end server" << endl;
        return;
    }
    
    int res = findMinDelay(g, start, end);
    if (res == -1) {
        cout << "Маршрут не найден" << endl;
    } else {
        cout << "Минимальная задержка: " << res << endl;
    }
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