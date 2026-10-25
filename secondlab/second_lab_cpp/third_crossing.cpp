#include <iostream>
#include <string>
#include <cctype>

using namespace std;

struct SetNode {
    string data;
    SetNode* next;
};

struct MySet {
    SetNode* head;
};

void initSet(MySet& s) { s.head = nullptr; }

bool contains(MySet& s, const string& val) {
    SetNode* curr = s.head;
    while (curr != nullptr) {
        if (curr->data == val) return true;
        curr = curr->next;
    }
    return false;
}

void add(MySet& s, const string& val) {
    if (val.empty() || contains(s, val)) return;
    SetNode* newNode = new SetNode;
    newNode->data = val;
    newNode->next = s.head;
    s.head = newNode;
}

void clearSet(MySet& s) {
    SetNode* curr = s.head;
    while (curr != nullptr) {
        SetNode* temp = curr;
        curr = curr->next;
        delete temp;
    }
    s.head = nullptr;
}

int intersectionSize(MySet& a, MySet& b) {
    int count = 0;
    SetNode* curr = a.head;
    while (curr != nullptr) {
        if (contains(b, curr->data)) count++;
        curr = curr->next;
    }
    return count;
}

bool parseSets(const string& input, MySet*& sets, int& n) {
    if (input.length() < 4 || input[0] != '{' || input[1] != '{' || 
        input[input.length()-1] != '}' || input[input.length()-2] != '}') {
        cout << "Error: invalid format. Expected {{...}, {...}}" << endl;
        return false;
    }

    n = 0;
    for (char c : input) {
        if (c == '{') n++;
    }
    n--;

    if (n < 2) {
        cout << "Error: need at least 2 sets" << endl;
        return false;
    }

    sets = new MySet[n];
    for (int i = 0; i < n; i++) initSet(sets[i]);

    int setIndex = -1;
    string currentVal = "";
    bool insideSet = false;

    for (int i = 1; i < input.length() - 1; i++) {
        char c = input[i];

        if (c == '{') {
            setIndex++;
            insideSet = true;
            currentVal = "";
        } 
        else if (c == '}') {
            if (!currentVal.empty()) {
                add(sets[setIndex], currentVal);
                currentVal = "";
            }
            insideSet = false;
        } 
        else if (c == ',' || c == ' ') {
            if (insideSet && !currentVal.empty()) {
                add(sets[setIndex], currentVal);
                currentVal = "";
            }
        } 
        else {
            if (!insideSet) {
                cout << "Error: invalid format (element outside of set)" << endl;
                return false;
            }
            if (!isdigit(c)) {
                cout << "Error: invalid character '" << c << "' (only digits allowed)" << endl;
                return false;
            }
            currentVal += c;
        }
    }
    return true;
}

void findMaxIntersection(MySet* sets, int n) {
    int maxSize = -1, bestI = -1, bestJ = -1;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int currentSize = intersectionSize(sets[i], sets[j]);
            if (currentSize > maxSize) {
                maxSize = currentSize;
                bestI = i;
                bestJ = j;
            }
        }
    }
    
    if (maxSize <= 0) {
        cout << "Пересечений не найдено." << endl;
    } else {
        cout << "Максимальное пересечение: " << maxSize << endl;
        cout << "Множества: " << bestI + 1 << " и " << bestJ + 1 << endl;
    }
}

int main() {
    string input;
    cout << "Введите множества (например, {{1, 2, 3}, {2, 3, 4}}): ";
    getline(cin, input);

    string cleanInput = "";
    for (char c : input) {
        if (c != ' ' && c != '\t') cleanInput += c;
    }

    if (cleanInput.empty()) {
        cout << "Error: empty input" << endl;
        return 1;
    }

    MySet* sets = nullptr;
    int n = 0;

    if (!parseSets(input, sets, n)) {
        if (sets != nullptr) delete[] sets;
        return 1;
    }

    findMaxIntersection(sets, n);

    for (int i = 0; i < n; i++) clearSet(sets[i]);
    delete[] sets;

    return 0;
}