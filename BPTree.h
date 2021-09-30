//
// Created by jingh on 30/9/2021.
//

#ifndef DATABASE_BPTREE_H
#define DATABASE_BPTREE_H
#include "Node.h"

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short int uint_s;
using namespace std;

class BPTree
{
private:
    int nodeSize;
    Node* rootNode;

public:
    BPTree(int nodeSize);
    void insertKey(int,tuple<uint, void *, uint_s>*);
    int heightOfTree(Node* cursor);
    void deleteKey(int);

private:
    void insertInternal(int newKey, Node* ptrNode, Node* child);
    Node* findParent(Node*, Node*);
    void deleteInternal(int deleteKey, Node* ptrNode, Node* child);
};


#endif //DATABASE_BPTREE_H