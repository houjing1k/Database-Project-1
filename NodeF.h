//
// Created by jingh on 30/9/2021.
//

#ifndef DATABASE_NODEF_H
#define DATABASE_NODEF_H

#include <tuple>

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short int uint_s;
using namespace std;

class NodeF {
public:
    char *nodeData;

public:
    NodeF(size_t);

    NodeF(int);

    bool isLeaf();

    void setLeaf(bool);

    uint_s getMaxSize();

    uint_s getCurSize();

    void setCurSize(uint_s);

    uint getKey(uint index);

    void setKey(uint index, uint key);

    NodeF *getChildNode(uint index);

    void setChildNode(uint index, NodeF *childNode);

private:
    void setMaxSize(uint_s);

};

#endif //DATABASE_NODEF_H
