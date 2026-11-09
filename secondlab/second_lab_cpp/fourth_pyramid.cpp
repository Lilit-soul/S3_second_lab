#include <iostream>
#include <string>
#include <cstdio>
#include <sstream>

using namespace std;

struct Block {
    int w, h;
};

struct DynamicArray {
    Block* data;
    int size;
    int capacity;
};

void initArray(DynamicArray& arr, int initialCapacity) {
    arr.capacity = initialCapacity;
    arr.size = 0;
    arr.data = new Block[arr.capacity];
}

void pushArray(DynamicArray& arr, Block b) {
    if (arr.size >= arr.capacity) {
        int newCap = arr.capacity * 2;
        Block* newData = new Block[newCap];
        for (int i = 0; i < arr.size; i++) {
            newData[i] = arr.data[i];
        }
        delete[] arr.data;
        arr.data = newData;
        arr.capacity = newCap;
    }
    arr.data[arr.size] = b;
    arr.size++;
}

void clearArray(DynamicArray& arr) {
    delete[] arr.data;
    arr.data = nullptr;
    arr.size = 0;
    arr.capacity = 0;
}

void sortBlocks(DynamicArray& arr) {
    int n = arr.size;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr.data[j].w < arr.data[j+1].w || 
               (arr.data[j].w == arr.data[j+1].w && arr.data[j].h < arr.data[j+1].h)) {
                Block temp = arr.data[j];
                arr.data[j] = arr.data[j+1];
                arr.data[j+1] = temp;
            }
        }
    }
}

bool readBlocks(DynamicArray& arr) {
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

    int n;
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

    initArray(arr, n);
    cout << "Введите ширину и высоту каждого блока (через пробел):\n";
    
    for (int i = 0; i < n; i++) {
        string line;
        if (!getline(cin, line)) {
            cout << "Error: invalid block data" << endl;
            clearArray(arr);
            return false;
        }
        
        bool lineEmpty = true;
        for (char c : line) {
            if (c != ' ' && c != '\t') { lineEmpty = false; break; }
        }
        if (lineEmpty) {
            cout << "Error: empty line for block data" << endl;
            clearArray(arr);
            return false;
        }

        istringstream iss(line);
        int w, h;
        string extra;
        if (!(iss >> w >> h) || (iss >> extra)) {
            cout << "Error: invalid block data format" << endl;
            clearArray(arr);
            return false;
        }
        
        if (w <= 0 || h <= 0) {
            cout << "Error: block dimensions must be positive" << endl;
            clearArray(arr);
            return false;
        }
        
        pushArray(arr, {w, h});
    }
    return true;
}

unsigned long calculateHeight(DynamicArray& arr) {
    sortBlocks(arr);
    unsigned long total_height = 0;
    int last_width = 2147483647;

    for (int i = 0; i < arr.size; i++) {
        if (arr.data[i].w < last_width) {
            total_height += arr.data[i].h;
            last_width = arr.data[i].w;
        }
    }
    return total_height;
}

int main() {
    DynamicArray blocks;
    initArray(blocks, 10);
    
    if (!readBlocks(blocks)) {
        clearArray(blocks);
        return 1;
    }
    
    unsigned long height = calculateHeight(blocks);
    cout << "Максимальная высота пирамиды: " << height << endl;
    
    clearArray(blocks);
    return 0;
}