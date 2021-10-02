//
// Created by jingh on 30/9/2021.
//

#ifndef DATABASE_BPTREE_H
#define DATABASE_BPTREE_H
#include "Node.h"
#include "NodeF.h"
#include<vector>

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short int uint_s;
using namespace std;

class BPTree
{
private:
    int nodeSize;

public :
    Node* rootNode;

public:
    BPTree(int nodeSize);
    Node* newNode(size_t nodeSize);
    void insertKey(int,tuple<uint, void *, uint_s>*);
    int heightOfTree(Node* cursor);
    void deleteKey(int);
    Node* searchForNode(int key);
    vector<tuple<uint, void *, uint_s>> searchForRange(int start, int end);
    void printTree(Node*);
    void printNode(Node* node, string label);
    void printTreeStats();
    uint getNumOfNodes(Node *);

private:
    void insertInternal(int newKey, Node* ptrNode, Node* child);
    Node* findParent(Node*, Node*);
    void deleteInternal(int deleteKey, Node* ptrNode, Node* child);
    void updateTreeAftDelete(int deleteKey, int newKey);
    int getMinKey(Node* ptrNode);
};


#endif //DATABASE_BPTREE_H
