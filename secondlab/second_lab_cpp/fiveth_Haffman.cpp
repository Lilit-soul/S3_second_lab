#include <iostream>
#include <string>

using namespace std;

struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;
};

struct ListNode {
    Node* treeNode;
    ListNode* next;
};

void insertSorted(ListNode*& head, Node* treeNode) {
    ListNode* newNode = new ListNode;
    newNode->treeNode = treeNode;
    newNode->next = nullptr;

    if (!head || head->treeNode->freq >= treeNode->freq) {
        newNode->next = head;
        head = newNode;
        return;
    }

    ListNode* curr = head;
    while (curr->next && curr->next->treeNode->freq < treeNode->freq) {
        curr = curr->next;
    }
    newNode->next = curr->next;
    curr->next = newNode;
}

ListNode* popMin(ListNode*& head) {
    if (!head) return nullptr;
    ListNode* temp = head;
    head = head->next;
    return temp;
}

Node* buildHuffmanTree(const string& text) {
    int freq[256] = {0};
    for (char c : text) freq[(unsigned char)c]++;

    ListNode* head = nullptr;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            Node* n = new Node;
            n->ch = (char)i;
            n->freq = freq[i];
            n->left = nullptr;
            n->right = nullptr;
            insertSorted(head, n);
        }
    }

    while (head && head->next) {
        ListNode* leftList = popMin(head);
        ListNode* rightList = popMin(head);

        Node* parent = new Node;
        parent->ch = '\0';
        parent->freq = leftList->treeNode->freq + rightList->treeNode->freq;
        parent->left = leftList->treeNode;
        parent->right = rightList->treeNode;

        delete leftList;
        delete rightList;

        insertSorted(head, parent);
    }

    if (!head) return nullptr;
    Node* root = head->treeNode;
    delete head;
    return root;
}

void generateCodes(Node* root, string code, string chars[], string codes[]) {
    if (!root) return;
    if (!root->left && !root->right) {
        chars[(unsigned char)root->ch] = root->ch;
        codes[(unsigned char)root->ch] = code;
    }
    generateCodes(root->left, code + "0", chars, codes);
    generateCodes(root->right, code + "1", chars, codes);
}

string encode(const string& text, string codes[]) {
    string result = "";
    for (char c : text) result += codes[(unsigned char)c];
    return result;
}

string decode(Node* root, const string& encoded) {
    string result = "";
    Node* curr = root;
    for (char bit : encoded) {
        if (bit == '0') curr = curr->left;
        else curr = curr->right;

        if (!curr->left && !curr->right) {
            result += curr->ch;
            curr = root;
        }
    }
    return result;
}

void clearTree(Node* root) {
    if (!root) return;
    clearTree(root->left);
    clearTree(root->right);
    delete root;
}

int main() {
    string text;
    cout << "Введите текст для кодирования: ";
    getline(cin, text);

    if (text.empty()) {
        cout << "Error: empty text" << endl;
        return 1;
    }

    Node* root = buildHuffmanTree(text);
    if (!root) return 1;

    string chars[256];
    string codes[256];
    generateCodes(root, "", chars, codes);

    cout << "\nТаблица кодов:\n";
    for (int i = 0; i < 256; i++) {
        if (!codes[i].empty()) {
            cout << chars[i] << ": " << codes[i] << endl;
        }
    }

    string encoded = encode(text, codes);
    cout << "\nЗакодированная строка: " << encoded << endl;

    string decoded = decode(root, encoded);
    cout << "Декодированная строка: " << decoded << endl;

    clearTree(root);
    return 0;
}