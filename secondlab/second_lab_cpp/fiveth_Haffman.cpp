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
    ListNode* newNode = new ListNode{treeNode, nullptr};
    if (!head || head->treeNode->freq >= treeNode->freq) {
        newNode->next = head;
        head = newNode;
        return;
    }
    ListNode* curr = head;
    while (curr->next && curr->next->treeNode->freq < treeNode->freq)
        curr = curr->next;
    newNode->next = curr->next;
    curr->next = newNode;
}

ListNode* popMin(ListNode*& head) {
    if (!head) return nullptr;
    ListNode* t = head;
    head = head->next;
    return t;
}

Node* buildHuffmanTree(const string& text) {
    int freq[256] = {0};
    for (char c : text) freq[(unsigned char)c]++;

    ListNode* head = nullptr;
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            Node* n = new Node{(char)i, freq[i], nullptr, nullptr};
            insertSorted(head, n);
        }
    }

    if (head && !head->next) {
        Node* leaf = head->treeNode;
        delete head;
        Node* root = new Node{'\0', leaf->freq, leaf, nullptr}; 
        return root;
    }

    while (head && head->next) {
        ListNode* l = popMin(head);
        ListNode* r = popMin(head);
        Node* parent = new Node{'\0',
                                l->treeNode->freq + r->treeNode->freq,
                                l->treeNode, r->treeNode};
        delete l;
        delete r;
        insertSorted(head, parent);
    }

    if (!head) return nullptr;
    Node* root = head->treeNode;
    delete head;
    return root;
}

void generateCodes(Node* root, const string& code, string codes[], bool hasCode[]) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[(unsigned char)root->ch] = code;
        hasCode[(unsigned char)root->ch] = true;
        return;
    }
    generateCodes(root->left, code + "0", codes, hasCode);
    generateCodes(root->right, code + "1", codes, hasCode);
}

string encode(const string& text, string codes[]) {
    string result;
    for (char c : text) result += codes[(unsigned char)c];
    return result;
}

string decode(Node* root, const string& encoded) {
    if (!root) return "";
    if (!root->left && !root->right) {
        string res;
        for (size_t i = 0; i < encoded.size(); ++i) res += root->ch;
        return res;
    }
    string result;
    Node* curr = root;
    for (char bit : encoded) {
        if (!curr) return "";
        curr = (bit == '0') ? curr->left : curr->right;
        if (!curr) return "";
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
    cout << "Введите текст для кодирования: ";
    string text;
    getline(cin, text);

    if (text.empty()) {
        cout << "Error: empty text\n";
        return 1;
    }

    Node* root = buildHuffmanTree(text);
    if (!root) return 1;

    string codes[256];
    bool hasCode[256] = {false};
    generateCodes(root, "", codes, hasCode);

    cout << "\nТаблица кодов:\n";
    for (int i = 0; i < 256; ++i) {
        if (hasCode[i]) {
            cout << (char)i << ": " << (codes[i].empty() ? "<empty>" : codes[i]) << "\n";
        }
    }

    string encoded = encode(text, codes);
    cout << "\nЗакодированная строка: " << encoded << "\n";

    string decoded = decode(root, encoded);
    cout << "Декодированная строка: " << decoded << "\n";

    clearTree(root);
    return 0;
}