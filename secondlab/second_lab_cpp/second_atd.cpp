#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

// Хеш-таблица для O(1)
struct HashNode {
    string key;
    HashNode* next;
};

struct HashSet {
    HashNode** table;
    int size;
    int count;
};

// Функция хеширования
int hashFunc(const string& key, int size) {
    int h = 0;
    for (char c : key) h = (h * 31 + c) % size;
    return h;
}

void initSet(HashSet& s, int size = 1000) {
    s.size = size;
    s.count = 0;
    s.table = new HashNode*[size];
    for (int i = 0; i < size; i++) s.table[i] = nullptr;
}

// Добавление 
void add(HashSet& s, const string& key) {
    if (key.empty()) return;
    int idx = hashFunc(key, s.size);
    
    HashNode* curr = s.table[idx];
    while (curr) {
        if (curr->key == key) return; 
        curr = curr->next;
    }
    
    HashNode* newNode = new HashNode;
    newNode->key = key;
    newNode->next = s.table[idx];
    s.table[idx] = newNode;
    s.count++;
}

// Удаление 
bool del(HashSet& s, const string& key) {
    int idx = hashFunc(key, s.size);
    HashNode* curr = s.table[idx];
    HashNode* prev = nullptr;
    
    while (curr) {
        if (curr->key == key) {
            if (prev) prev->next = curr->next;
            else s.table[idx] = curr->next;
            delete curr;
            s.count--;
            return true; // Элемент найден и удален
        }
        prev = curr;
        curr = curr->next;
    }
    return false; // Элемент не найден
}

// Проверка 
bool contains(HashSet& s, const string& key) {
    int idx = hashFunc(key, s.size);
    HashNode* curr = s.table[idx];
    while (curr) {
        if (curr->key == key) return true;
        curr = curr->next;
    }
    return false;
}

// Очистка памяти
void clearSet(HashSet& s) {
    for (int i = 0; i < s.size; i++) {
        HashNode* curr = s.table[i];
        while (curr) {
            HashNode* t = curr;
            curr = curr->next;
            delete t;
        }
    }
    delete[] s.table;
}

// Загрузка/сохранение
void loadFromFile(HashSet& s, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return;
    string val;
    while (file >> val) add(s, val);
    file.close();
}

void saveToFile(HashSet& s, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return;
    for (int i = 0; i < s.size; i++) {
        HashNode* curr = s.table[i];
        while (curr) {
            file << curr->key << "\n";
            curr = curr->next;
        }
    }
    file.close();
}

int main(int argc, char* argv[]) {
    string filename = "";
    string query = "";
    
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) filename = argv[++i];
        else if (arg == "--query" && i + 1 < argc) query = argv[++i];
    }
    
    if (filename.empty() || query.empty()) {
        cout << "Usage: ./program --file <file> --query \"<command> <value>\"" << endl;
        return 1;
    }
    
    HashSet mySet;
    initSet(mySet);
    loadFromFile(mySet, filename);
    
    istringstream iss(query);
    string cmd, val;
    iss >> cmd >> val;
    
    if (cmd == "SETADD") {
        if (val.empty()) { cout << "Error: empty value" << endl; clearSet(mySet); return 1; }
        add(mySet, val);
        saveToFile(mySet, filename);
        cout << "Added: " << val << endl;
    } 
    else if (cmd == "SETDEL") {
        if (val.empty()) { cout << "Error: empty value" << endl; clearSet(mySet); return 1; }
        
        if (del(mySet, val)) {
            saveToFile(mySet, filename);
            cout << "Deleted: " << val << endl;
        } else {
            cout << "Not found: " << val << endl;
        }
    } 
    else if (cmd == "SET_AT") {
        if (val.empty()) { cout << "Error: empty value" << endl; clearSet(mySet); return 1; }
        if (contains(mySet, val)) cout << "True" << endl;
        else cout << "False" << endl;
    } 
    else {
        cout << "Error: unknown command" << endl;
        clearSet(mySet);
        return 1;
    }
    
    clearSet(mySet);
    return 0;
}