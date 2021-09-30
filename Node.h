//
// Created by jingh on 30/9/2021.
//

#ifndef DATABASE_NODE_H
#define DATABASE_NODE_H
#include <tuple>
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short int uint_s;
using namespace std;

class Node
{
public:
    bool leaf;
    int *key;
    int curSize;
    int maxSize;
    Node **childNode; //ptr to next node

    tuple<uint, void *, uint_s> *keyPtr = (tuple<uint, void *, uint_s>*) childNode;
    childNode[i] =(Node*) keyPtr;
    friend class BTree;

public:
    Node(int nodeSize)
    {
        maxSize = nodeSize;
        key = new int[maxSize];
        childNode = new Node * [maxSize + 1];
    }
};

#endif //DATABASE_NODE_H
