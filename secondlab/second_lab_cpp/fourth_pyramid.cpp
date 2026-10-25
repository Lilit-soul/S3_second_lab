#include <iostream>
#include <string>
#include <cstdio>

using namespace std;

struct Block {
    int w, h;
};

void sortBlocks(Block* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].w < arr[j+1].w || (arr[j].w == arr[j+1].w && arr[j].h < arr[j+1].h)) {
                Block temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

bool readBlocks(Block*& blocks, int& n) {
    string input;
    cout << "Введите количество блоков: ";
    
    if (!getline(cin, input)) return false;

    bool isEmpty = true;
    for (char c : input) {
        if (c != ' ' && c != '\t') { isEmpty = false; break; }
    }
    if (isEmpty) {
        cout << "Error: empty input" << endl;
        return false;
    }

    try {
        n = stoi(input);
    } catch (...) {
        cout << "Error: invalid input" << endl;
        return false;
    }

    if (n <= 0) {
        cout << "Error: N must be positive" << endl;
        return false;
    }

    blocks = new Block[n];
    cout << "Введите ширину и высоту каждого блока (через пробел):\n";
    
    for (int i = 0; i < n; i++) {
        string line;
        if (!getline(cin, line)) {
            cout << "Error: invalid block data" << endl;
            delete[] blocks;
            blocks = nullptr;
            return false;
        }
        
        bool lineEmpty = true;
        for (char c : line) {
            if (c != ' ' && c != '\t') { lineEmpty = false; break; }
        }
        if (lineEmpty) {
            cout << "Error: empty line for block data" << endl;
            delete[] blocks;
            blocks = nullptr;
            return false;
        }

        int w, h;
        char extra;
        if (sscanf(line.c_str(), "%d %d %c", &w, &h, &extra) != 2) {
            cout << "Error: invalid block data format" << endl;
            delete[] blocks;
            blocks = nullptr;
            return false;
        }
        
        if (w <= 0 || h <= 0) {
            cout << "Error: block dimensions must be positive" << endl;
            delete[] blocks;
            blocks = nullptr;
            return false;
        }
        
        blocks[i].w = w;
        blocks[i].h = h;
    }
    return true;
}

unsigned long long calculateHeight(Block* blocks, int n) {
    sortBlocks(blocks, n);
    unsigned long long total_height = 0;
    int last_width = 2147483647;

    for (int i = 0; i < n; i++) {
        if (blocks[i].w < last_width) {
            total_height += blocks[i].h;
            last_width = blocks[i].w;
        }
    }
    return total_height;
}

int main() {
    Block* blocks = nullptr;
    int n = 0;
    
    if (!readBlocks(blocks, n)) {
        delete[] blocks;
        return 1;
    }
    
    unsigned long long height = calculateHeight(blocks, n);
    cout << "Максимальная высота пирамиды: " << height << endl;
    
    delete[] blocks;
    return 0;
}