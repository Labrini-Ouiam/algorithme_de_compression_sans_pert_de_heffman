#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;

// Structure pour représenter un nœud de l'arbre Huffman
struct HuffmanNode {
    char data;
    int frequency;
    HuffmanNode *left, *right;

    HuffmanNode(char data, int frequency) : data(data), frequency(frequency), left(nullptr), right(nullptr) {}
};

// Comparateur personnalisé pour la file de priorité
struct Compare {
    bool operator()(HuffmanNode* l, HuffmanNode* r) {
        return l->frequency > r->frequency;
    }
};

// Fonction pour construire l'arbre Huffman et retourner le nœud racine
HuffmanNode* buildHuffmanTree(const unordered_map<char, int>& freqMap) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> minHeap;

    // Création de nœuds pour chaque caractère et les ajouter à la file de priorité
    for (auto pair : freqMap) {
        HuffmanNode* node = new HuffmanNode(pair.first, pair.second);
        minHeap.push(node);
    }

    // Construction de l'arbre Huffman
    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top();
        minHeap.pop();
        HuffmanNode* right = minHeap.top();
        minHeap.pop();

        HuffmanNode* newNode = new HuffmanNode('$', left->frequency + right->frequency);
        newNode->left = left;
        newNode->right = right;

        minHeap.push(newNode);
    }

    return minHeap.top();
}

// Fonction pour parcourir l'arbre Huffman et stocker les codes de compression dans un map
void generateCodes(HuffmanNode* root, string code, unordered_map<char, string>& codes) {
    if (!root)
        return;

    if (root->data != '$')
        codes[root->data] = code;

    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

// Fonction pour compresser le fichier texte en utilisant les codes de Huffman
void compressFile(const string& inputFile, const string& outputFile, const unordered_map<char, string>& codes) {
    ifstream inFile(inputFile);
    ofstream outFile(outputFile, ios::binary);

    char c;
    string buffer = "";
    while (inFile.get(c)) {
        buffer += codes.at(c);
        while (buffer.length() >= 8) {
            char byte = 0;
            for (int i = 0; i < 8; ++i) {
                byte <<= 1;
                byte |= buffer[i] - '0';
            }
            outFile.put(byte);
            buffer = buffer.substr(8);
        }
    }

    // Compléter avec des zéros pour obtenir un nombre de bits multiple de 8
    int remainingBits = 8 - buffer.length();
    if (remainingBits != 8) {
        buffer += string(remainingBits, '0');
        char byte = 0;
        for (int i = 0; i < 8; ++i) {
            byte <<= 1;
            byte |= buffer[i] - '0';
        }
        outFile.put(byte);
    }

    inFile.close();
    outFile.close();
}

int main() {
    string inputFile = "input.txt";
    string outputFile = "output.bin";

    // Calcul de la fréquence des caractères dans le fichier texte
    unordered_map<char, int> freqMap;
    ifstream inFile(inputFile);
    char c;
    while (inFile.get(c))
        freqMap[c]++;

    // Construction de l'arbre Huffman
    HuffmanNode* root = buildHuffmanTree(freqMap);

    // Génération des codes de compression
    unordered_map<char, string> codes;
    generateCodes(root, "", codes);

    // Compression du fichier
    compressFile(inputFile, outputFile, codes);

    cout << "Compression terminée." << endl;

    return 0;
}
